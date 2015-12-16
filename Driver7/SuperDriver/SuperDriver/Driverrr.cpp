#include "ntddk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "DriverH.h"


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

//обработчики IO обращений к драйверу
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
NTSTATUS WriteToFile(IN char myString[]);//своеобразный запись в логфайл на винте
BOOLEAN FindProcess(IN char ProcessName[]);//суть задания
void NotifyRoutine(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create);


MegaDriverData gData; //структурка в которой хранятся параметры, переданные из Юзерской проги (2-3 строки)
BOOLEAN gInited; //флаг, который показывает пришла ли структура с параметрами (тру если да)
HANDLE gThreadHandle; //хендл потока, который будем создавать, для поиска процессов
PKEVENT gEvent;//id именованного евента
HANDLE ghProcessHandle;//хендл этого евента
//--------------------
char FindBuffer[256];
typedef ULONG DWORD;
typedef VOID* LPVOID;
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L) //код ошибки, которую возвращает ZwQuerySystemInformation
//если вдруг ей нехватает выделенного буффера
//в таком случае надо выделить бефер побольше
//#define SystemProcessesAndThreadsInformation 5 // тупо тип вывода(отображения) инфы




//==================================================================================
//===== Процедура Входа в драйвер
//==================================================================================
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	
	UNICODE_STRING NameString, LinkString, ProcessEventName;
	PDEVICE_OBJECT DeviceObject;//переменная объекта устройства
	NTSTATUS Status;//код статуса
		PEXAMPLE_DEVICE_EXTENSION dx;
	PVOID pvWaitEvents[1];
	OBJECT_ATTRIBUTES oaUserEvent;
	gInited = FALSE;
	DbgPrint(" >MegaDriver: we are inda DriverEntry"); //открываем DbgView - он отлавливает DbgPrint - будем видеть что происходит


	//================================================================================================
	//===== Инитим указатели на функции
	//================================================================================================
	DriverObject->DriverUnload = (PDRIVER_UNLOAD)MyDriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateMyDriver;//CreateFile
	DriverObject->MajorFunction[IRP_MJ_READ] = ReadMyDriver; //readfile
	DriverObject->MajorFunction[IRP_MJ_WRITE] = WriteMyDriver; //write...
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseMyDriver; //CloseFile
	//DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = ShutDownMyDriver;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoCtlMyDriver;


	//=================================================================================================
	//===== Создаём девайс
	//=================================================================================================
	//юникодим имя нашего драйвера
	RtlInitUnicodeString(&NameString, L"\\Device\\MegaDriver");
	//создаём девайс
	Status = IoCreateDevice(DriverObject, sizeof(65533), &NameString, 0, 0, FALSE, &DeviceObject);
	if(!NT_SUCCESS(Status)) 
	{
		DbgPrint(" >MegaDriver: DriverEntry-> IoCreateDevice - [failed]");
		return Status;
	}
	else DbgPrint(" >MegaDriver: DriverEntry-> IoCreateDevice - [ok]");

	//если всё гуд, то ставим флаги
	DeviceObject->Flags |= DO_DIRECT_IO;
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;


	// Получаем указатель на область, предназначенную под
	// структуру расширение устройства
	dx = (PEXAMPLE_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
	dx->fdo = DeviceObject; // Сохраняем обратный указатель



	//==========================================================================================
	//===== Создаём симболик линк
	//==========================================================================================
	//Юникодим имя линку
	RtlInitUnicodeString(&LinkString, L"\\DosDevices\\MegaDriver");
	//создаём символьную ссылку
	Status = IoCreateSymbolicLink(&LinkString, &NameString);

	//если краш, то удаляем девайс и выходим
	if(!NT_SUCCESS(Status))
	{
		DbgPrint(" >MegaDriver: DriverEntry-> IoCreateSymbolikLink - [failed]");
		IoDeleteDevice(DriverObject->DeviceObject);//сносим девайс
		return Status;
	} else DbgPrint(" >MegaDriver: DriverEntry-> IoCreateSymbolikLink - [ok]");



	//==================================================================================================
	//===== Создаём евент
	//==================================================================================================
	//RtlInitUnicodeString(&ProcessEventName, L"\\\\BaseNamedObjects\\Global\\EVENT_NAME");
	//gEvent = IoCreateNotificationEvent(&ProcessEventName, &ghProcessHandle);
	//if(ghProcessHandle!=NULL)
	//{
	//	KeClearEvent(gEvent); 
	//	DbgPrint(" >MegaDriver: DriverEntry-> EventCreated - [ok]");
	//}//хоть код и закоменчен, может пригодиться =) 
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
//===== Функция создания 
//================================================================================================
NTSTATUS CreateMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
	DbgPrint(" >MegaDriver: CreateDriver-> we are inda \"file\" - [ok]");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//завершаем обработку запроса

	return STATUS_SUCCESS;
}


