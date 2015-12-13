#define _X86_
#include <ntddk.h>
#include "undocnt.h"
#include <stdio.h>


Atoms()
{
	ATOM Atom, FindAtom;
	NTSTATUS rc;
	char Buffer[1000];

	return 0;
	rc=NtAddAtom(L"Hello",
#ifdef NT50
				5,
#endif
				&Atom);
	if (rc!=0) {
		printf("NtAddAtom failed, rc=%x\n", rc);
		return 0;
	}

	printf("Atom=%x\n", Atom);

	rc=NtFindAtom(L"Hello", 
#ifdef NT50
				5,
#endif
					&FindAtom);
	if (rc!=0) {
		printf("NtFindAtom failed, rc=%x\n", rc);
	} else {
		printf("FindAtom=%x\n", FindAtom);
	}
				
	rc=NtQueryInformationAtom(Atom,
							AllAtoms,
							Buffer,
							sizeof(Buffer),
							NULL);
	if (rc!=0) {
		printf("NtQueryInformationAtom with infoclass 'AllAtoms' failed, rc=%x\n", rc);
	} else {
		PATOMINFOALL pAtomInfoAll;
		ULONG i;

		pAtomInfoAll=(PATOMINFOALL)Buffer;

		for (i=0; i<pAtomInfoAll->TotalNumberOfEntriesInGlobalAtomTable; i++) {
			char AnotherBuffer[1024];

			printf("%x\n", pAtomInfoAll->AtomValues[i]);
			rc=NtQueryInformationAtom(pAtomInfoAll->AtomValues[i],
									SingleAtom,
									AnotherBuffer,
									sizeof(AnotherBuffer),
									NULL);
			if (rc!=0) {
				printf("NtQueryInformationAtom with infoclass 'SingleAtom' failed, rc=%x\n", rc);
			} else {
				PATOMINFOSINGLE pAtomInfoSingle;

				pAtomInfoSingle=(PATOMINFOSINGLE)AnotherBuffer;

				printf("\tpAtomInfoSingle->ReferenceCount   = %x\n", pAtomInfoSingle->ReferenceCount);
				printf("\tpAtomInfoSingle->Unknown          = %x\n", pAtomInfoSingle->Unknown);
				printf("\tpAtomInfoSingle->AtomStringLength = %x\n", pAtomInfoSingle->AtomStringLength);
				printf("\tpAtomInfoSingle->AtomString       = %S\n\n", pAtomInfoSingle->AtomString);
			}
		}
	}


	rc=NtDeleteAtom(Atom);
	if (rc!=0) {
		printf("NtDeleteAtom failed, rc=%x\n", rc);
		return 0;
	}
}


