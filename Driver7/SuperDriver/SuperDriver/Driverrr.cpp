#include "ntddk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "DriverH.h"


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

//����������� IO ��������� � ��������
NTSTATUS CreateMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS ReadMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS WriteMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS CloseMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
//System
NTSTATUS ShutDownMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS CleanUpMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS IoCtlMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

//Exit Point
NTSTATUS MyDriverUnload(IN PDRIVER_OBJECT DriverObject);

//Labz Fints
NTSTATUS WriteToFile(IN char myString[]);//������������ ������ � ������� �� �����
BOOLEAN FindProcess(IN char ProcessName[]);//���� �������
void NotifyRoutine(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create);


MegaDriverData gData; //���������� � ������� �������� ���������, ���������� �� �������� ����� (2-3 ������)
BOOLEAN gInited; //����, ������� ���������� ������ �� ��������� � ����������� (��� ���� ��)
HANDLE gThreadHandle; //����� ������, ������� ����� ���������, ��� ������ ���������
PKEVENT gEvent;//id ������������ ������
HANDLE ghProcessHandle;//����� ����� ������
//--------------------
char FindBuffer[256];
typedef ULONG DWORD;
typedef VOID* LPVOID;
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L) //��� ������, ������� ���������� ZwQuerySystemInformation
//���� ����� �� ��������� ����������� �������
//� ����� ������ ���� �������� ����� ��������
//#define SystemProcessesAndThreadsInformation 5 // ���� ��� ������(�����������) ����




//==================================================================================
//===== ��������� ����� � �������
//==================================================================================
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	
	UNICODE_STRING NameString, LinkString, ProcessEventName;
	PDEVICE_OBJECT DeviceObject;//���������� ������� ����������
	NTSTATUS Status;//��� �������
		PEXAMPLE_DEVICE_EXTENSION dx;
	PVOID pvWaitEvents[1];
	OBJECT_ATTRIBUTES oaUserEvent;
	gInited = FALSE;
	DbgPrint(" >MegaDriver: we are inda DriverEntry"); //��������� DbgView - �� ����������� DbgPrint - ����� ������ ��� ����������


	//================================================================================================
	//===== ������ ��������� �� �������
	//================================================================================================
	DriverObject->DriverUnload = (PDRIVER_UNLOAD)MyDriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateMyDriver;//CreateFile
	DriverObject->MajorFunction[IRP_MJ_READ] = ReadMyDriver; //readfile
	DriverObject->MajorFunction[IRP_MJ_WRITE] = WriteMyDriver; //write...
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseMyDriver; //CloseFile
	//DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = ShutDownMyDriver;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoCtlMyDriver;


	//=================================================================================================
	//===== ������ ������
	//=================================================================================================
	//�������� ��� ������ ��������
	RtlInitUnicodeString(&NameString, L"\\Device\\MegaDriver");
	//������ ������
	Status = IoCreateDevice(DriverObject, sizeof(65533), &NameString, 0, 0, FALSE, &DeviceObject);
	if(!NT_SUCCESS(Status)) 
	{
		DbgPrint(" >MegaDriver: DriverEntry-> IoCreateDevice - [failed]");
		return Status;
	}
	else DbgPrint(" >MegaDriver: DriverEntry-> IoCreateDevice - [ok]");

	//���� �� ���, �� ������ �����
	DeviceObject->Flags |= DO_DIRECT_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;


	// �������� ��������� �� �������, ��������������� ���
	// ��������� ���������� ����������
	dx = (PEXAMPLE_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
	dx->fdo = DeviceObject; // ��������� �������� ���������



	//==========================================================================================
	//===== ������ �������� ����
	//==========================================================================================
	//�������� ��� �����
	RtlInitUnicodeString(&LinkString, L"\\DosDevices\\MegaDriver");
	//������ ���������� ������
	Status = IoCreateSymbolicLink(&LinkString, &NameString);

	//���� ����, �� ������� ������ � �������
	if(!NT_SUCCESS(Status))
	{
		DbgPrint(" >MegaDriver: DriverEntry-> IoCreateSymbolikLink - [failed]");
		IoDeleteDevice(DriverObject->DeviceObject);//������ ������
		return Status;
	} else DbgPrint(" >MegaDriver: DriverEntry-> IoCreateSymbolikLink - [ok]");



	//==================================================================================================
	//===== ������ �����
	//==================================================================================================
	//RtlInitUnicodeString(&ProcessEventName, L"\\\\BaseNamedObjects\\Global\\EVENT_NAME");
	//gEvent = IoCreateNotificationEvent(&ProcessEventName, &ghProcessHandle);
	//if(ghProcessHandle!=NULL)
	//{
	//	KeClearEvent(gEvent); 
	//	DbgPrint(" >MegaDriver: DriverEntry-> EventCreated - [ok]");
	//}//���� ��� � ����������, ����� ����������� =) 
	RtlInitUnicodeString(&ProcessEventName,L"\\BaseNamedObjects\\DriverProcessEvent");//v draivere
	gEvent = IoCreateNotificationEvent(&ProcessEventName,&ghProcessHandle);
	if(ghProcessHandle!=NULL)
	{
		KeClearEvent(gEvent); 
		DbgPrint(" >MegaDriver: DriverEntry-> EventCreated - [ok]");
	}


	//BSOD
	//KeBugCheck(1);
	if(NT_SUCCESS(Status))
		DbgPrint(" >MegaDriver: DriverEntry-> all done - [ok]");

	return STATUS_SUCCESS;
}




