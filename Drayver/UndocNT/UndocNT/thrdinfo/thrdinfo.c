#define _X86_
#include "ntddk.h"
#include <stdio.h>
#include <stdlib.h>


#include "undocnt.h"


HANDLE ghThread;

void InfoThreadBasicInformation()
{
	/* No set method for this information class */
	NTSTATUS rc;
	THREAD_BASIC_INFORMATION ThreadBasicInfoBuffer;

	rc=NtQueryInformationThread(ghThread,
							ThreadBasicInformation,
							&ThreadBasicInfoBuffer,
							sizeof(ThreadBasicInfoBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("ThreadBasicInfoBuffer.ExitStatus          = %x\n", ThreadBasicInfoBuffer.ExitStatus);
		printf("ThreadBasicInfoBuffer.TebBaseAddress      = %x\n", ThreadBasicInfoBuffer.TebBaseAddress);
		printf("ThreadBasicInfoBuffer.UniqueProcessId     = %x\n", ThreadBasicInfoBuffer.UniqueProcessId);
		printf("ThreadBasicInfoBuffer.UniqueThreadId      = %x\n", ThreadBasicInfoBuffer.UniqueThreadId);
		printf("ThreadBasicInfoBuffer.AffinityMask        = %x\n", ThreadBasicInfoBuffer.AffinityMask);
		printf("ThreadBasicInfoBuffer.BasePriority        = %x\n", ThreadBasicInfoBuffer.BasePriority);
		printf("ThreadBasicInfoBuffer.DiffProcessPriority = %x\n", ThreadBasicInfoBuffer.DiffProcessPriority);
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadBasicInformation', rc=%x\n", rc);
	}
	printf("\n");
}

void InfoThreadTimes()
{
	/* No set method for this information class */
	NTSTATUS rc;
	KERNEL_USER_TIMES KernelUserTimesInfo;
	LARGE_INTEGER LocalTime;

	rc=NtQueryInformationThread(ghThread,
							ThreadTimes,
							&KernelUserTimesInfo,
							sizeof(KernelUserTimesInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		TIME_FIELDS TimeFields;

		printf("KernelUserTimesInfo.CreateTime = %I64x ", KernelUserTimesInfo.CreateTime.QuadPart);
		RtlSystemTimeToLocalTime(&KernelUserTimesInfo.CreateTime, &LocalTime);
		RtlTimeToTimeFields(&LocalTime, &TimeFields);
		printf("%02d-%02d-%04d, %02d-%02d-%02d\n", TimeFields.Day, TimeFields.Month, TimeFields.Year,
			TimeFields.Hour, TimeFields.Minute, TimeFields.Second);
		printf("KernelUserTimesInfo.ExitTime   = %I64x\n", KernelUserTimesInfo.ExitTime.QuadPart);
		printf("KernelUserTimesInfo.KernelTime = %I64x\n", KernelUserTimesInfo.KernelTime.QuadPart);
		printf("KernelUserTimesInfo.UserTime   = %I64x\n", KernelUserTimesInfo.UserTime.QuadPart);
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadTimes', rc=%x\n", rc);
	}
	printf("\n");
}

void InfoThreadPriority()
{
	/* No get method for this information class */
	THREAD_PRIORITY ThreadPriorityBuffer;
	NTSTATUS rc;

	ThreadPriorityBuffer.Priority=LOW_REALTIME_PRIORITY;

	rc=NtSetInformationThread(ghThread,
							ThreadPriority,
							&ThreadPriorityBuffer,
							sizeof(ThreadPriorityBuffer));
	if (rc==STATUS_SUCCESS) {
		printf("Thread priority set to LOW_REALTIME_PRIORITY\n");
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadPriority', rc=%x\n", rc);
	}
}

void InfoThreadBasePriority()
{
	/* No get method for this information class */
	THREAD_BASE_PRIORITY ThreadBasePriorityBuffer;
	NTSTATUS rc;

	ThreadBasePriorityBuffer.IncBasePriority=1;

	rc=NtSetInformationThread(ghThread,
							ThreadBasePriority,
							&ThreadBasePriorityBuffer,
							sizeof(ThreadBasePriorityBuffer));
	if (rc==STATUS_SUCCESS) {
		printf("Thread base priority incremented by 1\n");
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadBasePriority', rc=%x\n", rc);
	}
}

void InfoThreadAffinityMask()
{
	/* No get method for this information class*/
	NTSTATUS rc;
	THREADAFFINITYMASKINFO ThreadAffinityMaskInfo;

	ThreadAffinityMaskInfo.ThreadAffinityMask=0x01;
	rc=NtSetInformationThread(ghThread,
							ThreadAffinityMask,
							&ThreadAffinityMaskInfo,
							sizeof(ThreadAffinityMaskInfo));

	if (rc==STATUS_SUCCESS) {
		printf("AffinityMask set for the Thread\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadAffinityMask', rc=%x\n", rc);
	}
}

void InfoThreadImpersonationToken()
{
	/* No get method for this information class*/

	NTSTATUS rc;
	HANDLE hToken, hImpersonationToken;

	rc=NtOpenProcessToken(NtCurrentProcess(),
							MAXIMUM_ALLOWED,
							&hToken);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to open process token, rc=%x\n", rc);
		return;
	}

	rc=NtDuplicateToken(hToken,
						MAXIMUM_ALLOWED,
						NULL,
						FALSE,
						TokenImpersonation,
						&hImpersonationToken);

	if (rc!=STATUS_SUCCESS) {
		printf("Unable to duplicate process token, rc=%x\n", rc);
		return;
	}
	rc=NtSetInformationThread(ghThread,
							ThreadImpersonationToken,
							&hImpersonationToken,
							sizeof(hImpersonationToken));
	if (rc==STATUS_SUCCESS) {
		printf("ImpersonationToken set for the Thread\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadImpersonationToken', rc=%x\n", rc);
		return;
	}

	NtClose(hImpersonationToken);


	/* Now reverting back to original token */
	hImpersonationToken=NULL;
	rc=NtSetInformationThread(ghThread,
							ThreadImpersonationToken,
							&hImpersonationToken,
							sizeof(hImpersonationToken));
	if (rc==STATUS_SUCCESS) {
		printf("ImpersonationToken reverted to self\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadImpersonationToken', rc=%x\n", rc);
		return;
	}
}

void InfoThreadDescriptorTableEntry()
{
	/* No set method for this information class*/
	DESCRIPTOR_TABLE_ENTRY DescriptorTableEntryBuffer;
	int rc;

	memset(&DescriptorTableEntryBuffer, 0, sizeof(DescriptorTableEntryBuffer));
	DescriptorTableEntryBuffer.Selector=0x8;
	rc=NtQueryInformationThread(ghThread,
							ThreadDescriptorTableEntry,
							&DescriptorTableEntryBuffer,
							sizeof(DescriptorTableEntryBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		ULONG Base, Limit;

		Base=((ULONG)DescriptorTableEntryBuffer.Descriptor.HighWord.Bits.BaseHi)<<24;
		Base|=((ULONG)DescriptorTableEntryBuffer.Descriptor.HighWord.Bits.BaseMid)<<16;
		Base|=((ULONG)DescriptorTableEntryBuffer.Descriptor.BaseLow);

		Limit=((ULONG)DescriptorTableEntryBuffer.Descriptor.HighWord.Bits.LimitHi)<<16;
		Limit|=((ULONG)DescriptorTableEntryBuffer.Descriptor.LimitLow);


		printf("DescriptorTableEntryBuffer.Selector         = %x\n", DescriptorTableEntryBuffer.Selector);
		printf("DescriptorTableEntryBuffer.Descriptor.Base  = %x\n", Base);
		printf("DescriptorTableEntryBuffer.Descriptor.Limit = %x\n", Limit);
		 
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadDescriptorTableEntry', rc=%x\n", rc);
	}
}


void InfoAllignmentFaultFixup()
{
	/* No get method for this information class */
	/* Does not seem to have any effect on X86 processors */
	NTSTATUS rc;
	ALLIGNMENTFAULTFIXUPINFO AllignmentFaultFixupInfo;

	AllignmentFaultFixupInfo.bEnableAllignmentFaultFixup=TRUE;

	rc=NtSetInformationThread(ghThread,
							ThreadEnableAlignmentFaultFixup,
							&AllignmentFaultFixupInfo,
							sizeof(AllignmentFaultFixupInfo));
	if (rc==STATUS_SUCCESS) {
		printf("AlignmentFaultfixup enabled\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadEnableAlignmentFaultFixup', rc=%x\n", rc);
	}
}


void InfoThreadEventPair()
{
	/* No get method for this information class */
	HANDLE hEventPair=NULL;
	NTSTATUS rc;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING EventPairName;
	#define EVENTPAIRNAME	L"\\MyEventPair"
	EVENTPAIRINFO EventPairInfoBuffer;

	RtlInitUnicodeString(&EventPairName, EVENTPAIRNAME);
	InitializeObjectAttributes(&ObjectAttr, 
							&EventPairName, 
							OBJ_CASE_INSENSITIVE, 
							NULL, 
							NULL);

	rc=NtCreateEventPair(&hEventPair, 
						STANDARD_RIGHTS_ALL, 
						&ObjectAttr);
	if (rc!=STATUS_SUCCESS) {
		printf("Unable to create event pair, rc=%x\n", rc);
		return;
	}

	EventPairInfoBuffer.hEventPair=hEventPair;
	rc=NtSetInformationThread(ghThread,
							ThreadEventPair,
							&EventPairInfoBuffer,
							sizeof(EventPairInfoBuffer));

	if (rc==STATUS_SUCCESS) {
		printf("EventPair for the thread set\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadEventPair', rc=%x\n", rc);
	}
}

void NewWin32StartAddress()
{
}

void InfoWin32StartAddress()
{
	WIN32_START_ADDRESS Win32StartAddressBuffer;
	NTSTATUS rc;
	
	rc=NtQueryInformationThread(ghThread,
							ThreadQuerySetWin32StartAddress,
							&Win32StartAddressBuffer,
							sizeof(Win32StartAddressBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("Win32StartAddressBuffer.Win32StartAddress = %x\n", Win32StartAddressBuffer.Win32StartAddress);
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadQuerySetWin32StartAddress', rc=%x\n", rc);
		return;
	}

	printf("Address of NewWin32StartAddress = %x\n", NewWin32StartAddress);
	Win32StartAddressBuffer.Win32StartAddress=(PVOID)NewWin32StartAddress;
	rc=NtSetInformationThread(ghThread,
							ThreadQuerySetWin32StartAddress,
							&Win32StartAddressBuffer,
							sizeof(Win32StartAddressBuffer));
	if (rc==STATUS_SUCCESS) {
		printf("Win32Startaddress set to NewWin32StartAddress\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadQuerySetWin32StartAddress', rc=%x\n", rc);
		return;
	}

	rc=NtQueryInformationThread(ghThread,
							ThreadQuerySetWin32StartAddress,
							&Win32StartAddressBuffer,
							sizeof(Win32StartAddressBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("Win32StartAddressBuffer.Win32StartAddress = %x\n", Win32StartAddressBuffer.Win32StartAddress);
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadQuerySetWin32StartAddress', rc=%x\n", rc);
		return;
	}
}

void InfoZeroTlsCell()
{
	/* No get method for this information class */
	/*TLS values can be set only for the current thread*/

	/* The TlsFree API uses this system service. The TLS state is maintained in both 
	kernel TEB and user TEB. Hence calling this system service directly does not mean 
	that the TLS index now is available for reuse*/
	ZERO_TLSCELL ZeroTlsCellInfoInfoBuffer;
	NTSTATUS rc;

	ZeroTlsCellInfoInfoBuffer.TlsIndex=0;
	rc=NtSetInformationThread(ghThread,
							ThreadZeroTlsCell,
							&ZeroTlsCellInfoInfoBuffer,
							sizeof(ZeroTlsCellInfoInfoBuffer));
	
	if (rc==STATUS_SUCCESS) {
		printf("TlsIndex %d zeroed\n", ZeroTlsCellInfoInfoBuffer.TlsIndex);
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadZeroTlsCell', rc=%x\n", rc);
		return;
	}
}

void InfoThreadPerformanceCount()
{
	/* No set method for this information class */
	PERFORMANCE_COUNTER_INFO PerformaceCounterInfoBuffer;
	NTSTATUS rc;

	rc=NtQueryInformationThread(ghThread,
							ThreadPerformanceCount,
							&PerformaceCounterInfoBuffer,
							sizeof(PerformaceCounterInfoBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("PerformaceCounterInfoBuffer.Count1 = %x\n", PerformaceCounterInfoBuffer.Count1);
		printf("PerformaceCounterInfoBuffer.Count2 = %x\n", PerformaceCounterInfoBuffer.Count2);
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadPerformanceCount', rc=%x\n", rc);
		return;
	}
}

void InfoThreadAmILastThread()
{
	/* No set method for this information class */
	/* The system service ignores the ghThread passed, instead works with
	calling thread only */
	/* The system service is used by ExitThread function to check if the
	last thread is calling ExitThread. If so, it calls ExitProcess function
	to exit the process as well */
	AMI_LAST_THREADINFO	AmILastThreadInfoBuffer;
	NTSTATUS rc;

	rc=NtQueryInformationThread(ghThread,
							ThreadAmILastThread,
							&AmILastThreadInfoBuffer,
							sizeof(AmILastThreadInfoBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("AmILastThreadInfoBuffer.bAmILastThread = %s\n", AmILastThreadInfoBuffer.bAmILastThread?"TRUE":"FALSE");
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadAmILastThread', rc=%x\n", rc);
		return;
	}
}


void InfoIdealProcessor()
{
	/* No get method for this information class */
	/*Max value allowed is 0x20, No validation is performed about whether
	such processor is available on the system*/
	IDEAL_PROCESSORINFO IdealProcessorInfo;
	NTSTATUS rc;

	IdealProcessorInfo.IdealProcessor=1;
	rc=NtSetInformationThread(ghThread,
							ThreadIdealProcessor,
							&IdealProcessorInfo,
							sizeof(IdealProcessorInfo));
	if (rc==STATUS_SUCCESS) {
		printf("IdealProcessorInfo.IdealProcessor = %d\n", IdealProcessorInfo.IdealProcessor);
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadIdealProcessor', rc=%x\n", rc);
		return;
	}
}

void InfoPriorityBoost()
{
	NTSTATUS rc;
	PRIORITYBOOSTINFO PriorityBoostInfo;

	rc=NtQueryInformationThread(ghThread,
							ThreadPriorityBoost,
							&PriorityBoostInfo,
							sizeof(PriorityBoostInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryInformationThread failed with infoclass 'ThreadPriorityBoost', rc=%x\n", rc);
		return;
	}
	printf("PriorityBoostInfo.bPriorityBoostEnabled=%x\n", PriorityBoostInfo.bPriorityBoostEnabled);

	PriorityBoostInfo.bPriorityBoostEnabled=TRUE;
	rc=NtSetInformationThread(ghThread,
							ThreadPriorityBoost,
							&PriorityBoostInfo,
							sizeof(PriorityBoostInfo));
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetInformationThread failed with infoclass 'ThreadPriorityBoost', rc=%x\n", rc);
		return;
	}

	rc=NtQueryInformationThread(ghThread,
							ThreadPriorityBoost,
							&PriorityBoostInfo,
							sizeof(PriorityBoostInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryInformationThread failed with infoclass 'ThreadPriorityBoost', rc=%x\n", rc);
		return;
	}
	printf("PriorityBoostInfo.bPriorityBoostEnabled=%x\n", PriorityBoostInfo.bPriorityBoostEnabled);
}

void InfoTlsArrayAddress()
{
	/* No get method for this information class */
	TLS_ARRAYINFO TlsArrayInfo;
	static ULONG TlsArray[64];
	NTSTATUS rc;

	TlsArrayInfo.pTlsArray=TlsArray;
	rc=NtSetInformationThread(ghThread,
							ThreadSetTlsArrayAddress,
							&TlsArrayInfo,
							sizeof(TlsArrayInfo));
	if (rc==STATUS_SUCCESS) {
		printf("TLS Array set\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadSetTlsArrayAddress', rc=%x\n", rc);
		return;
	}
}

void InfoIsIOPending()
{
	/* No set method for this information class */
	NTSTATUS rc;
	IS_IO_PENDINGINFO IsIoPendingInfoBuffer;

	rc=NtQueryInformationThread(ghThread,
							ThreadIsIoPending,
							&IsIoPendingInfoBuffer,
							sizeof(IsIoPendingInfoBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("IsIoPendingInfoBuffer.bIsIOPending = %s\n", IsIoPendingInfoBuffer.bIsIOPending?"TRUE":"FALSE");
	} else {
		printf("NtQueryInformationThread failed with infoclass 'ThreadIsIoPending', rc=%x\n", rc);
		return;
	}
}

void InfoHideFromDebugger()
{
	/* No get method for this information class */
	/* TODO: service fails with the error code of STATUS_INFO_LENGTH_MISMATCH */
	HIDE_FROM_DEBUGGERINFO HideFromDebuggerInfo;
	NTSTATUS rc;

	HideFromDebuggerInfo.bHideFromDebugger=TRUE;
	rc=NtSetInformationThread(ghThread,
							ThreadHideFromDebugger,
							&HideFromDebuggerInfo,
							sizeof(HideFromDebuggerInfo));
	if (rc==STATUS_SUCCESS) {
		printf("Thread hidden from Debugger\n");
	} else {
		printf("NtSetInformationThread failed with infoclass 'ThreadHideFromDebugger', rc=%x\n", rc);
		return;
	}
}


int main(int argc, char **argv)
{
	if (argc==2) {
		NTSTATUS rc;
		CLIENT_ID ClientId;
		OBJECT_ATTRIBUTES ObjAttr;

		InitializeObjectAttributes(&ObjAttr,
									NULL,
									0,
									NULL,
									NULL);


		ClientId.UniqueProcess=(HANDLE)0;
		ClientId.UniqueThread=(HANDLE)atoi(argv[1]);

		rc=NtOpenThread(&ghThread,
							THREAD_ALL_ACCESS,
							&ObjAttr,
							&ClientId);
		if (rc!=STATUS_SUCCESS) {
			printf("NtOpenThread failed, rc=%x\n", rc);
			return 0;
		}
	} else {
		ghThread=NtCurrentThread();
	}

	printf("ghThread=%x\n", ghThread);


	InfoThreadBasicInformation();
	InfoThreadTimes();
	InfoThreadPriority();
	InfoThreadBasePriority();
	InfoThreadAffinityMask();
	InfoThreadImpersonationToken();
	InfoThreadDescriptorTableEntry();
	InfoAllignmentFaultFixup();
	InfoThreadEventPair();
	InfoWin32StartAddress();
	InfoZeroTlsCell();
	InfoThreadPerformanceCount();
	InfoThreadAmILastThread();
	InfoIdealProcessor();
	InfoPriorityBoost();
	InfoTlsArrayAddress();
	InfoIsIOPending();
	InfoHideFromDebugger();
}


