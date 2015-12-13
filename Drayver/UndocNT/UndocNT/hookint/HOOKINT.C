#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "Hookint.h"

#include "..\include\intel.h"
#include "..\include\undocnt.h"


/* Interrupt to be hooked */
#define HOOKINT 0x2E

int OldHandler;
ULONG *ServiceCounterTable;
ULONG ServiceCounterTableSize;
int NumberOfServices;


extern void _cdecl NewHandler();

/* Buffer to store result of sidt instruction */
char buffer[6];

/* Pointer to structure to identify the limit and base of IDTR*/
PIdtr_t Idtr=(PIdtr_t)buffer;

#pragma pack()

void NewHandlerCFunc(int ServiceId)
{
        if (ServiceId>NumberOfServices)
                return;
        ServiceCounterTable[ServiceId+1]++;
        return;
}


NTSTATUS DriverSpecificInitialization()
{
    PIdtEntry_t            IdtEntry;

    NumberOfServices=KeServiceDescriptorTable.NumberOfServices;
    ServiceCounterTableSize=(NumberOfServices+1)*sizeof(int);
    ServiceCounterTable=ExAllocatePool(PagedPool, ServiceCounterTableSize);

    if (!ServiceCounterTable) {
         return STATUS_INSUFFICIENT_RESOURCES;
    }

    memset(ServiceCounterTable, 0, ServiceCounterTableSize);
    *ServiceCounterTable=NumberOfServices;

    trace(("NumberOfServices=%x, ServiceCounterTableSize=%x, @%x\n", 
        NumberOfServices, ServiceCounterTableSize, ServiceCounterTable));

    /* Get the Base and Limit of IDTR Register */
    _asm sidt buffer
    IdtEntry=(PIdtEntry_t)Idtr->Base;

    /* Index the interrupt number to be hooked in appropriate IDT
    entry and save away the Old handler's address */
    OldHandler=((unsigned int)IdtEntry[HOOKINT].OffsetHigh<<16U)|
                        (IdtEntry[HOOKINT].OffsetLow);

    /* Plug into the interrupt by changing the offset field to point
    to NewHandler function */
    _asm cli
    IdtEntry[HOOKINT].OffsetLow=(unsigned short)NewHandler;
    IdtEntry[HOOKINT].OffsetHigh=(unsigned short)((unsigned int)NewHandler>>16);
    _asm sti

    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    MYDRIVERENTRY(DRIVER_DEVICE_NAME , FILE_DEVICE_HOOKINT, DriverSpecificInitialization());
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
        trace(("HOOKINT.SYS: IRP_MJ_DEVICE_CONTROL\n"));

        ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

        switch (ioControlCode)
        {

        case IOCTL_HOOKINT_SYSTEM_SERVICE_USAGE:
        {
            int i;

            /* Check if sufficient sized buffer is provided to hold
            the counters for system service usage */
            if (outputBufferLength>=ServiceCounterTableSize) {
                /* Output the counters describing the system service usage */
                trace((for (i=1; i<=NumberOfServices; i++)
                        DbgPrint("%x ", ServiceCounterTable[i])));
                trace((DbgPrint("\n")));

                /* Copy the counter information in user supplied buffer */
                memcpy(ioBuffer, ServiceCounterTable, 
                        ServiceCounterTableSize);

                /* Fill in the number of bytes to be returned to the caller */
                Irp->IoStatus.Information = ServiceCounterTableSize;
            } else {
                Irp->IoStatus.Status      = STATUS_INSUFFICIENT_RESOURCES;
            }
            break;
        }

        default:

            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;

            trace(("HOOKINT.SYS: unknown IRP_MJ_DEVICE_CONTROL\n"));

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
    PIdtEntry_t            IdtEntry;

    ExFreePool(ServiceCounterTable);

    /* Reach to IDT */
    IdtEntry=(PIdtEntry_t)Idtr->Base;

    /* Unplug the interrupt by replacing the offset field in the Interrupt
    Gate Descriptor by the old handler address */
    _asm cli
    IdtEntry[HOOKINT].OffsetLow=(unsigned short)OldHandler;
    IdtEntry[HOOKINT].OffsetHigh=(unsigned short)((unsigned int)OldHandler>>16);
    _asm sti

    RtlInitUnicodeString (&deviceLinkUnicodeString,
                          deviceLinkBuffer
                          );

    IoDeleteSymbolicLink (&deviceLinkUnicodeString);
    IoDeleteDevice (DriverObject->DeviceObject);

    trace(("HOOKINT.SYS: unloading\n"));
}
