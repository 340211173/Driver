#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "extnddrv.h"

#include "..\..\include\undocnt.h"


/* Prototypes for the services to be added */
NTSTATUS SampleService0(void);
NTSTATUS SampleService1(int param1);
NTSTATUS SampleService2(int param1, int param2);
/* TODO TODO TODO TODO
..............
..............
Add more to this list to add more services 
*/
	

/* Table describing the new services */
unsigned int ServiceTableBase[]={(unsigned int)SampleService0,
				(unsigned int)SampleService1,
				(unsigned int)SampleService2,
/* TODO TODO TODO TODO
..............
..............
Add more to this list to add more services 
*/
				};

/* Table describing the parameter bytes required for the new services */
unsigned char ParamTableBase[]={0,
			       4,
			       8,
/* TODO TODO TODO TODO
..............
..............
Add more parameter bytes to this list to add more services 
*/
			      };



unsigned int *NewServiceTableBase; /* Pointer to new SSDT */
unsigned char *NewParamTableBase; /* Pointer to new SSPT */
unsigned int NewNumberOfServices; /* New number of services */

unsigned int StartingServiceId;

NTSTATUS SampleService0(void)
{
	DbgPrint("Kernel service with 0 parameters called\n");
	return STATUS_SUCCESS;
}

NTSTATUS SampleService1(int param1)
{
	DbgPrint("Kernel service with 1 parameters called\n");
	DbgPrint("param1=%x\n", param1);
	return STATUS_SUCCESS+1;
}

NTSTATUS SampleService2(int param1, int param2)
{
	DbgPrint("Kernel service with 2 parameters called\n");
	DbgPrint("param1=%x param2=%x\n", param1, param2);
	return STATUS_SUCCESS+2;
}

/* TODO TODO TODO TODO
..............
..............
Add implementations of other services here 
*/

unsigned int GetAddrssofShadowTable()
{
	int i;
	unsigned char *p;
	unsigned int dwordatbyte;

	p=(unsigned char *)KeAddSystemServiceTable;

	for (i=0; i<4096; i++, p++) {
		__try {
			dwordatbyte=*(unsigned int *)p;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return 0;
		}

		if (MmIsAddressValid((PVOID)dwordatbyte)) {
			if (memcmp((PVOID)dwordatbyte, &KeServiceDescriptorTable, 16)==0) {
				if ((PVOID)dwordatbyte==&KeServiceDescriptorTable) {
					continue;
				}
				return dwordatbyte;
			}
		}
	}
	return 0;
}


NTSTATUS AddServices()
{
    PServiceDescriptorTableEntry_t KeServiceDescriptorTableShadow;
    unsigned int NumberOfServices;

    NumberOfServices=sizeof(ServiceTableBase)/sizeof(ServiceTableBase[0]);

    trace(("KeServiceDescriptorTable=%x\n", &KeServiceDescriptorTable));

    KeServiceDescriptorTableShadow=(PServiceDescriptorTableEntry_t)GetAddrssofShadowTable();

    if (KeServiceDescriptorTableShadow==NULL) {
	return STATUS_UNSUCCESSFUL;
    }
    
    trace(("KeServiceDescriptorTableShadow=%x\n", KeServiceDescriptorTableShadow));
    
    NewNumberOfServices=KeServiceDescriptorTable.NumberOfServices+NumberOfServices;

    StartingServiceId=KeServiceDescriptorTable.NumberOfServices;

    /* Allocate suffcient memory to hold the exising services as well as
    the services you want to add */
    NewServiceTableBase=(unsigned int *) ExAllocatePool(PagedPool, NewNumberOfServices*sizeof(unsigned int));
    if (NewServiceTableBase==NULL) {
	return STATUS_INSUFFICIENT_RESOURCES;
    }

    NewParamTableBase=(unsigned char *) ExAllocatePool(PagedPool, NewNumberOfServices);
    if (NewParamTableBase==NULL) {
	ExFreePool(NewServiceTableBase);
	return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Backup the exising SSDT and SSPT */
    memcpy(NewServiceTableBase, KeServiceDescriptorTable.ServiceTableBase, 
		KeServiceDescriptorTable.NumberOfServices*sizeof(unsigned int));
    memcpy(NewParamTableBase, KeServiceDescriptorTable.ParamTableBase, 
		KeServiceDescriptorTable.NumberOfServices);

    /* Append to it new SSDT and SSPT */
    memcpy(NewServiceTableBase+KeServiceDescriptorTable.NumberOfServices, 
		ServiceTableBase, sizeof(ServiceTableBase));
    memcpy(NewParamTableBase+KeServiceDescriptorTable.NumberOfServices, 
		ParamTableBase, sizeof(ParamTableBase));

    /* Modify the KeServiceDescriptorTableEntry to point to new SSDT and SSPT */
    KeServiceDescriptorTable.ServiceTableBase=NewServiceTableBase;
    KeServiceDescriptorTable.ParamTableBase=NewParamTableBase;
    KeServiceDescriptorTable.NumberOfServices=NewNumberOfServices;

    /* Also update the KeServiceDescriptorTableShadow to point to new SSDT and SSPT */
    KeServiceDescriptorTableShadow->ServiceTableBase=NewServiceTableBase;
    KeServiceDescriptorTableShadow->ParamTableBase=NewParamTableBase;
    KeServiceDescriptorTableShadow->NumberOfServices=NewNumberOfServices;

    
    /* Return Success */
    return STATUS_SUCCESS;
}


NTSTATUS
DriverDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    );


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    MYDRIVERENTRY(L"extnddrv", FILE_DEVICE_EXTNDDRV, AddServices());
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
    NTSTATUS            ntStatus;

    Irp->IoStatus.Status      = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_DEVICE_CONTROL:
	trace(("EXTNDDRV.SYS: IRP_MJ_CLOSE\n"));
	switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
	{
		case IOCTL_EXTNDDRV_GET_STARTING_SERVICEID:

		trace(("EXTNDDRV.SYS: IOCTL_EXTNDDRV_GET_STARTING_SERVICEID\n"));
		outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
		if (outputBufferLength<sizeof(StartingServiceId)) {
			Irp->IoStatus.Status      = STATUS_INSUFFICIENT_RESOURCES;
		} else {
			ioBuffer     = (PULONG)Irp->AssociatedIrp.SystemBuffer;
			memcpy(ioBuffer, &StartingServiceId, sizeof(StartingServiceId));
			Irp->IoStatus.Information = sizeof(StartingServiceId);
		}
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
    WCHAR                  deviceLinkBuffer[]  = L"\\DosDevices\\EXTNDDRV";
    UNICODE_STRING         deviceLinkUnicodeString;


    RtlInitUnicodeString (&deviceLinkUnicodeString,
			  deviceLinkBuffer
			  );

    IoDeleteSymbolicLink (&deviceLinkUnicodeString);


    IoDeleteDevice (DriverObject->DeviceObject);

    trace(("EXTNDDRV.SYS: unloading\n"));
}
