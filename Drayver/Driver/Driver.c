#include <ntddk.h>
#include <Ntstrsafe.h>
#include "undocnt.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject);
//VOID CreateProcCallback(HANDLE dwParentId, HANDLE hProcessId, BOOLEAN Create);
VOID LoadImageCallback(PUNICODE_STRING  FullImageName, HANDLE  ProcessId, PIMAGE_INFO  ImageInfo);

//typedef NTSYSAPI NTSTATUS (NTAPI *LOAD_DLL_ROUTINE)(PWCHAR PathToFile,ULONG Flags, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle);


#pragma alloc_text(INIT, DriverEntry)

HANDLE hTargetProcHandle;
BOOLEAN procFound = FALSE;
HANDLE hNewProc;

//LOAD_DLL_ROUTINE MyLoadDll = NULL;

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;

	DriverObject->DriverUnload = UnloadRoutine;
	//status = PsSetCreateProcessNotifyRoutine(CreateProcCallback, FALSE);
	//if (!NT_SUCCESS(status)) 
	//	DbgPrint("PsSetCreateProcessNotifyRoutine error");
	status = PsSetLoadImageNotifyRoutine(LoadImageCallback);
	if (!NT_SUCCESS(status)) 
		DbgPrint("PsSetLoadImageNotifyRoutine error");
	DbgPrint("DriverEntry");

	return status;
}

VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject)
{
	NTSTATUS status = STATUS_SUCCESS;

	//status = PsSetCreateProcessNotifyRoutine(CreateProcCallback, TRUE);
	//if (!NT_SUCCESS(status)) 
	//	DbgPrint("PsSetCreateProcessNotifyRoutine error");
	status = PsRemoveLoadImageNotifyRoutine(LoadImageCallback);
	if (!NT_SUCCESS(status)) 
		DbgPrint("PsRemoveLoadImageNotifyRoutine error");
	DbgPrint("Unload Routine");
}


/*VOID CreateProcCallback(HANDLE hParentId, HANDLE hProcessId, BOOLEAN Create)
{
	NTSTATUS status;
	ULONG requiredSize = 0;
	char procName[256];
	ANSI_STRING ansiString;
	UNICODE_STRING unicodeProcName;
	RtlUnicodeStringInit(&unicodeProcName, L"11111111111111111111111111111111111111");
	
	if(Create) // process is creating
	{
		DbgPrint("Process was created");
		
		status = NtQueryInformationProcess(hProcessId, ProcessImageFileName, &unicodeProcName, sizeof(UNICODE_STRING), &requiredSize);
		if (!NT_SUCCESS(status)) 
		DbgPrint("NtQueryInformationProcess error");
		DbgPrint("%d", sizeof(UNICODE_STRING));
		DbgPrint("%d", requiredSize);
		
		memset(procName, 0, 256);	
		RtlUnicodeStringToAnsiString(&ansiString, &unicodeProcName, TRUE);
		memcpy(procName, ansiString.Buffer, ansiString.Length);
		DbgPrint("Process %s is statring", procName);
		RtlFreeAnsiString(&ansiString);
		hTargetProcHandle = hProcessId;
	}
	else //process is exiting
	{
		DbgPrint("Process was terminated");
		if(hTargetProcHandle == hProcessId)
		{
			DbgPrint("Process calc.exe is terminating");
		}
	}
}*/

VOID LoadImageCallback(PUNICODE_STRING  FullImageName, HANDLE  ProcessId, PIMAGE_INFO  ImageInfo)
{
	char name[256];
	ANSI_STRING ansiString;
	
	UNICODE_STRING uSTargetProcName;
	UNICODE_STRING uSTargetDllName;
	
	RtlUnicodeStringInit(&uSTargetProcName, L"\\Device\\HarddiskVolume1\\WINDOWS\\system32\\calc.exe");
	if(!procFound)
	{
		if(RtlEqualUnicodeString(&uSTargetProcName, FullImageName, FALSE))
		{
			DbgPrint("Process calc.exe was started!");
			DbgPrint("Process handle: %d", ProcessId);
			hTargetProcHandle = ProcessId;
			procFound = TRUE;
			//NtCreateProcess( &hNewProc, NULL, NULL, hTargetProcHandle, FALSE, NULL, NULL,NULL);
		}
	}
	else
	{
		if(hTargetProcHandle == ProcessId)
		{
			//memset(name, 0, 256);
			//RtlUnicodeStringToAnsiString(&ansiString, FullImageName, TRUE);
			//memcpy(name, ansiString.Buffer, ansiString.Length);
			//DbgPrint("LoadImageCallback name: %s", name);
			//RtlFreeAnsiString(&ansiString);
			RtlUnicodeStringInit(&uSTargetDllName, L"\\SystemRoot\\System32\\ntdll.dll");
			
			if(RtlEqualUnicodeString(&uSTargetDllName, FullImageName, FALSE))
			{
				DbgPrint("ntdll.dll was mapped!");
				DbgPrint("ntdll.dll address: %x", ImageInfo->ImageBase);
				//MyLoadDll = (LOAD_DLL_ROUTINE)((ULONG)ImageInfo->ImageBase + 0x571bb);
				//DbgPrint("MyLoadDll addr: %x", MyLoadDll);
			}
		}
	}
}

/*

void myAPCProc()
{
	UNICODE_STRING dllName;
	HANDLE hDllHandle;
	
	RtlUnicodeStringInit(&uSTargetDllName, L"");
	MyLoadDll(NULL, 0, &dllName, &hDllHandle);
}*/