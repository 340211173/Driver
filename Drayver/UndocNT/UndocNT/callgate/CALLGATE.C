#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "callgate.h"

#include "..\include\intel.h"
#include "..\include\undocnt.h"

/* This function creates a callgate on request from the application and 
returns the callgate to the application, which the application can use
to run privileged instructions from user level application */
NTSTATUS CreateCallGate(PCallGateInfo_t CallGateInfo)
{
    static CALLGATE_DESCRIPTOR callgate_desc;
    static CODE_SEG_DESCRIPTOR ring0_desc;
    unsigned short SelectorArray[2];
    NTSTATUS rc;
    #define LOWORD(l)           ((unsigned short)(unsigned int)(l))
    #define HIWORD(l)           ((unsigned short)((((unsigned int)(l)) >> 16) & 0xFFFF))

    rc=KeI386AllocateGdtSelectors(SelectorArray, 0x02);
    if (rc!=STATUS_SUCCESS) {
        trace(("Unable to allocate selectors from GDT\n"));
        return rc;
    }

    trace(("Selectors allocated = %x %x\n", SelectorArray[0], SelectorArray[1]));
    
    /* Fill the descriptor according to the requirements of the code descriptor */
    ring0_desc.limit_0_15 = 0xFFFF;
    ring0_desc.base_0_15 = 0;
    ring0_desc.base_16_23 = 0;
    ring0_desc.readable = 1;
    ring0_desc.conforming = 0;
    ring0_desc.code_data = 1;
    ring0_desc.app_system = 1;
    ring0_desc.dpl = 0;
    ring0_desc.present = 1;
    ring0_desc.limit_16_19 = 0xF;
    ring0_desc.always_0 = 0;
    ring0_desc.seg_16_32 = 1;
    ring0_desc.granularity = 1;
    ring0_desc.base_24_31 = 0;

    /* Fill the descriptor according to the requirements of the call gate descriptor */
    callgate_desc.offset_0_15 = LOWORD( CallGateInfo->FunctionLinearAddress );
    callgate_desc.selector = SelectorArray[0];
    callgate_desc.param_count = CallGateInfo->NumberOfParameters;
    callgate_desc.some_bits = 0;
    callgate_desc.type = 0xC;          // 386 call gate
    callgate_desc.app_system = 0;      // A system descriptor
    callgate_desc.dpl = 3;             // Ring 3 code can call
    callgate_desc.present = 1;
    callgate_desc.offset_16_31 = HIWORD(CallGateInfo->FunctionLinearAddress);

    /* Return to the caller application the selectors allocated, caller
    is only interested in CallGateSelector */
    CallGateInfo->CodeSelector=SelectorArray[0];
    CallGateInfo->CallGateSelector=SelectorArray[1];

    /* Set the descriptor entry for code selector */
    rc=KeI386SetGdtSelector(SelectorArray[0], &ring0_desc);
    if (rc!=STATUS_SUCCESS) {
        trace(("SetGdtSelector=%x\n", rc));
        KeI386ReleaseGdtSelectors(SelectorArray, 0x02);        
        return rc;
    }

    /* Set the descriptor entry for call gate selector */
    rc=KeI386SetGdtSelector(SelectorArray[1], &callgate_desc);
    if (rc!=STATUS_SUCCESS) {
        trace(("SetGdtSelector=%x\n", rc));
        KeI386ReleaseGdtSelectors(SelectorArray, 0x02);        
        return rc;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/* This function releases the previously allocated callgate */
NTSTATUS ReleaseCallGate(PCallGateInfo_t CallGateInfo)
{
        unsigned short SelectorArray[2];
        int rc;

        SelectorArray[0]=CallGateInfo->CodeSelector;
        SelectorArray[1]=CallGateInfo->CallGateSelector;

        rc=KeI386ReleaseGdtSelectors(SelectorArray, 0x02);
        if (rc!=STATUS_SUCCESS) {
                trace(("ReleaseGDTSelectors failed, rc=%x\n", rc));
        }
        return rc;
}

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    MYDRIVERENTRY(DRIVER_DEVICE_NAME, FILE_DEVICE_CALLGATE, ntStatus);
    return ntStatus;
}


NTSTATUS
DriverDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{
    PIO_STACK_LOCATION  irpStack;
    PVOID               ioBuffer;
    ULONG               inputBufferLength;
    ULONG               outputBufferLength;
    ULONG               ioControlCode;
    NTSTATUS            ntStatus;

    Irp->IoStatus.Status      = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_DEVICE_CONTROL:
        trace(("CALLGATE.SYS: IRP_MJ_DEVICE_CONTROL\n"));
        ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

        switch (ioControlCode)
        {
        case IOCTL_CALLGATE_CREATE:
        {
            PCallGateInfo_t CallGateInfo;

            CallGateInfo=(PCallGateInfo_t)ioBuffer;
            Irp->IoStatus.Status=CreateCallGate(CallGateInfo);
            trace(("CreateCallGate rc=%x\n", Irp->IoStatus.Status));
            if (Irp->IoStatus.Status==STATUS_SUCCESS) {
                Irp->IoStatus.Information = sizeof(CallGateInfo_t);
            }
            break;
        }
        case IOCTL_CALLGATE_RELEASE:
        {
            PCallGateInfo_t CallGateInfo;

            CallGateInfo=(PCallGateInfo_t)ioBuffer;
            ntStatus=ReleaseCallGate(CallGateInfo);
            trace(("ReleaseCallGate rc=%x\n", ntStatus));
            break;
        }

        default:

            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            trace(("CALLGATE.SYS: unknown IRP_MJ_DEVICE_CONTROL\n"));
            break;
        }

        break;
    }
    ntStatus = Irp->IoStatus.Status;
    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    return ntStatus;
}



VOID
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    WCHAR                  deviceLinkBuffer[]  = L"\\DosDevices\\"DRIVER_DEVICE_NAME;
    UNICODE_STRING         deviceLinkUnicodeString;

    RtlInitUnicodeString (&deviceLinkUnicodeString,
                          deviceLinkBuffer
                          );
    IoDeleteSymbolicLink (&deviceLinkUnicodeString);
    IoDeleteDevice (DriverObject->DeviceObject);
    trace(("CALLGATE.SYS: unloading\n"));
}
