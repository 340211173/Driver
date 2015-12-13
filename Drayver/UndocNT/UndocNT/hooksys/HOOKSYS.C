#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "hooksys.h"

#include "..\..\include\undocnt.h"

typedef NTSTATUS (*NTCREATEFILE)(
            PHANDLE FileHandle,
            ACCESS_MASK DesiredAccess,
            POBJECT_ATTRIBUTES ObjectAttributes,
            PIO_STATUS_BLOCK IoStatusBlock,
            PLARGE_INTEGER AllocationSize OPTIONAL,
            ULONG FileAttributes,
            ULONG ShareAccess,
            ULONG CreateDisposition,
            ULONG CreateOptions,
            PVOID EaBuffer OPTIONAL,
            ULONG EaLength
);

#define SYSTEMSERVICE(_function)  KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]
NTCREATEFILE OldNtCreateFile;

NTSTATUS NewNtCreateFile(
                    PHANDLE FileHandle,
                    ACCESS_MASK DesiredAccess,
                    POBJECT_ATTRIBUTES ObjectAttributes,
                    PIO_STATUS_BLOCK IoStatusBlock,
                    PLARGE_INTEGER AllocationSize OPTIONAL,
                    ULONG FileAttributes,
                    ULONG ShareAccess,
                    ULONG CreateDisposition,
                    ULONG CreateOptions,
                    PVOID EaBuffer OPTIONAL,
                    ULONG EaLength)
{
        int rc;
        char ParentDirectory[1024];
        PUNICODE_STRING Parent=NULL;

        ParentDirectory[0]='\0';
        if (ObjectAttributes->RootDirectory!=0) {
                PVOID Object;

                Parent=(PUNICODE_STRING)ParentDirectory;
                rc=ObReferenceObjectByHandle(ObjectAttributes->RootDirectory,
                                             0,
                                             0,
                                             KernelMode,
                                             &Object,
                                             NULL);
                if (rc==STATUS_SUCCESS) {
                        extern NTSTATUS ObQueryNameString(void *, void *, int size,
                                                          int *);
                        int BytesReturned;

                        rc=ObQueryNameString(Object,
                                          ParentDirectory,
                                          sizeof(ParentDirectory),
                                          &BytesReturned);
                        ObDereferenceObject(Object);

                        if (rc!=STATUS_SUCCESS)
                                RtlInitUnicodeString(Parent, L"Unknown\\");
                } else {
                                RtlInitUnicodeString(Parent, L"Unknown\\");
                }
        }

        DbgPrint("NtCreateFile : Filename = %S%S%S\n", Parent?Parent->Buffer:L"",
                        Parent?L"\\":L"", ObjectAttributes->ObjectName->Buffer);
        rc=((NTCREATEFILE)(OldNtCreateFile)) (
                        FileHandle,
                        DesiredAccess,
                        ObjectAttributes,
                        IoStatusBlock,
                        AllocationSize,
                        FileAttributes,
                        ShareAccess,
                        CreateDisposition,
                        CreateOptions,
                        EaBuffer,
                        EaLength);
        DbgPrint("NtCreateFile : rc = %x\n", rc);
        return rc;
}

NTSTATUS HookServices()
{
        OldNtCreateFile=(NTCREATEFILE)(SYSTEMSERVICE(ZwCreateFile));
        _asm cli
        (NTCREATEFILE)(SYSTEMSERVICE(ZwCreateFile))=NewNtCreateFile;
        _asm sti
        return STATUS_SUCCESS;
}

void UnHookServices()
{
        _asm cli
        (NTCREATEFILE)(SYSTEMSERVICE(ZwCreateFile))=OldNtCreateFile;
        _asm sti
        return;
}


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
        MYDRIVERENTRY(DRIVER_DEVICE_NAME,
                        FILE_DEVICE_HOOKSYS,
                        HookServices());
        return ntStatus;
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

    UnHookServices();

    RtlInitUnicodeString (&deviceLinkUnicodeString,
                          deviceLinkBuffer
                          );

    IoDeleteSymbolicLink (&deviceLinkUnicodeString);
    IoDeleteDevice (DriverObject->DeviceObject);
}

