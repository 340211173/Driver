#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "pagedirdrv.h"

#include "..\include\undocnt.h"

void DisplayPageDirectory(void *Peb)
{
	unsigned int *PageDirectory=(unsigned int *)0xC0300000;
	int i;
	int ctr=0;

	KeAttachProcess(Peb);

	for (i=0; i<1024; i++) {
		if (PageDirectory[i]&0x01) {
			if ((ctr%8)==0)
				DbgPrint("  \n");
			DbgPrint("%08x ", PageDirectory[i]&0xFFFFF000);
			ctr++;
		}
	}
	DbgPrint("\n\n");

	KeDetachProcess();
}


void DisplayPageDirectoryForAllProcesses()
{
	PLIST_ENTRY ProcessListHead, ProcessListPtr;
	ULONG BuildNumber;
	ULONG ListEntryOffset;
	ULONG NameOffset;

	BuildNumber=NtBuildNumber & 0x0000FFFF;
	if ((BuildNumber==0x421) || (BuildNumber==0x565)) { // NT 3.51 or NT 4.0
		ListEntryOffset=0x98;
		NameOffset=0x1DC;
	} else if (BuildNumber==0x755) {// Windows 2000 beta2 
		ListEntryOffset=0xA0;
		NameOffset=0x1FC;
	} else {
		DbgPrint("Unsupported NT Version\n");
		return;
	}
	
	ProcessListHead=ProcessListPtr=(PLIST_ENTRY)(((char *)PsInitialSystemProcess)+ListEntryOffset);
	while (ProcessListPtr->Flink!=ProcessListHead) {
		void *Peb;
		char ProcessName[16];

		Peb=(void *)(((char *)ProcessListPtr)-ListEntryOffset);
		memset(ProcessName, 0, sizeof(ProcessName));
		memcpy(ProcessName, ((char *)Peb)+NameOffset, 16);

		DbgPrint("**%s Peb @%x**  ", ProcessName, Peb);
		DisplayPageDirectory(Peb);
		ProcessListPtr=ProcessListPtr->Flink;
	} 
}


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    MYDRIVERENTRY(DRIVER_DEVICE_NAME, FILE_DEVICE_PAGEDIRDRV, STATUS_SUCCESS);
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


    //
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    //

    irpStack = IoGetCurrentIrpStackLocation (Irp);


    //
    // Get the pointer to the input/output buffer and it's length
    //

    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;



    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_CREATE:
    case IRP_MJ_CLOSE: break;

    case IRP_MJ_DEVICE_CONTROL:

	ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

	switch (ioControlCode)
	{

	case IOCTL_PAGEDIRDRV_DUMPPAGEDIR:
		DisplayPageDirectoryForAllProcesses();
		break;

	default:
	    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
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

    //
    // Delete the device object
    //

    IoDeleteDevice (DriverObject->DeviceObject);
}