//================================================================================================
//===== ������� �������� 
//================================================================================================
NTSTATUS CreateMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
	DbgPrint(" >MegaDriver: CreateDriver-> we are inda \"file\" - [ok]");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//��������� ��������� �������

	return STATUS_SUCCESS;
}


//=================================================================================================
//===== ������� ������  // ��������� IO ����� � IOctl
//================================================================================================
	NTSTATUS ReadMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//��������� ��������� �������
	DbgPrint(" >MegaDriver: ReadDriver");

	return STATUS_SUCCESS;
}



//==================================================================================================
//===== ������� ������
//=================================================================================================
NTSTATUS WriteMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//��������� ��������� �������
	DbgPrint(" >MegaDriver: WriteDriver");

	return STATUS_SUCCESS;
}

//===============================================================================================
//===== ������� ��������
//==============================================================================================
NTSTATUS CloseMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint(" >MegaDriver: CloseMyDriver-> trying to close \"file\" ");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//��������� ��������� �������
	DbgPrint(" >MegaDriver: CloseMyDriver-> closed - [ok] ");

	return STATUS_SUCCESS;
}



//=====================================================================================
//===== ������� ��������� ��������
//=====================================================================================
NTSTATUS ShutDownMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint(" >MegaDriver: StopDriver");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//��������� ��������� �������

	return STATUS_SUCCESS;
}


//===============================================================================================
//===== ������� IO Control
//===============================================================================================
NTSTATUS IoCtlMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	MegaDriverData *pdata;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG BytesCount =0; // ����� ����������/���������� ���� (���� 0)	
	PIO_STACK_LOCATION IrpStack=IoGetCurrentIrpStackLocation(Irp);
	UCHAR *buff;//for byte

	// �������� ��������� �� ���������� ����������
	PEXAMPLE_DEVICE_EXTENSION dx = (PEXAMPLE_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

	// �������� �� IRP �������� IOCTL ���� (������� ������)
	ULONG ControlCode = IrpStack->Parameters.DeviceIoControl.IoControlCode;
	//���
	DbgPrint(" >MegaDriver: IoCtlDriver - we are in");

	//--------------------------------------------------------------------------------------------------------------
	switch(ControlCode)
	{
		//��� ��� �������� ���������� � ����� ��������
	case IOCTL_SEND_DATA_STRUCT:
		BytesCount = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
		if(DataSize == BytesCount) 
		{
			//===================================================================================================
			//===== �������� ���������
			//===================================================================================================
			DbgPrint(" >MegaDriver: IoCtlDriver -> Check BufferIn Size - [ok]");

			//��������� ��������
			if(((MegaDriverData*)Irp->AssociatedIrp.SystemBuffer)->procToFind == NULL)
			{
				DbgPrint(" >MegaDriver: IoCtlDriver -> Input Params == NULL");
				break;//��������� �����, ��� ����� ����� ���� ����...
			}
			//copy data
			strcpy(gData.procToFind, ((MegaDriverData*)Irp->AssociatedIrp.SystemBuffer)->procToFind);
			strcpy(gData.procToOpen, ((MegaDriverData*)Irp->AssociatedIrp.SystemBuffer)->procToOpen);
			//show what we have
			DbgPrint(" >MegaDriver: IoCtlDriver -> Input Params:");
			DbgPrint(" >MegaDriver: IoCtlDriver -> procToFind %s", gData.procToFind);
			DbgPrint(" >MegaDriver: IoCtlDriver -> procToOpen %s", gData.procToOpen);
			//if not BSOD =)
			gInited = TRUE;
			//�������... ��� �� ������� ��������� !!!

			//������� �������� �� �������� ������ �������� � �������
			PsSetCreateProcessNotifyRoutine(NotifyRoutine, FALSE);

			//===================================================================================================
			//===== ������ �������� �������
			//===================================================================================================
			pdata = (MegaDriverData*)Irp->AssociatedIrp.SystemBuffer;
			strcpy(gData.status, "all data accepted");//������� ���� 
			strcpy(pdata->procToFind, "11111111111");
			strcpy(pdata->procToOpen, "22222222222");
			if(FindProcess(FindBuffer))
			{
				DbgPrint(" >MegaDriver: +++ PROCESS FOUND +++");
				strcpy(pdata->status, "found");
			}
			else
			{
				strcpy(pdata->status, "not found");
			}
			Irp->IoStatus.Information = sizeof(gData);

			DbgPrint(" >MegaDriver: IoCtlDriver -> Status: %s", pdata->status);


			//�������� ��������� ����� ������ �������)))


			//IoRequestDpc( //create DPC
			//IoInitializeDpcRequest
			//KeInsertQueueDpc
			//PsCreateSystemThread//IRQL_PASSIVE_LEVEL
			//WriteToFile("First test string from IoCtl \r\n");
			//---------



		}
		else DbgPrint(" >MegaDriver: IoCtlDriver -> Check BufferIn Size - [failed]");
		break;
	case IOCTL_JUST_CHECK_STATUS:
		pdata = (MegaDriverData*)Irp->AssociatedIrp.SystemBuffer;
		strcpy(gData.status, "all data accepted");//������� ���� 
		strcpy(pdata->procToFind, "11111111111");
		strcpy(pdata->procToOpen, "22222222222");
		if(FindProcess(FindBuffer))
		{
			DbgPrint(" >MegaDriver: +++ PROCESS FOUND +++");
			strcpy(pdata->status, "found");
		}
		else
		{
			strcpy(pdata->status, "not found");
		}
		Irp->IoStatus.Information = sizeof(gData);

		DbgPrint(" >MegaDriver: IoCtlDriver -> Status: %s", pdata->status);
		break;
	default:
		DbgPrint(" >MegaDriver: IoCtlDriver -> UnKnown ControlCode");
	}
	//--------------------------------------------------------------------------------------------------------------



	IoCompleteRequest(Irp, IO_NO_INCREMENT);//��������� ��������� �������

	return STATUS_SUCCESS;
}





