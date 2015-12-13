#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "addint.h"

#include "..\include\intel.h"
#include "..\include\undocnt.h"

/* Old Idt Entry */
IdtEntry_t OldIdtEntry;

/* Interrupt Handler */
extern void _cdecl InterruptHandler();

/* Buffer to store result of sidt instruction */
char buffer[6];

/* Pointer to structure to identify the limit and base of IDTR*/
PIdtr_t Idtr=(PIdtr_t)buffer;


void _cdecl CFunc()
{
}

NTSTATUS AddInterrupt()
{
    PIdtEntry_t            IdtEntry;

    /* Get the Base and Limit of IDTR Register */
    _asm sidt buffer
    IdtEntry=(PIdtEntry_t)Idtr->Base;

    if ((IdtEntry[ADDINT].OffsetLow!=0)||(IdtEntry[ADDINT].OffsetHigh!=0))
        return STATUS_UNSUCCESSFUL;

    /* Save away the old IDT entry */
    memcpy(&OldIdtEntry, &IdtEntry[ADDINT], sizeof(OldIdtEntry));
 

    _asm cli
    /* Initialize the IDT entry according to the interrupt gate requirement */
    IdtEntry[ADDINT].OffsetLow=(unsigned short)InterruptHandler;
    IdtEntry[ADDINT].Selector=8;
    IdtEntry[ADDINT].Reserved=0;
    IdtEntry[ADDINT].Type=0xE;
    IdtEntry[ADDINT].Always0=0;
    IdtEntry[ADDINT].Dpl=3;
    IdtEntry[ADDINT].Present=1;
    IdtEntry[ADDINT].OffsetHigh=(unsigned short)((unsigned int)InterruptHandler>>16);
    _asm sti

    return STATUS_SUCCESS;
}


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    MYDRIVERENTRY(DRIVER_DEVICE_NAME, FILE_DEVICE_ADDINT, AddInterrupt());
    return ntStatus;
}

void RemoveInterrupt()
{
    PIdtEntry_t            IdtEntry;

    /* Reach to IDT */
    IdtEntry=(PIdtEntry_t)Idtr->Base;

    _asm cli
    /* Restore the old IdtEntry */
    memcpy(&IdtEntry[ADDINT], &OldIdtEntry, sizeof(OldIdtEntry));
    _asm sti
}


NTSTATUS
DriverDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{
    Irp->IoStatus.Status      = STATUS_SUCCESS;
    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );
    return Irp->IoStatus.Status;
}



VOID
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    WCHAR                  deviceLinkBuffer[]  = L"\\DosDevices\\"DRIVER_DEVICE_NAME;
    UNICODE_STRING         deviceLinkUnicodeString;

    RemoveInterrupt();
    RtlInitUnicodeString (&deviceLinkUnicodeString,
                          deviceLinkBuffer
                          );

    IoDeleteSymbolicLink (&deviceLinkUnicodeString);
    IoDeleteDevice (DriverObject->DeviceObject);

    trace(("ADDINT.SYS: unloading\n"));
}