//=================================================================================================
//===== Функция Чтения  // обработка IO будет в IOctl
//================================================================================================
	NTSTATUS ReadMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//завершаем обработку запроса
	DbgPrint(" >MegaDriver: ReadDriver");

	return STATUS_SUCCESS;
}



//==================================================================================================
//===== Функция Записи
//=================================================================================================
NTSTATUS WriteMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//завершаем обработку запроса
	DbgPrint(" >MegaDriver: WriteDriver");

	return STATUS_SUCCESS;
}

//===============================================================================================
//===== Функция закрытия
//==============================================================================================
NTSTATUS CloseMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint(" >MegaDriver: CloseMyDriver-> trying to close \"file\" ");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//завершаем обработку запроса
	DbgPrint(" >MegaDriver: CloseMyDriver-> closed - [ok] ");

	return STATUS_SUCCESS;
}



//=====================================================================================
//===== Функция Остановки Драйвера
//=====================================================================================
NTSTATUS ShutDownMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	DbgPrint(" >MegaDriver: StopDriver");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);//завершаем обработку запроса

	return STATUS_SUCCESS;
}


//===============================================================================================
//===== Функция IO Control
//===============================================================================================
NTSTATUS IoCtlMyDriver(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	MegaDriverData *pdata;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG BytesCount =0; // Число переданных/полученных байт (пока 0)	
	PIO_STACK_LOCATION IrpStack=IoGetCurrentIrpStackLocation(Irp);
	UCHAR *buff;//for byte

	// Получаем указатель на расширение устройства
	PEXAMPLE_DEVICE_EXTENSION dx = (PEXAMPLE_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

	// Выделяем из IRP значение IOCTL кода (причина вызова)
	ULONG ControlCode = IrpStack->Parameters.DeviceIoControl.IoControlCode;
	//лог
	DbgPrint(" >MegaDriver: IoCtlDriver - we are in");

	//--------------------------------------------------------------------------------------------------------------
	switch(ControlCode)
	{
		//тут нам прислали структурку с двумя строкаме
	case IOCTL_SEND_DATA_STRUCT:
		BytesCount = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
		if(DataSize == BytesCount) 
		{
			//===================================================================================================
			//===== Получаем параметры
			//===================================================================================================
			DbgPrint(" >MegaDriver: IoCtlDriver -> Check BufferIn Size - [ok]");

			//проверяем параметр
			if(((MegaDriverData*)Irp->AssociatedIrp.SystemBuffer)->procToFind == NULL)
			{
				DbgPrint(" >MegaDriver: IoCtlDriver -> Input Params == NULL");
				break;//сваливаем нахер, ибо иначе может быть бсод...
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
			//Шикарно... тут мы приняли параметрЫ !!!

			//создаем нотифаер на создание нового процесса в системе
			PsSetCreateProcessNotifyRoutine(NotifyRoutine, FALSE);

			//===================================================================================================
			//===== Чонить отсылаем обратно
			//===================================================================================================
			pdata = (MegaDriverData*)Irp->AssociatedIrp.SystemBuffer;
			strcpy(gData.status, "all data accepted");//нипашет пока 
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


			//осталось всеголишь найти нужный процесс)))


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
		strcpy(gData.status, "all data accepted");//нипашет пока 
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



	IoCompleteRequest(Irp, IO_NO_INCREMENT);//завершаем обработку запроса

	return STATUS_SUCCESS;
}





//=======================================================================================
//===== Функция Выгрузки Драйвера
//=======================================================================================
NTSTATUS MyDriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING LinkString;

	//PsTerminateSystemThread(STATUS_SUCCESS);

	//сносим евент
	//eve//ZwClose(ghProcessHandle);
	ZwClose(ghProcessHandle);

	//снимает нотифаер на создание процесса в системе (если же он конечно создан)
	if(gInited)PsSetCreateProcessNotifyRoutine(NotifyRoutine, TRUE);

	DbgPrint(" >MegaDriver: UnloadDriver");
	//Юникодим имя симлики
	RtlInitUnicodeString(&LinkString, L"\\DosDevices\\MegaDriver");
	//удаляем симлинк
	IoDeleteSymbolicLink(&LinkString);
	//удаляем девайс
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

	//юникодим имя файла
	RtlInitUnicodeString( &fullFileName,
		L"\\??\\C:\\MegaDriverLog.txt");

	InitializeObjectAttributes( &oa,
		&fullFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL );
	//открываем/создаем файл
	status = ZwCreateFile ( &fileHandle,
		GENERIC_WRITE | SYNCHRONIZE,//IN ACCESS_MASK 
		&oa,// IN POBJECT_ATTRIBUTES 
		&iostatus,//OUT PIO_STATUS_BLOCK 
		0, // alloc size = none//IN PLARGE_INTEGER 
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE,
		FILE_OPEN_IF,//если есть - дописать
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	// Здесь:
	// GENERIC_WRITE равно STANDARD(0x40000000L)
	//
	// FILE_GENERIC_WRITE равно STANDARD_RIGHTS_WRITE|FILE_WRITE_DATA |
	// FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA |
	// SYNCHRONIZE, что можно увидеть в заголовочном файле winnt.h

	if( NT_SUCCESS(status))
	{
		// Структура, которая поможет определить длину файла:
		FILE_STANDARD_INFORMATION fileInfo;

		status = // Получаем информацию о файле
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
				myString, len, // Записываемая строка
				&ByteOffset, // a если NULL? см. ниже
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
//примечание тем, кто будет это читать:
//все орфографические очепятки допущены автором намеренно. 
// (с) DjComandos
BOOLEAN FindProcess(IN char ProcessName[])
{
	ULONG BufferSize = 64000; // начальный размер буфера ~ 64kb
	LPVOID pBuffer = NULL;	// указатель на буфер
	NTSTATUS Status;
	PSYSTEM_PROCESS_INFORMATION pInfo;
	LPWSTR tempProcessName;//

	WriteToFile("\r\n\r\n>New Log: \r\n");



	//цикл: пока не выделим буфер нужного размера
	do
	{
		//создаем буфер в памяти размером BufferSize
		pBuffer = ExAllocatePool (NonPagedPool, BufferSize); 
		// тестим - выделили ли память?
		if (pBuffer == NULL) 
		{
			//всё совсем херово
			return FALSE;
		}
		// юзаем хитрую функцию
		Status = ZwQuerySystemInformation(
			(SYSTEM_INFORMATION_CLASS)SystemProcessesAndThreadsInformation,//тип вывода инфы = 5
			pBuffer, //указатель на буфер
			BufferSize,//размер буфера(длина) 
			NULL);//длинна того чего вернули (а посрать сколько)
		//если нам вернули больше чем размер буффера
		if (Status == STATUS_INFO_LENGTH_MISMATCH)
		{
			// овобождаем буфера
			ExFreePool(pBuffer); 
			// увеличиваем размер буфера (с маленьким запасов в два раза)
			BufferSize *= 2; 
		}
		// другая ошибко
		else if (!NT_SUCCESS(Status)) 
		{
			//освобожаем память
			ExFreePool(pBuffer); 
			//и валим нахер
			return FALSE;
		}
	}
	while (Status == STATUS_INFO_LENGTH_MISMATCH);
	//-----------------------------------------------------------------------------------------------------------
	//--- если дошли доседова, то:
	//--- у нас есть указатель на список структур,
	//--- которые описывают все запущенные в системе процессы
	//--- КРОМЕ тех, которые всякими хитрыми методами
	//--- скрывались (т.е. отображаются только легальные, нескрытые процессы типа Notepad)
	//-----------------------------------------------------------------------------------------------------------

	pInfo = (PSYSTEM_PROCESS_INFORMATION)pBuffer;//вот он родимый

	while(true)
	{
		tempProcessName = pInfo->ProcessName.Buffer; //получаем имя процесса
		if (tempProcessName == NULL) //если не получили
			tempProcessName = L"NULL"; //то как-то так
		//декодем в ОСКИ
		wcstombs(ProcessName,tempProcessName,256);

		//==============================================================
		//===== А собственно это наш процесс?
		//==============================================================


		if(strcmp(ProcessName,gData.procToFind)==0)
		{
			//наш клиент
			//задокументируем
			WriteToFile("\r\n\r\n>FOUND: ");
			WriteToFile(ProcessName);
			return TRUE;
		}


		//пишем в лог перевод каретке
		WriteToFile("\r\n");
		//пишем в лог имя процесса
		WriteToFile(ProcessName);



		// если в pInfo больше нихера нет
		if (pInfo->NextEntryDelta == 0)
			break; //то валим

		// Like a i++ inda Array
		pInfo = (PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)pInfo)+ pInfo->NextEntryDelta);//ну GetNext
	}
	//убрали за собой
	ExFreePool(pBuffer);

	//если дошли доседава - нихера не нашли
	return FALSE;//печально
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
	//магия
	KeClearEvent(gEvent);
}