//=======================================================================================
//===== ������� �������� ��������
//=======================================================================================
NTSTATUS MyDriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING LinkString;

	//PsTerminateSystemThread(STATUS_SUCCESS);

	//������ �����
	//eve//ZwClose(ghProcessHandle);
	ZwClose(ghProcessHandle);

	//������� �������� �� �������� �������� � ������� (���� �� �� ������� ������)
	if(gInited)PsSetCreateProcessNotifyRoutine(NotifyRoutine, TRUE);

	DbgPrint(" >MegaDriver: UnloadDriver");
	//�������� ��� �������
	RtlInitUnicodeString(&LinkString, L"\\DosDevices\\MegaDriver");
	//������� �������
	IoDeleteSymbolicLink(&LinkString);
	//������� ������
	IoDeleteDevice(DriverObject->DeviceObject);

	return STATUS_SUCCESS;
}





//File name - HardCoded
//open file, write myString to file, close file
//IRQL: PASSiVE_LEVEL
//
NTSTATUS WriteToFile(IN char myString[])
{
	NTSTATUS status;
	UNICODE_STRING fullFileName;
	HANDLE fileHandle;
	IO_STATUS_BLOCK iostatus;
	OBJECT_ATTRIBUTES oa;
	ULONG len;

	//�������� ��� �����
	RtlInitUnicodeString( &fullFileName,
		L"\\??\\C:\\MegaDriverLog.txt");

	InitializeObjectAttributes( &oa,
		&fullFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL );
	//���������/������� ����
	status = ZwCreateFile ( &fileHandle,
		GENERIC_WRITE | SYNCHRONIZE,//IN ACCESS_MASK 
		&oa,// IN POBJECT_ATTRIBUTES 
		&iostatus,//OUT PIO_STATUS_BLOCK 
		0, // alloc size = none//IN PLARGE_INTEGER 
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE,
		FILE_OPEN_IF,//���� ���� - ��������
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	// �����:
	// GENERIC_WRITE ����� STANDARD(0x40000000L)
	//
	// FILE_GENERIC_WRITE ����� STANDARD_RIGHTS_WRITE|FILE_WRITE_DATA |
	// FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA |
	// SYNCHRONIZE, ��� ����� ������� � ������������ ����� winnt.h

	if( NT_SUCCESS(status))
	{
		// ���������, ������� ������� ���������� ����� �����:
		FILE_STANDARD_INFORMATION fileInfo;

		status = // �������� ���������� � �����
			ZwQueryInformationFile( fileHandle,
			&iostatus,
			&fileInfo,
			sizeof(FILE_STANDARD_INFORMATION),
			FileStandardInformation
			);
		len = strlen(myString);
		if( NT_SUCCESS(status) )
		{
			LARGE_INTEGER ByteOffset = fileInfo.EndOfFile;
			status = ZwWriteFile(fileHandle,
				NULL,
				NULL,
				NULL,
				&iostatus,
				myString, len, // ������������ ������
				&ByteOffset, // a ���� NULL? ��. ����
				NULL);
			if( !NT_SUCCESS(status) || iostatus.Information != len )
			{
				DbgPrint("WriteToFile: Error on writing. Status = %x.", status);
			}
		}
		ZwClose(fileHandle);
	}


	return STATUS_SUCCESS;
}







//IRQL: PASSIVE_LEVEL
//���������� ���, ��� ����� ��� ������:
//��� ��������������� �������� �������� ������� ���������. 
// (�) DjComandos
BOOLEAN FindProcess(IN char ProcessName[])
{
	ULONG BufferSize = 64000; // ��������� ������ ������ ~ 64kb
	LPVOID pBuffer = NULL;	// ��������� �� �����
	NTSTATUS Status;
	PSYSTEM_PROCESS_INFORMATION pInfo;
	LPWSTR tempProcessName;//

	WriteToFile("\r\n\r\n>New Log: \r\n");



	//����: ���� �� ������� ����� ������� �������
	do
	{
		//������� ����� � ������ �������� BufferSize
		pBuffer = ExAllocatePool (NonPagedPool, BufferSize); 
		// ������ - �������� �� ������?
		if (pBuffer == NULL) 
		{
			//�� ������ ������
			return FALSE;
		}
		// ����� ������ �������
		Status = ZwQuerySystemInformation(
			(SYSTEM_INFORMATION_CLASS)SystemProcessesAndThreadsInformation,//��� ������ ���� = 5
			pBuffer, //��������� �� �����
			BufferSize,//������ ������(�����) 
			NULL);//������ ���� ���� ������� (� ������� �������)
		//���� ��� ������� ������ ��� ������ �������
		if (Status == STATUS_INFO_LENGTH_MISMATCH)
		{
			// ���������� ������
			ExFreePool(pBuffer); 
			// ����������� ������ ������ (� ��������� ������� � ��� ����)
			BufferSize *= 2; 
		}
		// ������ ������
		else if (!NT_SUCCESS(Status)) 
		{
			//���������� ������
			ExFreePool(pBuffer); 
			//� ����� �����
			return FALSE;
		}
	}
	while (Status == STATUS_INFO_LENGTH_MISMATCH);
	//-----------------------------------------------------------------------------------------------------------
	//--- ���� ����� ��������, ��:
	//--- � ��� ���� ��������� �� ������ ��������,
	//--- ������� ��������� ��� ���������� � ������� ��������
	//--- ����� ���, ������� ������� ������� ��������
	//--- ���������� (�.�. ������������ ������ ���������, ��������� �������� ���� Notepad)
	//-----------------------------------------------------------------------------------------------------------

	pInfo = (PSYSTEM_PROCESS_INFORMATION)pBuffer;//��� �� �������

	while(true)
	{
		tempProcessName = pInfo->ProcessName.Buffer; //�������� ��� ��������
		if (tempProcessName == NULL) //���� �� ��������
			tempProcessName = L"NULL"; //�� ���-�� ���
		//������� � ����
		wcstombs(ProcessName,tempProcessName,256);

		//==============================================================
		//===== � ���������� ��� ��� �������?
		//==============================================================


		if(strcmp(ProcessName,gData.procToFind)==0)
		{
			//��� ������
			//���������������
			WriteToFile("\r\n\r\n>FOUND: ");
			WriteToFile(ProcessName);
			return TRUE;
		}


		//����� � ��� ������� �������
		WriteToFile("\r\n");
		//����� � ��� ��� ��������
		WriteToFile(ProcessName);



		// ���� � pInfo ������ ������ ���
		if (pInfo->NextEntryDelta == 0)
			break; //�� �����

		// Like a i++ inda Array
		pInfo = (PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)pInfo)+ pInfo->NextEntryDelta);//�� GetNext
	}
	//������ �� �����
	ExFreePool(pBuffer);

	//���� ����� �������� - ������ �� �����
	return FALSE;//��������
}


//========================================================================================================
//====== NotifyRoutine
//========================================================================================================
void NotifyRoutine(IN HANDLE ParentId,
	IN HANDLE ProcessId,
	IN BOOLEAN Create)
{
	DbgPrint("MegaDriver: NotifyRoutine: a new process was created");
	KeSetEvent(gEvent, 0, FALSE);
	//�����
	KeClearEvent(gEvent);
}