void CreateDirectoryObject()
{
	OBJECT_ATTRIBUTES ObjAttr;
	UNICODE_STRING uDirectoryName;
	HANDLE hDirectory;
	NTSTATUS rc;

	RtlInitUnicodeString(&uDirectoryName,
							L"\\WINDOWS\\Hello");

	InitializeObjectAttributes(&ObjAttr,
								&uDirectoryName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	rc=NtCreateDirectoryObject(&hDirectory,
							STANDARD_RIGHTS_REQUIRED,
							&ObjAttr);
	if (rc==STATUS_SUCCESS) {
		printf("Directory created\n");
		NtClose(hDirectory);
	} else {
		printf("Directory creation failed, rc=%x\n", rc);
	}
}

void OpenDirectoryObject()
{
	UNICODE_STRING uDirectoryObject;
	OBJECT_ATTRIBUTES ObjAttr;
	NTSTATUS rc;
	HANDLE hDirectory;

	RtlInitUnicodeString(&uDirectoryObject,
							L"\\");

	InitializeObjectAttributes(&ObjAttr,
								&uDirectoryObject,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtOpenDirectoryObject(&hDirectory,
							MAXIMUM_ALLOWED,
							&ObjAttr);
	if (rc==STATUS_SUCCESS) {
		printf("NtOpenDirectoryObject: hDirectory = %x\n", hDirectory);
		NtClose(hDirectory);
	}
}

void QueryDirectoryObject()
{
	UNICODE_STRING uDirectoryObject;
	OBJECT_ATTRIBUTES ObjAttr;
	NTSTATUS rc;
	HANDLE hDirectory;
	QUERYDIRECTORYOBJECTBUFFER QueryDirectoryObjectBuffer;
	ULONG BytesReturned, EntryIndex;

	RtlInitUnicodeString(&uDirectoryObject,
							L"\\");

	InitializeObjectAttributes(&ObjAttr,
								&uDirectoryObject,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtOpenDirectoryObject(&hDirectory,
							MAXIMUM_ALLOWED,
							&ObjAttr);
	if (rc==STATUS_SUCCESS) {
		printf("NtOpenDirectoryObject: hDirectory = %x\n", hDirectory);

		rc=NtQueryDirectoryObject(hDirectory,
								&QueryDirectoryObjectBuffer,
								sizeof(QueryDirectoryObjectBuffer),
								1,
								1,
								&BytesReturned,
								&EntryIndex);
		while (rc==STATUS_SUCCESS) {
			printf("%x %x %S %S\n", BytesReturned, EntryIndex, QueryDirectoryObjectBuffer.DirectoryEntry.Buffer, QueryDirectoryObjectBuffer.DirectoryEntryType.Buffer);
			rc=NtQueryDirectoryObject(hDirectory,
									&QueryDirectoryObjectBuffer,
									sizeof(QueryDirectoryObjectBuffer),
									1,
									0,
									&BytesReturned,
									&EntryIndex);
		}
		NtClose(hDirectory);
	}
}



CreateSymbolicLink()
{
	OBJECT_ATTRIBUTES ObjAttr;
	UNICODE_STRING uSymbolicLinkName;
	UNICODE_STRING uObjectName;
	HANDLE hSymbolicLink;
	NTSTATUS rc;


	RtlInitUnicodeString(&uSymbolicLinkName,
		L"\\DosDevices\\M:");

	RtlInitUnicodeString(&uObjectName,
		L"\\Device\\HardDisk1\\Partition1");

	InitializeObjectAttributes(&ObjAttr,
								&uSymbolicLinkName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	rc=NtCreateSymbolicLinkObject(&hSymbolicLink,
								SYMBOLIC_LINK_ALL_ACCESS,
								&ObjAttr,
								&uObjectName);
	
	if (rc==STATUS_SUCCESS) {
		printf("Symbolic link created, hSymbolicLink=%x\n", hSymbolicLink);
		NtClose(hSymbolicLink);
	} else {
		printf("NtCreateSymbolicLinkObject failed, rc=%x\n", rc);
	}
	return 0;
}

OpenSymbolicLink()
{
	OBJECT_ATTRIBUTES ObjAttr;
	UNICODE_STRING uSymbolicLinkName;
	HANDLE hSymbolicLink;
	NTSTATUS rc;


	RtlInitUnicodeString(&uSymbolicLinkName,
		L"\\SystemRoot");

	InitializeObjectAttributes(&ObjAttr,
								&uSymbolicLinkName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	rc=NtOpenSymbolicLinkObject(&hSymbolicLink,
								MAXIMUM_ALLOWED,
								&ObjAttr);
	
	if (rc==STATUS_SUCCESS) {
		printf("Symbolic link opened, hSymbolicLink=%x\n", hSymbolicLink);
		NtClose(hSymbolicLink);
	} else {
		printf("NtOpenSymbolicLinkObject failed, rc=%x\n", rc);
	}
	return 0;
}

QuerySymbolicLink()
{
	OBJECT_ATTRIBUTES ObjAttr;
	UNICODE_STRING uSymbolicLinkName;
	HANDLE hSymbolicLink;
	NTSTATUS rc;
	UNICODE_STRING uSymbolicLinkContents;
	WCHAR Buffer[1000];

	RtlInitUnicodeString(&uSymbolicLinkName,
		L"\\SystemRoot");

	InitializeObjectAttributes(&ObjAttr,
								&uSymbolicLinkName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	rc=NtOpenSymbolicLinkObject(&hSymbolicLink,
								MAXIMUM_ALLOWED,
								&ObjAttr);
	
	if (rc==STATUS_SUCCESS) {
		ULONG BytesReturned;

		memset(Buffer, 0, sizeof(Buffer));
		RtlInitUnicodeString(&uSymbolicLinkContents, Buffer);

		rc=NtQuerySymbolicLinkObject(hSymbolicLink,
										&uSymbolicLinkContents,
										&BytesReturned);
		if (rc==STATUS_SUCCESS) {
			printf("Symbolic link contents = %S\n", uSymbolicLinkContents.Buffer);
		} else {
			printf("NtQuerySymbolicLinkObject failed, rc=%x\n", rc);
		}
									
		NtClose(hSymbolicLink);
	} else {
		printf("NtOpenSymbolicLinkObject failed, rc=%x\n", rc);
	}
	return 0;

}


QueryObject()
{
	NTSTATUS rc;
	ULONG BytesReturned;
	HANDLE hSemaphore;
	ULONG i;
	char Buffer[3000];
	OBJECT_BASIC_INFO ObjectBasicInfoBuffer;
	OBJECT_PROTECTION_INFO	ObjectProtectionInfoBuffer;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING SemaphoreName;

	RtlInitUnicodeString(&SemaphoreName, L"\\MySemaphore");
	InitializeObjectAttributes(&ObjectAttr,
								&SemaphoreName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateSemaphore(&hSemaphore,
						STANDARD_RIGHTS_REQUIRED|0x03,
						&ObjectAttr,
						2,
						10);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create semaphore\n");
		return 0;
	}


	rc=NtQueryObject(hSemaphore,
					ObjectBasicInfo,
					&ObjectBasicInfoBuffer,
					sizeof(ObjectBasicInfoBuffer),
					&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		printf("NtQueryObject success, BytesReturned=%x\n", BytesReturned);
	} else {
		printf("NtQueryObject failed, rc=%x\n", rc);
	}

	rc=NtQueryObject(hSemaphore,
					ObjectNameInfo,
					Buffer,
					sizeof(Buffer),
					&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		POBJECT_NAME_INFO pObjectNameInfoBuffer;
		pObjectNameInfoBuffer=(POBJECT_NAME_INFO)Buffer;
		printf("NtQueryObject success, BytesReturned=%x\n", BytesReturned);
		printf("%S\n", pObjectNameInfoBuffer->ObjectName.Buffer);
	} else {
		printf("NtQueryObject failed, rc=%x\n", rc);
	}

	rc=NtQueryObject(hSemaphore,
					ObjectTypeInfo,
					Buffer,
					sizeof(Buffer),
					&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		POBJECT_TYPE_INFO pObjectTypeInfoBuffer;
		pObjectTypeInfoBuffer=(POBJECT_TYPE_INFO)Buffer;
		printf("NtQueryObject success, BytesReturned=%x\n", BytesReturned);
		printf("%S\n", pObjectTypeInfoBuffer->ObjectTypeName.Buffer);
	} else {
		printf("NtQueryObject failed, rc=%x\n", rc);
	}

	rc=NtQueryObject(hSemaphore,
						ObjectAllTypesInfo,
						Buffer,
						sizeof(Buffer),
						&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		POBJECT_ALL_TYPES_INFO pObjectAllTypesInfoBuffer;
		POBJECT_TYPE_INFO pObjectTypeInfoBuffer;

		printf("NtQueryObject success, BytesReturned=%x\n", BytesReturned);
		pObjectAllTypesInfoBuffer=(POBJECT_ALL_TYPES_INFO)Buffer;
		pObjectTypeInfoBuffer=pObjectAllTypesInfoBuffer->ObjectsTypeInfo;

		for (i=0; i<pObjectAllTypesInfoBuffer->NumberOfObjectTypes; i++) {
			ULONG Skip;
			printf("%S\n", pObjectTypeInfoBuffer->ObjectTypeNameBuffer);
			Skip=(ULONG) ((pObjectTypeInfoBuffer->ObjectTypeName.MaximumLength +3)&0xFFFFFFFC);
			pObjectTypeInfoBuffer=(POBJECT_TYPE_INFO)(((char *)pObjectTypeInfoBuffer->ObjectTypeName.Buffer)+Skip);
		}
	} else {
		printf("NtQueryObject failed, rc=%x\n", rc);
	}

	rc=NtQueryObject(hSemaphore,
						ObjectProtectionInfo,
						&ObjectProtectionInfoBuffer,
						sizeof(ObjectProtectionInfoBuffer),
						&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		printf("NtQueryObject success, BytesReturned=%x\n", BytesReturned);
		printf("ObjectProtectionInfoBuffer.bInherit       = %s\n", ObjectProtectionInfoBuffer.bInherit?"TRUE":"FALSE");
		printf("ObjectProtectionInfoBuffer.bProtectHandle = %s\n", ObjectProtectionInfoBuffer.bProtectHandle?"TRUE":"FALSE");
	} else {
		printf("NtQueryObject failed, rc=%x\n", rc);
	}

	NtClose(hSemaphore);
	return 0;
}


SetObjectInformation()
{
	NTSTATUS rc;
	HANDLE hSemaphore;
	OBJECT_PROTECTION_INFO	ObjectProtectionInfoBuffer;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING SemaphoreName;

	RtlInitUnicodeString(&SemaphoreName, L"\\MySemaphore");
	InitializeObjectAttributes(&ObjectAttr,
								&SemaphoreName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateSemaphore(&hSemaphore,
						STANDARD_RIGHTS_ALL,
						&ObjectAttr,
						0,
						10);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create semaphore, rc=%x\n", rc);
		return 0;
	}

	ObjectProtectionInfoBuffer.bInherit=TRUE;
	ObjectProtectionInfoBuffer.bProtectHandle=TRUE;

	rc=NtSetInformationObject(hSemaphore,
							ObjectProtectionInfo,
							&ObjectProtectionInfoBuffer,
							sizeof(ObjectProtectionInfoBuffer));

	if (rc==STATUS_SUCCESS) {
		printf("NtSetInformationObject success\n");
	} else {
		printf("NtSetInformationObject failed, rc=%x\n", rc);
	}

	NtClose(hSemaphore);
	return 0;
}

CreateEventObject()
{
	NTSTATUS rc;
	HANDLE hEvent;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING EventName;

	RtlInitUnicodeString(&EventName, L"\\MyEvent");
	InitializeObjectAttributes(&ObjectAttr,
								&EventName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateEvent(&hEvent,
						STANDARD_RIGHTS_ALL,
						&ObjectAttr,
						SynchronizationEvent,
						TRUE);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create event, rc=%x\n", rc);
		return 0;
	} else {
		printf("Event created, hEvent=%x\n", hEvent);
	}
	NtClose(hEvent);
	return 0;
}

OpenEventObject()
{
	NTSTATUS rc;
	HANDLE hEvent;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING EventName;

	RtlInitUnicodeString(&EventName, L"\\BaseNamedObjects\\ScmCreatedEvent");
	InitializeObjectAttributes(&ObjectAttr,
								&EventName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtOpenEvent(&hEvent,
						MAXIMUM_ALLOWED,
						&ObjectAttr);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open event, rc=%x\n", rc);
		return 0;
	} else {
		printf("Event opened, hEvent=%x\n", hEvent);
		NtClose(hEvent);
	}
	return 0;
}

void DumpEventInfo(HANDLE hEvent)
{
	EVENT_INFO EventInfo;
	ULONG BytesReturned;
	NTSTATUS rc;

	rc=NtQueryEvent(hEvent,
					EventBasicInfo,
					&EventInfo,
					sizeof(EventInfo),
					&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		printf("EventInfo.EventType  = ");
		if (EventInfo.EventType==NotificationEvent) {
			printf("NotificationEvent\n");
		} else if (EventInfo.EventType==SynchronizationEvent) {
			printf("SynchronizationEvent\n");
		} else {
			printf("UnknownEventType\n");
		}
		printf("EventInfo.EventState = %s\n", EventInfo.EventState?"SIGNALLED":"NOT SIGNALLED");
	} else {
		printf("Unable to query event information\n");
	}
}



EventManagement()
{
	NTSTATUS rc;
	HANDLE hEvent;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING EventName;

	RtlInitUnicodeString(&EventName, L"\\MyEvent");
	InitializeObjectAttributes(&ObjectAttr,
								&EventName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateEvent(&hEvent,
						STANDARD_RIGHTS_ALL|0x03,
						&ObjectAttr,
						SynchronizationEvent,
						TRUE);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create event, rc=%x\n", rc);
		return 0;
	} else {
		ULONG PreviousState;
		printf("Event created, hEvent=%x\n", hEvent);

		printf("Information about the event\n");
		DumpEventInfo(hEvent);


		rc=NtResetEvent(hEvent, &PreviousState);
		if (rc==STATUS_SUCCESS) {
			printf("Information after resetting the event\n");
			printf("PreviousState=%s\n", PreviousState?"SIGNALLED":"NOT SIGNALLED");
			DumpEventInfo(hEvent);
		} else {
			printf("NtResetEvent failed, rc=%x\n", rc);
		}


		rc=NtPulseEvent(hEvent, &PreviousState);
		if (rc==STATUS_SUCCESS) {
			printf("Information after PulseEvent\n");
			printf("PreviousState=%s\n", PreviousState?"SIGNALLED":"NOT SIGNALLED");
			DumpEventInfo(hEvent);

		} else {
			printf("NtPulseEvent failed, rc=%x\n", rc);
		}

		rc=NtSetEvent(hEvent, &PreviousState);
		if (rc==STATUS_SUCCESS) {
			printf("Information after setting the event\n");
			printf("PreviousState=%s\n", PreviousState?"SIGNALLED":"NOT SIGNALLED");
			DumpEventInfo(hEvent);
		} else {
			printf("NtSetEvent failed, rc=%x\n", rc);
		}

		rc=NtClearEvent(hEvent);
		if (rc==STATUS_SUCCESS) {
			printf("Information after ClearEvent\n");
			DumpEventInfo(hEvent);
		} else {
			printf("NtClearEvent failed, rc=%x\n", rc);
		}
		NtClose(hEvent);
	}
	return 0;
}

void DumpMutantInfo(HANDLE hMutant)
{
	MUTANT_INFO MutantInfo;
	ULONG BytesReturned;
	NTSTATUS rc;

	rc=NtQueryMutant(hMutant,
					MutantBasicInfo,
					&MutantInfo,
					sizeof(MutantInfo),
					&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		printf("MutantInfo.MutantState           = %s\n", MutantInfo.MutantState?"SIGNALLED":"NOT SIGNALLED");
		printf("MutantInfo.bOwnedByCallingThread = %s\n", MutantInfo.bOwnedByCallingThread?"TRUE":"FALSE");
		printf("MutantInfo.bAbandoned            = %s\n", MutantInfo.bAbandoned?"TRUE":"FALSE");
	} else {
		printf("Unable to query mutant information\n");
	}
}


MutantManagement()
{
	NTSTATUS rc;
	HANDLE hMutantCreated, hMutantOpened;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING MutantName;
	ULONG bWasSignalled;
	LARGE_INTEGER Timeout;

	RtlInitUnicodeString(&MutantName, L"\\MyMutant");
	InitializeObjectAttributes(&ObjectAttr,
								&MutantName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateMutant(&hMutantCreated,
						STANDARD_RIGHTS_ALL|0x03,
						&ObjectAttr,
						TRUE);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create mutant, rc=%x\n", rc);
		return 0;
	} 

	printf("Mutant created, hMutantCreated=%x\n", hMutantCreated);
	rc=NtOpenMutant(&hMutantOpened,
						MAXIMUM_ALLOWED,
						&ObjectAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open mutant, rc=%x\n", rc);
		NtClose(hMutantCreated);
		return 0;
	}
	printf("Mutant opened, hMutantOpened=%x\n", hMutantOpened);
	printf("Information about the Mutant\n");
	DumpMutantInfo(hMutantCreated);

	Timeout.QuadPart=0;
	rc=NtWaitForSingleObject(hMutantCreated,
						FALSE,
						&Timeout);
	if (rc!=STATUS_SUCCESS) {
		printf("NtWaitForSingleObject failed, rc=%x\n", rc);
		return 0;
	}

#if 0
	Timeout.QuadPart=0;
	rc=NtWaitForMultipleObjects(1,
						&hMutantCreated,
						WaitAll,
						TRUE,
						&Timeout);
	if (rc!=STATUS_SUCCESS) {
		printf("NtWaitForMultipleObjects failed, rc=%x\n", rc);
		return 0;
	}
#endif

	rc=NtReleaseMutant(hMutantCreated, &bWasSignalled);
	if (rc!=STATUS_SUCCESS) {
		printf("NtReleseMutant failed, rc=%x\n", rc);
	}
	printf("bWasSignalled=%s\n", bWasSignalled?"NOT SIGNALLED":"SIGNALLED");

	printf("Information about the Mutant after releasing the mutant first time\n");
	DumpMutantInfo(hMutantCreated);

	rc=NtReleaseMutant(hMutantCreated, &bWasSignalled);
	if (rc!=STATUS_SUCCESS) {
		printf("NtReleseMutant failed, rc=%x\n", rc);
	}
	printf("bWasSignalled=%s\n", bWasSignalled?"NOT SIGNALLED":"SIGNALLED");


	printf("Information about the Mutant after releasing the mutant second time\n");
	DumpMutantInfo(hMutantCreated);

	NtClose(hMutantOpened);
	NtClose(hMutantCreated);
}

void DumpSemaphoreInfo(HANDLE hSemaphore)
{
	SEMAPHORE_INFO SemaphoreInfo;
	ULONG BytesReturned;
	NTSTATUS rc;

	rc=NtQuerySemaphore(hSemaphore,
					SemaphoreBasicInfo,
					&SemaphoreInfo,
					sizeof(SemaphoreInfo),
					&BytesReturned);
	if (rc==STATUS_SUCCESS) {
		printf("SemaphoreInfo.CurrentCount = %x\n", SemaphoreInfo.CurrentCount);
		printf("SemaphoreInfo.MaxCount     = %x\n", SemaphoreInfo.MaxCount);
	} else {
		printf("Unable to query Semaphore information\n");
	}
}



SemaphoreManagement()
{
	NTSTATUS rc;
	HANDLE hSemaphoreCreated, hSemaphoreOpened;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING SemaphoreName;
	ULONG PreviousCount;
	LARGE_INTEGER Timeout;

	RtlInitUnicodeString(&SemaphoreName, L"\\MySemaphore");
	InitializeObjectAttributes(&ObjectAttr,
								&SemaphoreName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateSemaphore(&hSemaphoreCreated,
						STANDARD_RIGHTS_ALL|0x03,
						&ObjectAttr,
						2,
						0x10);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create Semaphore, rc=%x\n", rc);
		return 0;
	} 

	printf("Semaphore created, hSemaphoreCreated=%x\n", hSemaphoreCreated);
	rc=NtOpenSemaphore(&hSemaphoreOpened,
						MAXIMUM_ALLOWED,
						&ObjectAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open Semaphore, rc=%x\n", rc);
		NtClose(hSemaphoreCreated);
		return 0;
	}
	printf("Semaphore opened, hSemaphoreOpened=%x\n", hSemaphoreOpened);

	printf("Information about the Semaphore\n");
	DumpSemaphoreInfo(hSemaphoreCreated);


	rc=NtReleaseSemaphore(hSemaphoreCreated, 2, &PreviousCount);
	if (rc!=STATUS_SUCCESS) {
		printf("NtReleaseSemaphore failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("Information about the Semaphore after releasing it 2 times\n");
	printf("Previous count = %x\n", PreviousCount);
	DumpSemaphoreInfo(hSemaphoreCreated);

	Timeout.QuadPart=0;
	rc=NtWaitForSingleObject(hSemaphoreCreated,
						FALSE,
						&Timeout);
	if (rc!=STATUS_SUCCESS) {
		printf("NtWaitForSingleObject failed, rc=%x\n", rc);
		return 0;
	}

	printf("Information about the Semaphore after aquiring it using NtWaitForSingleObject\n");
	DumpSemaphoreInfo(hSemaphoreCreated);

ExitFunction:
	NtClose(hSemaphoreOpened);
	NtClose(hSemaphoreCreated);

}

void _stdcall TimerCompletionRoutine(PVOID lpArgToCompletionRoutine,
										ULONG dwTimerLowValue,
										ULONG dwTimerHighValue)
{
	printf("TimerCompletionRoutine called, lpArgToCompletionRoutine=%x\n", lpArgToCompletionRoutine);
	return;
}

void DumpTimerInfo(HANDLE hTimer)
{
	NTSTATUS rc;
	TIMER_INFO TimerInfo;
	ULONG BytesReturned;

	rc=NtQueryTimer(hTimer,
					TimerBasicInfo,
					&TimerInfo,
					sizeof(TimerInfo),
					&BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryTimer failed, rc=%x\n", rc);
		return;
	}

	printf("TimerInfo.DueTime    = %08x%08x\n", TimerInfo.DueTime.HighPart, TimerInfo.DueTime.LowPart);
	printf("TimerInfo.TimerState = %s\n", TimerInfo.TimerState?"SIGNALLED":"NOT SIGNALLED");
}


TimerManagement()
{
	NTSTATUS rc;
	HANDLE hTimerCreated, hTimerOpened;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING TimerName;
	LARGE_INTEGER DueTime;
	LARGE_INTEGER DelayTime;
	BOOLEAN PresentState;


	RtlInitUnicodeString(&TimerName, L"\\MyTimer");
	InitializeObjectAttributes(&ObjectAttr,
								&TimerName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateTimer(&hTimerCreated,
						STANDARD_RIGHTS_ALL|0x03,
						&ObjectAttr,
						SynchronizationTimer);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create Timer, rc=%x\n", rc);
		return 0;
	} 

	printf("Timer created, hTimerCreated=%x\n", hTimerCreated);
	rc=NtOpenTimer(&hTimerOpened,
						MAXIMUM_ALLOWED,
						&ObjectAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open Timer, rc=%x\n", rc);
		NtClose(hTimerCreated);
		return 0;
	}
	printf("Timer opened, hTimerOpened=%x\n", hTimerOpened);
	printf("Dumping timer information\n");
	DumpTimerInfo(hTimerCreated);

	printf("Setting timer\n");

	DueTime.QuadPart=-(10^3);
	rc=NtSetTimer(hTimerCreated,
					&DueTime,
					TimerCompletionRoutine,
					10,
					FALSE,
					0,
					&PresentState);
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetTimer failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("PresentState=%s\n", PresentState?"SIGNALLED":"NOT SIGNALLED");

	printf("Waiting for timer expire\n");

	DelayTime.QuadPart = 10*1000*1000;   // 1 second 
	DelayTime.QuadPart = -(DelayTime.QuadPart); 

	rc=NtDelayExecution(TRUE, &DelayTime);
	if (rc!=STATUS_USER_APC) {
		printf("NtDelayExecution failed, rc=%x\n", rc);
		goto ExitFunction;
	} 

	printf("Dumping timer information after timer expire\n");
	DumpTimerInfo(hTimerCreated);

	printf("Again setting timer\n");

	DueTime.QuadPart=-(10^6);
	rc=NtSetTimer(hTimerCreated,
					&DueTime,
					TimerCompletionRoutine,
					10,
					FALSE,
					0,
					&PresentState);

	if (rc!=STATUS_SUCCESS) {
		printf("NtSetTimer failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("PresentState=%s\n", PresentState?"SIGNALLED":"NOT SIGNALLED");

	printf("Cancelling timer\n");


	rc=NtCancelTimer(hTimerCreated,
					&PresentState);

	if (rc!=STATUS_SUCCESS) {
		printf("NtCancelTimer failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("Timer state at cancellation=%s\n", PresentState?"SIGNALLED":"NOT SIGNALLED");
ExitFunction:
	NtClose(hTimerOpened);
	NtClose(hTimerCreated);
	return 0;
}


void TimerResolution()
{
	NTSTATUS rc;
	ULONG MinResolution, MaxResolution, SystemResolution;

	rc=NtQueryTimerResolution(&MaxResolution, &MinResolution, &SystemResolution);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryTimerResolution failed, rc=%x\n", rc);
	} else {
		ULONG ResolutionSet;

		printf("MaxResolution    = %x\n", MaxResolution);
		printf("MinResolution    = %x\n", MinResolution);
		printf("SystemResolution = %x\n", SystemResolution);

		rc=NtSetTimerResolution(MinResolution,
								TRUE,
								&ResolutionSet);
		if (rc!=STATUS_SUCCESS) {
			printf("NtSetTimerResolution failed, rc=%x\n", rc);
			return;
		}
		printf("ResolutionSet=%x\n", ResolutionSet);

		rc=NtSetTimerResolution(MinResolution,
								FALSE,
								&ResolutionSet);
		if (rc!=STATUS_SUCCESS) {
			printf("NtSetTimerResolution failed, rc=%x\n", rc);
			return;
		}
		printf("ResolutionSet=%x\n", ResolutionSet);
	}
}				

void PerformanceCounter()
{
	LARGE_INTEGER PerformanceCount;
	LARGE_INTEGER Frequency;
	NTSTATUS rc;

	PerformanceCount.QuadPart=0;
	Frequency.QuadPart=0;

	rc=NtQueryPerformanceCounter(&PerformanceCount, &Frequency);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryPerformanceCounter failed, rc=%x\n", rc);
	}

	printf("PerformanceCount = %08x%08x\n", PerformanceCount.HighPart, PerformanceCount.LowPart);
	printf("Frequency        = %08x%08x\n", Frequency.HighPart, Frequency.LowPart);

}

void DumpTime(TIME_FIELDS TimeFields)
{
	printf("TimeFields.Year         = %d\n", TimeFields.Year);
	printf("TimeFields.Month        = %d\n", TimeFields.Month);
	printf("TimeFields.Day          = %d\n", TimeFields.Day);
	printf("TimeFields.Hour         = %d\n", TimeFields.Hour);
	printf("TimeFields.Minute       = %d\n", TimeFields.Minute);
	printf("TimeFields.Second       = %d\n", TimeFields.Second);
	printf("TimeFields.Milliseconds = %d\n", TimeFields.Milliseconds);
	printf("TimeFields.Weekday      = %d\n", TimeFields.Weekday);
}

BOOLEAN EnableOrDisablePrivilege(ULONG PrivilegeId, BOOLEAN bDisable)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES PrivilegeSet;
	NTSTATUS rc;

	rc=NtOpenProcessToken(NtCurrentProcess(),
						TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
						&hToken);

	if (rc!=STATUS_SUCCESS) {
		printf("NtOpenProcessToken failed, rc=%x\n", rc);
		return FALSE;
	}

	memset(&PrivilegeSet, 0, sizeof(PrivilegeSet));
	PrivilegeSet.PrivilegeCount=1;
    PrivilegeSet.Privileges[0].Luid=RtlConvertUlongToLuid(PrivilegeId);
    PrivilegeSet.Privileges[0].Attributes = bDisable?0:SE_PRIVILEGE_ENABLED;

	rc=NtAdjustPrivilegesToken(hToken,
							FALSE,
							&PrivilegeSet,
							0,
							NULL,
							NULL);
	NtClose(hToken);

	if (rc!=STATUS_SUCCESS) {
		printf("NtAdjustPrivilegesToken failed, rc=%x\n", rc);
		return FALSE;
	}
	return TRUE;
}



void TimeManagement()
{
	LARGE_INTEGER SystemTime, NewSystemTime;
	NTSTATUS rc;
	TIME_FIELDS TimeFields;
	BOOLEAN retval;
	ULONG TickCount;

	if (!EnableOrDisablePrivilege(SE_SYSTEMTIME_PRIVILEGE, FALSE)) {
		printf("Unable to enable SE_SYSTEMTIME_PRIVILEGE\n");
		return;
	}

							

	rc=NtQuerySystemTime(&SystemTime);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemTime failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("Dumping System time\n");
	RtlTimeToTimeFields(&SystemTime, &TimeFields);
	DumpTime(TimeFields);


	TimeFields.Day++;
	retval=RtlTimeFieldsToTime(&TimeFields, &NewSystemTime);
	if (!retval) {
		printf("RtlTimeFieldsToTime failed\n");
		goto ExitFunction;
	}

	rc=NtSetSystemTime(&NewSystemTime, NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetSystemTime failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("Dumping System time after incrementing day by 1\n");

	rc=NtQuerySystemTime(&SystemTime);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemTime failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	RtlTimeToTimeFields(&SystemTime, &TimeFields);
	DumpTime(TimeFields);


	TimeFields.Day--;
	retval=RtlTimeFieldsToTime(&TimeFields, &NewSystemTime);
	if (!retval) {
		printf("RtlTimeFieldsToTime failed\n");
		goto ExitFunction;
	}

	rc=NtSetSystemTime(&NewSystemTime, NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetSystemTime failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("Dumping System time after decrementing day by 1\n");

	rc=NtQuerySystemTime(&SystemTime);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemTime failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	RtlTimeToTimeFields(&SystemTime, &TimeFields);
	DumpTime(TimeFields);

ExitFunction:
	TickCount=NtGetTickCount();
	printf("NtGetTickCount() returned %x\n", TickCount);

	EnableOrDisablePrivilege(SE_SYSTEMTIME_PRIVILEGE, TRUE);
}

void _stdcall RegApcRoutine(PVOID ApcContext, PIO_STATUS_BLOCK pIoStatusBlock, ULONG Reserved)
{
	printf("Registry change ApcRoutine called\n");
	return;
}


KeyManagement()
{
	HANDLE hKeyOpened, hKeyCreated, hKey;
	NTSTATUS rc;
	OBJECT_ATTRIBUTES ObjectAttr, HiveObjectAttr;
	UNICODE_STRING KeyName;
	ULONG Disposition;
	ULONG Index;
	PKEY_BASIC_INFORMATION pKeyBasicInfo;
	PKEY_VALUE_BASIC_INFORMATION pKeyValueBasicInfo;
	PKEY_FULL_INFORMATION pKeyFullInfo;
	PKEY_VALUE_FULL_INFORMATION pKeyValueFullInfo;
	HANDLE hFile;
	UNICODE_STRING FileName;
	IO_STATUS_BLOCK IoStatusBlock;
	LARGE_INTEGER DelayTime;
	KEY_VALUE_ENTRY KeyValueEntries[2];
	UNICODE_STRING KeyValueName[2];
	ULONG i;

	UNICODE_STRING ValueName;
	WCHAR *regszData=L"StringData";
	ULONG regdwordData=0x1020;
	static char Buffer[4096];
	ULONG BufferSize=sizeof(Buffer);
	ULONG BytesReturned;

	KEY_WRITE_TIME_INFORMATION KeyWriteTimeInfo;

	rc=NtInitializeRegistry(0);
	if (rc!=STATUS_SUCCESS) {
		printf("NtInitializeRegistry failed, rc=%x\n", rc);
	}


	/* Open HKEY_LOCAL_MACHINE\Software */
	RtlInitUnicodeString(&KeyName, L"\\Registry\\Machine\\Software");
	InitializeObjectAttributes(&ObjectAttr,
								&KeyName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtOpenKey(&hKeyOpened,
					MAXIMUM_ALLOWED,
					&ObjectAttr);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open key, rc=%x\n", rc);
		return 0;
	}
	printf("Key opened, hKeyOpened=%x\n", hKeyOpened);


	/* Create TestKey under HKEY_LOCAL_MACHINE\Software */
	RtlInitUnicodeString(&KeyName, L"TestKey");
	InitializeObjectAttributes(&ObjectAttr,
								&KeyName,
								OBJ_CASE_INSENSITIVE,
								hKeyOpened,
								NULL);
	rc=NtCreateKey(&hKeyCreated,
					KEY_ALL_ACCESS,
					&ObjectAttr,
					0,
					NULL,
					0,
					&Disposition);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create key, rc=%x\n", rc);
		NtClose(hKeyOpened);
		return 0;
	}
	printf("Key created, hKeyCreated=%x\n", hKeyCreated);


	rc=NtNotifyChangeKey(hKeyCreated,
						NULL,
						(PIO_APC_ROUTINE)RegApcRoutine,
						Buffer,
						&IoStatusBlock,
						REG_NOTIFY_CHANGE_NAME|REG_NOTIFY_CHANGE_ATTRIBUTES|REG_NOTIFY_CHANGE_LAST_SET|REG_NOTIFY_CHANGE_SECURITY,
						TRUE,
						Buffer,
						sizeof(Buffer),
						TRUE);

	if ((rc!=STATUS_SUCCESS)&&(rc!=STATUS_PENDING)) {
		printf("NtNotifyChangeKey failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	/* Create two subkeys under HKEY_LOCAL_MACHINE\Software\TestKey */
	RtlInitUnicodeString(&KeyName, L"Key1");
	InitializeObjectAttributes(&ObjectAttr,
									&KeyName,
									OBJ_CASE_INSENSITIVE,
									hKeyCreated,
									NULL);
	rc=NtCreateKey(&hKey,
					KEY_ALL_ACCESS,
					&ObjectAttr,
					0,
					NULL,
					0,
					&Disposition);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create key, rc=%x\n", rc);
		goto ExitFunction;
	}
	NtClose(hKey);

	RtlInitUnicodeString(&KeyName, L"Key2");
	InitializeObjectAttributes(&ObjectAttr,
									&KeyName,
									OBJ_CASE_INSENSITIVE,
									hKeyCreated,
									NULL);
	rc=NtCreateKey(&hKey,
					KEY_ALL_ACCESS,
					&ObjectAttr,
					0,
					NULL,
					0,
					&Disposition);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create key, rc=%x\n", rc);
		goto ExitFunction;
	}
	NtClose(hKey);

	/* Create two values under HKEY_LOCAL_MACHINE\Software\TestKey */
	RtlInitUnicodeString(&ValueName, L"Value1");
	rc=NtSetValueKey(hKeyCreated,
					&ValueName,
					0,
					REG_SZ,
					regszData,
					(wcslen(regszData)+1)*2);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to set value data, rc=%x\n", rc);
		goto ExitFunction;
	}

	RtlInitUnicodeString(&ValueName, L"Value2");
	rc=NtSetValueKey(hKeyCreated,
					&ValueName,
					0,
					REG_DWORD,
					&regdwordData,
					sizeof(regdwordData));

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to set value data, rc=%x\n", rc);
		goto ExitFunction;
	}

	DelayTime.QuadPart = 10*1000*1000;   // 1 second 
	DelayTime.QuadPart = -(DelayTime.QuadPart); 

	rc=NtDelayExecution(TRUE, &DelayTime);
	if ((rc!=STATUS_USER_APC)&&(rc!=STATUS_SUCCESS)) {
		printf("NtDelayExecution failed, rc=%x\n", rc);
		goto ExitFunction;
	} 


	/* Save key in hive */
	RtlInitUnicodeString(&FileName, L"\\Device\\HardDisk0\\Partition1\\hello.dat");
	InitializeObjectAttributes(&ObjectAttr,
								&FileName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	rc=NtCreateFile(&hFile,
					GENERIC_WRITE,
					&ObjectAttr,
					&IoStatusBlock,
					NULL,
					FILE_ATTRIBUTE_NORMAL,
					FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
					FILE_OPEN_IF,
					0,
					NULL,
					0);
	if (rc!=STATUS_SUCCESS) {
		printf("NtCreateFile failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	if (!EnableOrDisablePrivilege(SE_BACKUP_PRIVILEGE, FALSE)) {
		printf("EnableOrDisablePrivilege failed\n");
		goto ExitFunction;
	}

	rc=NtSaveKey(hKeyCreated,
				hFile);
	EnableOrDisablePrivilege(SE_BACKUP_PRIVILEGE, TRUE);

	if (rc!=STATUS_SUCCESS) {
		printf("NtSaveFile failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	NtClose(hFile);


	/* Load hive on key*/
	RtlInitUnicodeString(&KeyName, L"\\Registry\\Machine\\Prasad");
	InitializeObjectAttributes(&ObjectAttr,
									&KeyName,
									OBJ_CASE_INSENSITIVE,
									NULL,
									NULL);
	RtlInitUnicodeString(&FileName, L"\\Device\\HardDisk0\\Partition1\\hello.dat");
	InitializeObjectAttributes(&HiveObjectAttr,
								&FileName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	if (!EnableOrDisablePrivilege(SE_RESTORE_PRIVILEGE, FALSE)) {
		printf("EnableOrDisablePrivilege failed\n");
		goto ExitFunction;
	}

	rc=NtLoadKey2(&ObjectAttr, &HiveObjectAttr, REG_NO_LAZY_FLUSH);
	EnableOrDisablePrivilege(SE_RESTORE_PRIVILEGE, TRUE);

	if (rc!=STATUS_SUCCESS) {
		printf("NtLoadKey failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	
	if (!EnableOrDisablePrivilege(SE_RESTORE_PRIVILEGE, FALSE)) {
		printf("EnableOrDisablePrivilege failed\n");
		goto ExitFunction;
	}
	rc=NtUnloadKey(&ObjectAttr);
	EnableOrDisablePrivilege(SE_RESTORE_PRIVILEGE, TRUE);
	if (rc!=STATUS_SUCCESS) {
		printf("NtUnloadKey failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	/* Query Key */
	rc=NtQueryKey(hKeyCreated,
					KeyFullInformation,
					Buffer,
					sizeof(Buffer),
					&BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryKey failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	pKeyFullInfo=(PKEY_FULL_INFORMATION)Buffer;
	printf("pKeyFullInfo->SubKeys         = %d\n", pKeyFullInfo->SubKeys);
	printf("pKeyFullInfo->MaxNameLen      = %d\n", pKeyFullInfo->MaxNameLen);
	printf("pKeyFullInfo->Values          = %d\n", pKeyFullInfo->Values);
	printf("pKeyFullInfo->MaxValueNameLen = %d\n", pKeyFullInfo->MaxValueNameLen);
	printf("pKeyFullInfo->MaxValueDataLen = %d\n", pKeyFullInfo->MaxValueDataLen);

	/* Query Value */
	RtlInitUnicodeString(&ValueName, L"Value1");
	rc=NtQueryValueKey(hKeyCreated,
					&ValueName,
					KeyValueFullInformation,
					Buffer,
					sizeof(Buffer),
					&BytesReturned);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryValueKey failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	pKeyValueFullInfo=(PKEY_VALUE_FULL_INFORMATION)Buffer;
	printf("pKeyValueFullInfo->DataLength = %d\n", pKeyValueFullInfo->DataLength);
	printf("pKeyValueFullInfo->NameLength = %d\n", pKeyValueFullInfo->NameLength);
	printf("pKeyValueFullInfo->Type       = ", pKeyValueFullInfo->DataLength);
	switch (pKeyValueFullInfo->Type) {
		case REG_SZ: 
			printf("(REG_SZ)\n");
			break;
		case REG_DWORD:
			printf("(REG_DWORD)\n");
			break;
		default:
			printf("(%x)\n", pKeyValueFullInfo->Type);
	}

	

	RtlInitUnicodeString(&KeyValueName[0], L"Value1");
	RtlInitUnicodeString(&KeyValueName[1], L"Value2");
	KeyValueEntries[0].ValueName=&KeyValueName[0];
	KeyValueEntries[1].ValueName=&KeyValueName[1];

	rc=NtQueryMultipleValueKey(hKeyCreated,
							KeyValueEntries,
							2,
							Buffer,
							&BufferSize,
							&BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryMultipleValueKey failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	for (i=0; i<2; i++) {
		printf("%S ", KeyValueEntries[i].ValueName->Buffer);
		switch(KeyValueEntries[i].Type) {
			case REG_SZ:
				printf("REG_SZ %S\n", Buffer+KeyValueEntries[i].DataOffset);
				break;
			case REG_DWORD:
				printf("REG_DWORD %x\n", *((ULONG *)(Buffer+KeyValueEntries[i].DataOffset)));
				break;
		}
	}
							

	/* Enumerate values under HKEY_LOCAL_MACHINE\Software\TestKey */
	Index=0;
	rc=NtEnumerateValueKey(hKeyCreated,
						Index,
						KeyValueBasicInformation,
						Buffer,
						sizeof(Buffer),
						&BytesReturned);
	while (rc==STATUS_SUCCESS) {
		*(PWCHAR)(&Buffer[BytesReturned])=L'\0';
		pKeyValueBasicInfo=(PKEY_VALUE_BASIC_INFORMATION)Buffer;
		printf("%S ", pKeyValueBasicInfo->Name);
		switch (pKeyValueBasicInfo->Type) {
			case REG_SZ: 
				printf("(REG_SZ)\n");
				break;
			case REG_DWORD:
				printf("(REG_DWORD)\n");
				break;
			default:
				printf("(%x)\n", pKeyValueBasicInfo->Type);
		}
		Index++;

		RtlInitUnicodeString(&ValueName, pKeyValueBasicInfo->Name);


		rc=NtEnumerateValueKey(hKeyCreated,
							Index,
							KeyValueBasicInformation,
							Buffer,
							sizeof(Buffer),
							&BytesReturned);
	}


	/* Enumerate subkeys under HKEY_LOCAL_MACHINE\Software\TestKey */
	Index=0;
	rc=NtEnumerateKey(hKeyCreated,
						Index,
						KeyBasicInformation,
						Buffer,
						sizeof(Buffer),
						&BytesReturned);
	while (rc==STATUS_SUCCESS) {
		*(PWCHAR)(&Buffer[BytesReturned])=L'\0';
		pKeyBasicInfo=(PKEY_BASIC_INFORMATION)Buffer;
		printf("%x %S\n", pKeyBasicInfo->NameLength, pKeyBasicInfo->Name);
		Index++;

		rc=NtEnumerateKey(hKeyCreated,
							Index,
							KeyBasicInformation,
							Buffer,
							sizeof(Buffer),
							&BytesReturned);
	}

	/* Delete keys under HKEY_LOCAL_MACHINE\Software\TestKey */
	RtlInitUnicodeString(&KeyName, L"Key1");
	InitializeObjectAttributes(&ObjectAttr,
								&KeyName,
								OBJ_CASE_INSENSITIVE,
								hKeyCreated,
								NULL);

	rc=NtOpenKey(&hKey,
					MAXIMUM_ALLOWED,
					&ObjectAttr);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open key, rc=%x\n", rc);
		goto ExitFunction;
	}

	rc=NtDeleteKey(hKey);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to delete key, rc=%x\n", rc);
		NtClose(hKey);
		goto ExitFunction;
	}
	NtClose(hKey);

	RtlInitUnicodeString(&KeyName, L"Key2");
	InitializeObjectAttributes(&ObjectAttr,
								&KeyName,
								OBJ_CASE_INSENSITIVE,
								hKeyCreated,
								NULL);

	rc=NtOpenKey(&hKey,
					MAXIMUM_ALLOWED,
					&ObjectAttr);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open key, rc=%x\n", rc);
		goto ExitFunction;
	}

	rc=NtDeleteKey(hKey);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to delete key, rc=%x\n", rc);
		NtClose(hKey);
		goto ExitFunction;
	}
	NtClose(hKey);


	/* Delete values under HKEY_LOCAL_MACHINE\Software\TestKey */
	RtlInitUnicodeString(&ValueName, L"Value1");
	rc=NtDeleteValueKey(hKeyCreated,
						&ValueName);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to delete value, rc=%x\n", rc);
		goto ExitFunction;
	}

	RtlInitUnicodeString(&ValueName, L"Value2");
	rc=NtDeleteValueKey(hKeyCreated,
						&ValueName);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to delete value, rc=%x\n", rc);
		goto ExitFunction;
	}

	NtQuerySystemTime(&KeyWriteTimeInfo.LastWriteTime);

	NtSetInformationKey(hKeyCreated,
							KeyWriteTimeInformation,
							&KeyWriteTimeInfo,
							sizeof(KeyWriteTimeInfo));

	rc=NtDeleteKey(hKeyCreated);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to delete key, rc=%x\n", rc);
	}


ExitFunction:
	NtClose(hKeyCreated);

	rc=NtFlushKey(hKeyOpened);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to flush key, rc=%x\n", rc);
	}
	NtClose(hKeyOpened);
}

#define STACK_SIZE 10000 
static ULONG Stack[STACK_SIZE];


void _stdcall ThreadApcRoutine(PVOID ApcContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
	printf("ThreadApcRoutine called\n");
	NtTerminateThread(NtCurrentThread(), 0);
	return;
}


void NewThreadFunc()
{
	while (1) {
		LARGE_INTEGER Timeout;
		NTSTATUS rc;

		printf("Inside NewThreadFunc\n");

		Timeout.QuadPart=0;
		rc=NtDelayExecution(TRUE, &Timeout);
		if (rc!=STATUS_SUCCESS) {
			printf("NtDelayExecution failed, rc=%x\n", rc);
			return;
		} 
	}
}

void ThreadFunc(int Param)
{
	while (1) {
		printf("Inside ThreadFunc\n");
	}
}


ThreadManagement()
{
	UNICODE_STRING ThreadName;
	OBJECT_ATTRIBUTES ObjectAttr;
	NTSTATUS rc;
	HANDLE hThreadCreated, hThreadOpened;
	CLIENT_ID ClientId;
	CONTEXT Context;
	STACKINFO StackInfo;
	LARGE_INTEGER Timeout;
	ULONG SuspendCount;

	RtlInitUnicodeString(&ThreadName, L"\\MyThread");

	InitializeObjectAttributes(&ObjectAttr,
								&ThreadName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	StackInfo.TopOfStack=(ULONG)&Stack[STACK_SIZE];
	StackInfo.BottomOfStack=(ULONG)&Stack[-1];
	StackInfo.OnePageBelowTopOfStack=(ULONG)(&Stack[STACK_SIZE]-4096);


	Context.ContextFlags=CONTEXT_FULL;
	rc=NtGetContextThread(NtCurrentThread(), &Context);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to get context of current thread\n");
		return 0;
	}

	Context.Eip=(ULONG)ThreadFunc;
	Context.Esp=StackInfo.TopOfStack;

	rc=NtCreateThread(&hThreadCreated,
						THREAD_ALL_ACCESS,
						&ObjectAttr,
						NtCurrentProcess(),
						&ClientId,
						&Context,
						&StackInfo,
						FALSE);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create thread, rc=%x\n", rc);
		return 0;
	}

	/* Open thread by name */
	rc=NtOpenThread(&hThreadOpened,
					MAXIMUM_ALLOWED,
					&ObjectAttr,
					NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtOpenThread with threadname failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("hThreadOpened=%x\n", hThreadOpened);
	NtClose(hThreadOpened);

	/* Open thread by Client id */
	InitializeObjectAttributes(&ObjectAttr,
								NULL,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	rc=NtOpenThread(&hThreadOpened,
					MAXIMUM_ALLOWED,
					&ObjectAttr,
					&ClientId);
	if (rc!=STATUS_SUCCESS) {
		printf("NtOpenThread with threadid failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("hThreadOpened=%x\n", hThreadOpened);
	NtClose(hThreadOpened);


	/* Give some chance for the created thread to execute */
	NtYieldExecution();


	/* Suspend, change context and resume*/
	rc=NtSuspendThread(hThreadCreated,
						&SuspendCount);
	if (rc!=STATUS_SUCCESS) {
		printf("NtSuspendThread failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	Context.ContextFlags=CONTEXT_FULL;
	rc=NtGetContextThread(hThreadCreated,
						&Context);
	if (rc!=STATUS_SUCCESS) {
		printf("NtGetContextThread failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	Context.Eip=(ULONG)NewThreadFunc;
	rc=NtSetContextThread(hThreadCreated,
						&Context);
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetContextThread failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	rc=NtResumeThread(hThreadCreated,
						&SuspendCount);
	if (rc!=STATUS_SUCCESS) {
		printf("NtResumeThread failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	/* Give some chance for the thread after changing the context */
	NtYieldExecution();


	/* Queue and APC to thread */
	rc=NtQueueApcThread(hThreadCreated,
						(PKNORMAL_ROUTINE)ThreadApcRoutine,
						NULL,
						NULL,
						NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQueueApcThread failed, rc=%x\n", rc);
		goto ExitFunction;
	}


	/* Wait for the thread to terminate */
	Timeout.QuadPart=-(10^20);
	rc=NtWaitForSingleObject(hThreadCreated,
						TRUE,
						&Timeout);
	if (rc!=STATUS_SUCCESS) {
		printf("NtWaitForSingleObject failed, rc=%x\n", rc);
		goto ExitFunction;
	}

ExitFunction:
	NtClose(hThreadCreated);
}


SectionManagement()
{
	NTSTATUS rc;
	HANDLE hSectionCreated, hSectionOpened;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING SectionName;
	LARGE_INTEGER MaxSize;
	ULONG SectionSize=8192;
	PVOID BaseAddress=NULL;


	RtlInitUnicodeString(&SectionName, L"\\MySection");
	InitializeObjectAttributes(&ObjectAttr,
								&SectionName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	MaxSize.HighPart=0;
	MaxSize.LowPart=SectionSize;

	rc=NtCreateSection(&hSectionCreated,
					SECTION_ALL_ACCESS,
					&ObjectAttr,
					&MaxSize,
					PAGE_READWRITE,
					SEC_COMMIT,
					NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create section, rc=%x\n", rc);
		return 0;
	}
	printf("hSectionCreated=%x\n", hSectionCreated);

	rc=NtOpenSection(&hSectionOpened,
					MAXIMUM_ALLOWED,
					&ObjectAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open section, rc=%x\n", rc);
		NtClose(hSectionCreated);
		return 0;
	}
	printf("hSectionOpened=%x\n", hSectionOpened);

	rc=NtMapViewOfSection(hSectionCreated,
						NtCurrentProcess(),
						&BaseAddress,
						0,
						SectionSize,
						NULL,
						&SectionSize,
					    ViewShare,
						0,
						PAGE_READWRITE);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to map section, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("Section mapped @%x, size=%x\n", BaseAddress, SectionSize);

	memset(BaseAddress, 0, SectionSize);


	rc=NtUnmapViewOfSection(NtCurrentProcess(),
							BaseAddress);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to unmap section, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("Section unmapped\n");

ExitFunction:
	NtClose(hSectionCreated);
	NtClose(hSectionOpened);
}

HandleException(PVOID BaseAddress, ULONG SizeRequestedOrAllocated)
{
	NTSTATUS rc;
	rc=NtAllocateVirtualMemory(NtCurrentProcess(),
							&BaseAddress,
							3,
							&SizeRequestedOrAllocated,
							MEM_COMMIT,
							PAGE_READWRITE);
	if (rc!=STATUS_SUCCESS) {
		printf("NtAllocateVirtualMemory failed, rc=%x\n", rc);
		return EXCEPTION_EXECUTE_HANDLER;
	}
	printf("Memory commited\n");
	return EXCEPTION_CONTINUE_EXECUTION;
}


DumpMemoryInformation(PVOID BaseAddress, ULONG SizeRequestedAllocated)
{
	char Buffer[4096];
	PMEMORY_BASIC_INFORMATION pMemoryBasicInfo;
	ULONG BytesReturned;
	NTSTATUS rc;

	memset(Buffer, 0, sizeof(Buffer));
	rc=NtQueryVirtualMemory(NtCurrentProcess(),
							BaseAddress,
							MemoryBasicInformation,
							Buffer,
							sizeof(Buffer),
							&BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryVirtualMemory failed, rc=%x\n", rc);
		return 0;
	}
	pMemoryBasicInfo=(PMEMORY_BASIC_INFORMATION)Buffer;
	printf("BytesReturned=%x\n", BytesReturned);

	printf("pMemoryBasicInfo->BaseAddress   = %x\n", pMemoryBasicInfo->BaseAddress);
	printf("pMemoryBasic->AllocationBase    = %x\n", pMemoryBasicInfo->AllocationBase);
	printf("pMemoryBasic->AllocationProtect = %x\n", pMemoryBasicInfo->AllocationProtect);
	printf("pMemoryBasic->RegionSize        = %x\n", pMemoryBasicInfo->RegionSize);
	printf("pMemoryBasic->State             = %x\n", pMemoryBasicInfo->State);
	printf("pMemoryBasic->Protect           = %x\n", pMemoryBasicInfo->Protect);
	printf("pMemoryBasic->Type              = %x\n", pMemoryBasicInfo->Type);
}


MemoryManagement()
{
	PVOID BaseAddress=NULL;
	ULONG SizeRequestedAllocated=1;
	NTSTATUS rc;
	ULONG OldProtect;
	char Buffer[100];
	ULONG Len, BytesRead, BytesWritten;


	rc=NtAllocateVirtualMemory(NtCurrentProcess(),
							&BaseAddress,
							3,
							&SizeRequestedAllocated,
							MEM_RESERVE,
							PAGE_READWRITE);
	if (rc!=STATUS_SUCCESS) {
		printf("NtAllocateVirtualMemory failed, rc=%x\n", rc);
		return 0;
	}
	printf("Memory of size %x allocated @%x\n", SizeRequestedAllocated, BaseAddress);


	__try {
		memset(BaseAddress, 'A', SizeRequestedAllocated);
	}
	__except (HandleException(BaseAddress, SizeRequestedAllocated)) {
	}		


	DumpMemoryInformation(BaseAddress, SizeRequestedAllocated);


	rc=NtProtectVirtualMemory(NtCurrentProcess(),
							&BaseAddress,
							&SizeRequestedAllocated,
							PAGE_READONLY,
							&OldProtect);
	if (rc!=STATUS_SUCCESS) {
		printf("NtProtectVirtualMemory failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("OldProtect=%x\n", OldProtect);
	DumpMemoryInformation(BaseAddress, SizeRequestedAllocated);

	if (!EnableOrDisablePrivilege(SE_LOCK_MEMORY_PRIVILEGE, FALSE)) {
		printf("EnableOrDisablePrivilege failed\n");
		goto ExitFunction;
	}

	rc=NtLockVirtualMemory(NtCurrentProcess(),
						&BaseAddress,
						&SizeRequestedAllocated,
						1);

	EnableOrDisablePrivilege(SE_LOCK_MEMORY_PRIVILEGE, TRUE);
	if (rc!=STATUS_SUCCESS) {
		printf("NtLockVirtualMemory failed, rc=%x\n", rc);
		goto ExitFunction;
	}


	if (!EnableOrDisablePrivilege(SE_LOCK_MEMORY_PRIVILEGE, FALSE)) {
		printf("EnableOrDisablePrivilege failed\n");
		goto ExitFunction;
	}

	rc=NtUnlockVirtualMemory(NtCurrentProcess(),
						&BaseAddress,
						&SizeRequestedAllocated,
						1);


	EnableOrDisablePrivilege(SE_LOCK_MEMORY_PRIVILEGE, TRUE);
	if (rc!=STATUS_SUCCESS) {
		printf("NtunlockVirtualMemory failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	rc=NtProtectVirtualMemory(NtCurrentProcess(),
							&BaseAddress,
							&SizeRequestedAllocated,
							PAGE_READWRITE,
							&OldProtect);
	if (rc!=STATUS_SUCCESS) {
		printf("NtProtectVirtualMemory failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	strcpy(Buffer, "Hello");
	Len=strlen(Buffer)+1;

	rc=NtWriteVirtualMemory(NtCurrentProcess(),
							BaseAddress,
							Buffer,
							Len,
							&BytesWritten);
	if (rc!=STATUS_SUCCESS) {
		printf("NtWriteVirtualMemory failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("BytesWritten=%x\n", BytesWritten);

	memset(Buffer, 0, sizeof(Buffer));

	rc=NtReadVirtualMemory(NtCurrentProcess(),
							BaseAddress,
							Buffer,
							Len,
							&BytesRead);
	if (rc!=STATUS_SUCCESS) {
		printf("NtWriteVirtualMemory failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("BytesRead=%x %s\n", BytesRead, Buffer);


ExitFunction:
	rc=NtFreeVirtualMemory(NtCurrentProcess(),
							&BaseAddress,
							&SizeRequestedAllocated,
							MEM_DECOMMIT);
	if (rc!=STATUS_SUCCESS) {
		printf("NtFreeVirtualMemory failed, rc=%x\n", rc);
		return 0;
	}

	rc=NtFreeVirtualMemory(NtCurrentProcess(),
							&BaseAddress,
							&SizeRequestedAllocated,
							MEM_RELEASE);
	if (rc!=STATUS_SUCCESS) {
		printf("NtFreeVirtualMemory failed, rc=%x\n", rc);
		return 0;
	}
}

CompletionPort()
{
	NTSTATUS rc;
	HANDLE hIoCompletionPortCreated, hIoCompletionPortOpened;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING IoCompletionPortName;
	ULONG BytesReturned;
	IOCOMPLETIONPORT_BASIC_INFO IoCompletionPortBasicInformation;
	IO_STATUS_BLOCK IoStatusBlock;
	LPOVERLAPPED lpOverLapped=NULL;
	OVERLAPPED OverLapped;
	ULONG CompletionKey;

	RtlInitUnicodeString(&IoCompletionPortName, L"\\MyIoCompletionPort");

	InitializeObjectAttributes(&ObjectAttr,
								&IoCompletionPortName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateIoCompletion(&hIoCompletionPortCreated,
							0x1F0003,
							&ObjectAttr,
							0);
	if (rc!=STATUS_SUCCESS) {
		printf("NtCreateIoCompletionPort failed, rc=%x\n", rc);
		return 0;
	}
	printf("hIoCompletionPortCreated=%x\n", hIoCompletionPortCreated);


	rc=NtOpenIoCompletion(&hIoCompletionPortOpened,
							MAXIMUM_ALLOWED,
							&ObjectAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("NtOpenIoCompletionPort failed, rc=%x\n", rc);
		NtClose(hIoCompletionPortCreated);
		return 0;
	}
	printf("hIoCompletionPortOpened=%x\n", hIoCompletionPortOpened);

	printf("Address of OverLapped=%x\n", &OverLapped);
	rc=NtSetIoCompletion(hIoCompletionPortCreated,
						1,
						&OverLapped,
						STATUS_SUCCESS,
						0x10);

	if (rc!=STATUS_SUCCESS) {
		printf("NtSetIoCompletion failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	rc=NtSetIoCompletion(hIoCompletionPortCreated,
						2,
						&OverLapped,
						STATUS_ACCESS_DENIED,
						0x20);

	if (rc!=STATUS_SUCCESS) {
		printf("NtSetIoCompletion failed, rc=%x\n", rc);
		goto ExitFunction;
	}


	
	rc=NtQueryIoCompletion(hIoCompletionPortCreated,
						IoCompletionPortBasicInfo,
						&IoCompletionPortBasicInformation,
						sizeof(IoCompletionPortBasicInformation),
						&BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryIoCompletion failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("BytesReturned=%x, IoCompletionPortBasicInformation.NumberOfEvents=%x\n", BytesReturned, IoCompletionPortBasicInformation.NumberOfEvents);


	rc=NtRemoveIoCompletion(hIoCompletionPortCreated,
							&CompletionKey,
							&lpOverLapped,
							&IoStatusBlock,
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtRemoveIoCompletion failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("CompletionKey             = %x\n", CompletionKey);
	printf("lpOverLapped              = %x\n", lpOverLapped);
	printf("IoStatusBlock.Information = %x\n", IoStatusBlock.Information);
	printf("IoStatusBlock.Status      = %x\n\n", IoStatusBlock.Status);

	rc=NtRemoveIoCompletion(hIoCompletionPortCreated,
							&CompletionKey,
							&lpOverLapped,
							&IoStatusBlock,
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtRemoveIoCompletion failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("CompletionKey             = %x\n", CompletionKey);
	printf("lpOverLapped              = %x\n", lpOverLapped);
	printf("IoStatusBlock.Information = %x\n", IoStatusBlock.Information);
	printf("IoStatusBlock.Status      = %x\n", IoStatusBlock.Status);


ExitFunction:
	NtClose(hIoCompletionPortCreated);
	NtClose(hIoCompletionPortOpened);
}

DeleteFile()
{
	UNICODE_STRING FileName;
	OBJECT_ATTRIBUTES ObjAttr;
	NTSTATUS rc;

	RtlInitUnicodeString(&FileName, L"\\??\\h:\\aa");

	InitializeObjectAttributes(&ObjAttr,
								&FileName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	
	rc=NtDeleteFile(&ObjAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("NtDeleteFile failed, rc=%x\n", rc);
		return 0;
	}
	printf("File deleted\n");
}


QueryFileAttributes()
{
	UNICODE_STRING FileName;
	OBJECT_ATTRIBUTES ObjAttr;
	NTSTATUS rc;
	FILE_BASIC_INFORMATION FileBasicInfo;

	printf("%x\n", sizeof(FileBasicInfo));

	RtlInitUnicodeString(&FileName, L"\\??\\h:\\aa");

	InitializeObjectAttributes(&ObjAttr,
								&FileName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtQueryAttributesFile(&ObjAttr,
							&FileBasicInfo);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryAttributesFile failed, rc=%x\n", rc);
		return 0;
	}
	
	printf("FileBasicInfo.CreationTime   = %08x%08x\n", FileBasicInfo.CreationTime);
	printf("FileBasicInfo.LastAccessTime = %08x%08x\n", FileBasicInfo.LastAccessTime);
	printf("FileBasicInfo.LastWriteTime  = %08x%08x\n", FileBasicInfo.LastWriteTime);
	printf("FileBasicInfo.ChangeTime     = %08x%08x\n", FileBasicInfo.ChangeTime);
	printf("FileBasicInfo.FileAttributes = %08x\n", FileBasicInfo.FileAttributes);
}

uuid()
{
	LUID Luid;
	NTSTATUS rc;

	memset(&Luid, 0, sizeof(Luid));
	rc=NtAllocateLocallyUniqueId(&Luid);
	if (rc!=STATUS_SUCCESS) {
		printf("NtAllocateLocallyUniqueId failed, rc=%x\n", rc);
		return 0;
	}
	printf("Luid.LowPart  = %x\n", Luid.LowPart);
	printf("Luid.HighPart = %x\n", Luid.HighPart);
}

void JobManagement()
{
	NTSTATUS rc;
	HANDLE hJobCreated, hJobOpened;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING JobName;
	PFNNTCREATEJOBOBJECT pfnNtCreateJobObject;
	PFNNTOPENJOBOBJECT pfnNtOpenJobObject;
	PFNNTASSIGNPROCESSTOJOBOBJECT pfnNtAssignProcessToJobObject;
	PFNNTTERMINATEJOBOBJECT pfnNtTerminateJobObject;
	PFNNTQUERYINFORMATIONJOBOBJECT pfnNtQueryInformationJobObject;
	PFNNTSETINFORMATIONJOBOBJECT pfnNtSetInformationJobObject;

	PVOID _stdcall GetProcAddress(PVOID hModule, PCCHAR FunctionName);
	PVOID _stdcall GetModuleHandleA(PCCHAR ModuleName);
	static char Buffer[4096];
	PJOBOBJECT_BASIC_PROCESS_ID_LIST pJobObjectBasicProcessIdList;
	PJOBOBJECT_BASIC_UI_RESTRICTIONS pJobObjectBasicUiRestrictions;

	ULONG i, BytesReturned;
	

	pfnNtCreateJobObject=(PFNNTCREATEJOBOBJECT)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtCreateJobObject");
	if (!pfnNtCreateJobObject) {
		return;
	}
	pfnNtOpenJobObject=(PFNNTOPENJOBOBJECT)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtOpenJobObject");
	if (!pfnNtOpenJobObject) {
		return;
	}
	pfnNtAssignProcessToJobObject=(PFNNTASSIGNPROCESSTOJOBOBJECT)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtAssignProcessToJobObject");
	if (!pfnNtAssignProcessToJobObject) {
		return;
	}
	pfnNtTerminateJobObject=(PFNNTTERMINATEJOBOBJECT)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtTerminateJobObject");

	if (!pfnNtTerminateJobObject) {
		return;
	}

	pfnNtQueryInformationJobObject=(PFNNTQUERYINFORMATIONJOBOBJECT)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtQueryInformationJobObject");

	if (!pfnNtQueryInformationJobObject) {
		return;
	}

	pfnNtSetInformationJobObject=(PFNNTSETINFORMATIONJOBOBJECT)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtSetInformationJobObject");

	if (!pfnNtSetInformationJobObject) {
		return;
	}

	RtlInitUnicodeString(&JobName, L"\\MyJob");
	InitializeObjectAttributes(&ObjectAttr,
								&JobName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);

	rc=pfnNtCreateJobObject(&hJobCreated,
						JOB_OBJECT_ALL_ACCESS,
						&ObjectAttr);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create job, rc=%x\n", rc);
		return;
	}

	printf("hJobCreated=%x\n", hJobCreated);

	rc=pfnNtOpenJobObject(&hJobOpened,
						JOB_OBJECT_ALL_ACCESS,
						&ObjectAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open job, rc=%x\n", rc);
		NtClose(hJobCreated);
		return;
	}

	printf("hJobOpened=%x\n", hJobOpened);
	rc=pfnNtAssignProcessToJobObject(hJobCreated,
									NtCurrentProcess());

	if (rc!=STATUS_SUCCESS) {
		printf("NtAssignProcessToJobObject failed, rc=%x\n", rc);
		goto ExitFunction;
	}
	printf("Process assigned to job\n");


	rc=pfnNtQueryInformationJobObject(hJobCreated,
									JobObjectBasicProcessIdList,
									Buffer,
									sizeof(Buffer),
									&BytesReturned);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryInformationJobObject failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	pJobObjectBasicProcessIdList=(PJOBOBJECT_BASIC_PROCESS_ID_LIST)Buffer;

	printf("pJobObjectBasicProcessIdList->NumberOfAssignedProcesses = %x\n", pJobObjectBasicProcessIdList->NumberOfAssignedProcesses);
	printf("pJobObjectBasicProcessIdList->NumberOfProcessIdsInList  = %x\n", pJobObjectBasicProcessIdList->NumberOfProcessIdsInList);
	for (i=0; i<pJobObjectBasicProcessIdList->NumberOfProcessIdsInList; i++) {
		printf("%x ", pJobObjectBasicProcessIdList->ProcessIdList[i]);
	}
	printf("\n");

	pJobObjectBasicUiRestrictions=(PJOBOBJECT_BASIC_UI_RESTRICTIONS)Buffer;
	pJobObjectBasicUiRestrictions->UIRestrictionsClass=JOB_OBJECT_UILIMIT_DESKTOP;

	rc=pfnNtSetInformationJobObject(hJobCreated,
									JobObjectBasicUIRestrictions,
									pJobObjectBasicUiRestrictions,
									sizeof(*pJobObjectBasicUiRestrictions));

	if (rc!=STATUS_SUCCESS) {
		printf("NtSetInformationJobObject failed, rc=%x\n", rc);
		goto ExitFunction;
	}

	printf("NtSetInformationJobObject success\n");


ExitFunction:
	NtClose(hJobOpened);
	NtClose(hJobCreated);
}

void Language()
{
	PVOID _stdcall GetProcAddress(PVOID hModule, PCCHAR FunctionName);
	PVOID _stdcall GetModuleHandleA(PCCHAR ModuleName);
	PFNNTQUERYDEFAULTUILANGUAGE pfnNtQueryDefaultUILanguage;
	PFNNTSETDEFAULTUILANGUAGE pfnNtSetDefaultUILanguage;
	USHORT DefaultUILanguage;
	NTSTATUS rc;


	pfnNtQueryDefaultUILanguage=(PFNNTQUERYDEFAULTUILANGUAGE)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtQueryDefaultUILanguage");
	if (!pfnNtQueryDefaultUILanguage) {
		return;
	}

	pfnNtSetDefaultUILanguage=(PFNNTSETDEFAULTUILANGUAGE)GetProcAddress(GetModuleHandleA("NTDLL.DLL"), 
										"NtSetDefaultUILanguage");
	if (!pfnNtSetDefaultUILanguage) {
		return;
	}

	rc=pfnNtQueryDefaultUILanguage(&DefaultUILanguage);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryDefaultUILanguage failed, rc=%x\n", rc);
		return;
	}
	printf("DefaultUILanguage=%x %x %x\n", DefaultUILanguage, PRIMARYLANGID(DefaultUILanguage), SUBLANGID(DefaultUILanguage));


	rc=pfnNtSetDefaultUILanguage((MAKELANGID(LANG_RUSSIAN,SUBLANG_DEFAULT)));
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetDefaultUILanguage failed, rc=%x\n", rc);
		return;
	}
}

void PageFile()
{
	NTSTATUS rc;
	UNICODE_STRING PagingFileName;
	LARGE_INTEGER InitialSize;
	LARGE_INTEGER MaxSize;

	if (!EnableOrDisablePrivilege(SE_CREATE_PAGEFILE_PRIVILEGE, FALSE)) {
		printf("Unable to enable SE_CREATE_PAGEFILE_PRIVILEGE\n");
		return;
	}

	InitialSize.QuadPart=4*1024*1024;
	MaxSize.QuadPart=10*1024*1024;
	RtlInitUnicodeString(&PagingFileName, L"\\??\\C:\\MYPAGEFILE.SYS");


	rc=NtCreatePagingFile(&PagingFileName,
							&InitialSize,
							&MaxSize,
							0);
	EnableOrDisablePrivilege(SE_CREATE_PAGEFILE_PRIVILEGE, TRUE);

	if (rc!=STATUS_SUCCESS) {
		printf("NtCreatePagingFile failed, rc=%x\n", rc);
		return;
	}
	printf("Paging file created\n");
}


ProcessManagement()
{
	NTSTATUS rc;
	HANDLE hProcessCreated, hProcessOpened;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING ProcessName;

	RtlInitUnicodeString(&ProcessName, L"\\MyProcess");
	InitializeObjectAttributes(&ObjectAttr,
								&ProcessName,
								OBJ_CASE_INSENSITIVE,
								NULL,
								NULL);
	rc=NtCreateProcess(&hProcessCreated,
						PROCESS_ALL_ACCESS,
						&ObjectAttr,
						NtCurrentProcess(),
						TRUE,
						NULL,
						NULL,
						NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create process, rc=%x\n", rc);
		return 0;
	}
	printf("hProcessCreated=%x\n", hProcessCreated);

	rc=NtOpenProcess(&hProcessOpened,
						PROCESS_ALL_ACCESS,
						&ObjectAttr,
						NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open process, rc=%x\n", rc);
		NtClose(hProcessCreated);
		return 0;
	}
	printf("hProcessOpened=%x\n", hProcessOpened);

	NtClose(hProcessOpened);
	NtClose(hProcessCreated);
}

void Locale()
{
	ULONG SystemLocale, ThreadLocale;
	NTSTATUS rc;

	rc=NtQueryDefaultLocale(FALSE, &SystemLocale);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryDefaultLocale for SystemLocale failed, rc=%x\n", rc);
		return;
	}
	printf("SystemLocale = %x %x %x\n", SystemLocale, PRIMARYLANGID(SystemLocale), SUBLANGID(SystemLocale));

	rc=NtQueryDefaultLocale(TRUE, &ThreadLocale);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryDefaultLocale for ThreadLocale failed, rc=%x\n", rc);
		return;
	}
	printf("ThreadLocale = %x\n", ThreadLocale);

	rc=NtSetDefaultLocale(TRUE, MAKELANGID(SUBLANG_DEFAULT, SUBLANG_FRENCH));
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetDefaultLocale for ThreadLocale failed, rc=%x\n", rc);
		return;
	}
	printf("Thread locale set to %x\n", MAKELANGID(SUBLANG_DEFAULT, SUBLANG_FRENCH));

	rc=NtSetDefaultLocale(TRUE, ThreadLocale);
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetDefaultLocale for ThreadLocale failed, rc=%x\n", rc);
		return;
	}
	printf("Thread locale set back to original\n");
}

main()
{
	Atoms();
	CreateDirectoryObject();
	CreateSymbolicLink();
	OpenDirectoryObject();
	QueryDirectoryObject();
	OpenSymbolicLink();
	QuerySymbolicLink();
	QueryObject();
	SetObjectInformation();
	CreateEventObject();
	OpenEventObject();
	EventManagement();
	MutantManagement();
	SemaphoreManagement();
	TimerManagement();
	TimerResolution();
	PerformanceCounter();
	TimeManagement();
	KeyManagement(); 
	ThreadManagement();
	SectionManagement();
	MemoryManagement();
	CompletionPort();
	DeleteFile();
	QueryFileAttributes();
	uuid();
	JobManagement();
	Language();
	PageFile();
	ProcessManagement();
	Locale();
	

	return 0;
}



