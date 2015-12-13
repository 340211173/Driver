#define _X86_
#include "ntddk.h"
#include <stdio.h>
#include <stdlib.h>

#include "undocnt.h"

HANDLE ghProcess;

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


void DumpBasicInformation()
{
	/* No set method for this information class */
	PROCESS_BASIC_INFORMATION ProcessBasicInfo;
	NTSTATUS rc;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessBasicInformation,
							&ProcessBasicInfo,
							sizeof(ProcessBasicInfo),
							NULL);

	if (rc==STATUS_SUCCESS) {
		printf("ProcessBasicInfo.ExitStatus                   = %x\n", ProcessBasicInfo.ExitStatus);
		printf("ProcessBasicInfo.PebBaseAddress               = %x\n", ProcessBasicInfo.PebBaseAddress);
		printf("ProcessBasicInfo.AffinityMask                 = %x\n", ProcessBasicInfo.AffinityMask);
		printf("ProcessBasicInfo.BasePriority                 = %x\n", ProcessBasicInfo.BasePriority);
		printf("ProcessBasicInfo.UniqueProcessId              = %x\n", ProcessBasicInfo.UniqueProcessId);
		printf("ProcessBasicInfo.InheritedFromUniqueProcessId = %x\n", ProcessBasicInfo.InheritedFromUniqueProcessId);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessBasicInformation', rc=%x\n", rc);
	}

}

void DumpQuotaLimitsInformation()
{
	/* Both set and get method for this information class */
	QUOTA_LIMITS QuotaLimitsInfo;
	NTSTATUS rc;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessQuotaLimits,
							&QuotaLimitsInfo,
							sizeof(QuotaLimitsInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("QuotaLimitsInfo.PagedPoolLimit        = %x\n", QuotaLimitsInfo.PagedPoolLimit);
		printf("QuotaLimitsInfo.NonPagedPoolLimit     = %x\n", QuotaLimitsInfo.NonPagedPoolLimit);
		printf("QuotaLimitsInfo.MinimumWorkingSetSize = %x\n", QuotaLimitsInfo.MinimumWorkingSetSize);
		printf("QuotaLimitsInfo.MaximumWorkingSetSize = %x\n", QuotaLimitsInfo.MaximumWorkingSetSize);
		printf("QuotaLimitsInfo.PagefileLimit         = %x\n", QuotaLimitsInfo.PagefileLimit);
		printf("QuotaLimitsInfo.TimeLimit             = %I64x\n", QuotaLimitsInfo.TimeLimit.QuadPart);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessQuotaLimits', rc=%x\n", rc);
	}

	QuotaLimitsInfo.MaximumWorkingSetSize+=4096;
	rc=NtSetInformationProcess(ghProcess,
							ProcessQuotaLimits,
							&QuotaLimitsInfo,
							sizeof(QuotaLimitsInfo));
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetInformationProcess failed with infoclass 'ProcessQuotaLimits', rc=%x\n", rc);
		return;
	}

	rc=NtQueryInformationProcess(ghProcess,
							ProcessQuotaLimits,
							&QuotaLimitsInfo,
							sizeof(QuotaLimitsInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("QuotaLimitsInfo.PagedPoolLimit        = %x\n", QuotaLimitsInfo.PagedPoolLimit);
		printf("QuotaLimitsInfo.NonPagedPoolLimit     = %x\n", QuotaLimitsInfo.NonPagedPoolLimit);
		printf("QuotaLimitsInfo.MinimumWorkingSetSize = %x\n", QuotaLimitsInfo.MinimumWorkingSetSize);
		printf("QuotaLimitsInfo.MaximumWorkingSetSize = %x\n", QuotaLimitsInfo.MaximumWorkingSetSize);
		printf("QuotaLimitsInfo.PagefileLimit         = %x\n", QuotaLimitsInfo.PagefileLimit);
		printf("QuotaLimitsInfo.TimeLimit             = %I64x\n", QuotaLimitsInfo.TimeLimit.QuadPart);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessQuotaLimits', rc=%x\n", rc);
	}
}

void DumpIoCounters()
{
	/*Returns STATUS_NOT_SUPPORTED. This system service seems to be planned initially but
	never implemented. System does maintain these IO counters on system wide basis but not
	on per process basis*/
	/* No set method for this information class */

	IO_COUNTERS IoCountersInfo;
	NTSTATUS rc;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessIoCounters,
							&IoCountersInfo,
							sizeof(IoCountersInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("IoCountersInfo.ReadOperationCount  = %x\n", IoCountersInfo.ReadOperationCount);
		printf("IoCountersInfo.WriteOperationCount = %x\n", IoCountersInfo.WriteOperationCount);
		printf("IoCountersInfo.OtherOperationCount = %x\n", IoCountersInfo.OtherOperationCount);
		printf("IoCountersInfo.ReadTransferCount   = %I64x\n", IoCountersInfo.ReadTransferCount.QuadPart);
		printf("IoCountersInfo.WriteTransferCount  = %I64x\n", IoCountersInfo.WriteTransferCount.QuadPart);
		printf("IoCountersInfo.OtherTransferCount  = %I64x\n", IoCountersInfo.OtherTransferCount.QuadPart);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessIoCounters', rc=%x\n", rc);
	}
}


void DumpVmCounters()
{
	/* No set method for this information class */
	VM_COUNTERS VmCountersInfo;
	NTSTATUS rc;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessVmCounters,
							&VmCountersInfo,
							sizeof(VmCountersInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("VmCountersInfo.PeakVirtualSize            = %x\n", VmCountersInfo.PeakVirtualSize);
		printf("VmCountersInfo.VirtualSize                = %x\n", VmCountersInfo.VirtualSize);
		printf("VmCountersInfo.PageFaultCount             = %x\n", VmCountersInfo.PageFaultCount);
		printf("VmCountersInfo.PeakWorkingSetSize         = %x\n", VmCountersInfo.PeakWorkingSetSize);
		printf("VmCountersInfo.WorkingSetSize             = %x\n", VmCountersInfo.WorkingSetSize);
		printf("VmCountersInfo.QuotaPeakPagedPoolUsage    = %x\n", VmCountersInfo.QuotaPeakPagedPoolUsage);
		printf("VmCountersInfo.QuotaPagedPoolUsage        = %x\n", VmCountersInfo.QuotaPagedPoolUsage);
		printf("VmCountersInfo.QuotaPeakNonPagedPoolUsage = %x\n", VmCountersInfo.QuotaPeakNonPagedPoolUsage);
		printf("VmCountersInfo.QuotaNonPagedPoolUsage     = %x\n", VmCountersInfo.QuotaNonPagedPoolUsage);
		printf("VmCountersInfo.PagefileUsage              = %x\n", VmCountersInfo.PagefileUsage);
		printf("VmCountersInfo.PeakPagefileUsage          = %x\n", VmCountersInfo.PeakPagefileUsage);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessVmCounters', rc=%x\n", rc);
	}
}

void DumpProcessTimes()
{
	/* No set method for this information class */
	KERNEL_USER_TIMES KernelUserTimesInfo;
	NTSTATUS rc;
	LARGE_INTEGER LocalTime;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessTimes,
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
		printf("NtQueryInformationProcess failed with infoclass 'ProcessTimes', rc=%x\n", rc);
	}
}

void DumpBasePriority()
{
	/* No get method for this information class */
	BASEPRIORITYINFO BasePriorityInfoBuffer;
	NTSTATUS rc;

	if (!EnableOrDisablePrivilege(SE_INC_BASE_PRIORITY_PRIVILEGE, FALSE)) {
		printf("Unable to enable SE_SYSTEMTIME_PRIVILEGE\n");
		return;
	}

	BasePriorityInfoBuffer.BasePriority=LOW_PRIORITY;
	rc=NtSetInformationProcess(ghProcess,
							ProcessBasePriority,
							&BasePriorityInfoBuffer,
							sizeof(BasePriorityInfoBuffer));
	EnableOrDisablePrivilege(SE_INC_BASE_PRIORITY_PRIVILEGE, TRUE);


	if (rc==STATUS_SUCCESS) {
		printf("Base Priority set to %x\n", BasePriorityInfoBuffer.BasePriority);
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessBasePriority', rc=%x\n", rc);
	}
}

void DumpRaisePriority()
{
	/* No get method for this information class */
	RAISEPRIORITYINFO RaisePriorityInfoBuffer;
	NTSTATUS rc;

	RaisePriorityInfoBuffer.RaisePriority=1;

	rc=NtSetInformationProcess(ghProcess,
							ProcessRaisePriority,
							&RaisePriorityInfoBuffer,
							sizeof(RaisePriorityInfoBuffer));

	if (rc==STATUS_SUCCESS) {
		printf("Priority raised by 1\n");
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessRaisePriority', rc=%x\n", rc);
	}
}

void DumpDebugPort()
{
	/* Returns 0 as the debug port if the application is not being debugged and returns
	0xFFFFFFFF as the debug port if the application is being debugged */

	/* The set method work, however one needs to add the code which can listen on the
	debug port and follow the debugging protocol. Otherwise, the process hangs, since
	nobody is ready to listen and follow the protocol. Hence the demostration is not
	given */
	DEBUGPORTINFO DebugPortInfoBuffer;
	NTSTATUS rc;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessDebugPort,
							&DebugPortInfoBuffer,
							sizeof(DebugPortInfoBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("DebugPortInfoBuffer.hDebugPort = %x\n", DebugPortInfoBuffer.hDebugPort);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessDebugPort', rc=%x\n", rc);
	}
}

void DumpExceptionPort()
{
	/* The following code fails with STATUS_PORT_ALREADY_SET error, because
	CSRSS calls this service to set the exception port for each process. It
	is not possible to call it more than once. Although, the follwing code 
	will work for native applications, one needs to write code to listen
	on the port and follow the hard error protocol followed by CSRSS */
	EXCEPTIONPORTINFO ExceptionPortInfoBuffer;
	NTSTATUS rc;
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING uPortName;
	HANDLE hPort;

	RtlInitUnicodeString(&uPortName, L"\\MyPort");
	InitializeObjectAttributes(&ObjectAttr, &uPortName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	rc=NtCreatePort(&hPort, &ObjectAttr, 0x0, 0x0, 0x00000);
	if (rc!=STATUS_SUCCESS) {
		printf("NtCreatePort failed, rc=%x\n", rc);
		return;
	}

	ExceptionPortInfoBuffer.hExceptionPort=hPort;
	rc=NtSetInformationProcess(ghProcess,
						ProcessExceptionPort,
						&ExceptionPortInfoBuffer,
						sizeof(ExceptionPortInfoBuffer));
	if (rc==STATUS_SUCCESS) {
		printf("ExceptionPortInfoBuffer.hExceptionPort = %x\n", ExceptionPortInfoBuffer.hExceptionPort);
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessExceptionPort', rc=%x\n", rc);
	}
}

void DumpAccessToken()
{
	/* No get method for this information class */
	/* Following code fails with an error code STATUS_TOKEN_ALREADY_IN_USE, because this token
	is already set for the process, the code will work if some other token is specified */
	PROCESS_ACCESS_TOKEN ProcessAccessTokenInfoBuffer;
	HANDLE hToken;
	NTSTATUS rc;

	rc=NtOpenProcessToken(NtCurrentProcess(),
						TOKEN_ALL_ACCESS,
						&hToken);
	if (rc!=STATUS_SUCCESS) {
		printf("NtOpenProcessToken failed, rc=%x\n", rc);
		return;
	}

	ProcessAccessTokenInfoBuffer.Token=hToken;
	ProcessAccessTokenInfoBuffer.Thread=NtCurrentThread();

	if (!EnableOrDisablePrivilege(SE_ASSIGNPRIMARYTOKEN_PRIVILEGE, FALSE)) {
		printf("Unable to get SE_ASSIGNPRIMARYTOKEN_PRIVILEGE privilege\n");
		return;
	}

	rc=NtSetInformationProcess(ghProcess,
							ProcessAccessToken,
							&ProcessAccessTokenInfoBuffer,
							sizeof(ProcessAccessTokenInfoBuffer));
	EnableOrDisablePrivilege(SE_ASSIGNPRIMARYTOKEN_PRIVILEGE, TRUE);


	if (rc==STATUS_SUCCESS) {
		printf("Process access token set\n");
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessAccessToken', rc=%x\n", rc);
	}
}

BOOLEAN IsLDTPresent()
{
	PPROCESS_LDT_INFORMATION pProcessLdtInfo;
	NTSTATUS rc;
	char Buffer[LDT_TABLE_SIZE + sizeof(PROCESS_LDT_INFORMATION) - sizeof(LDT_ENTRY)];

	pProcessLdtInfo=(PPROCESS_LDT_INFORMATION)Buffer;
	pProcessLdtInfo->Start=0;
	pProcessLdtInfo->Length=LDT_TABLE_SIZE;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessLdtInformation,
							Buffer,
							sizeof(Buffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		if (pProcessLdtInfo->Length==0) {
			return FALSE;
		} else {
			return TRUE;
		}
	} else {
		return FALSE;
	}
}


void DumpLdtInformation()
{
	NTSTATUS rc;
	PPROCESS_LDT_INFORMATION pProcessLdtInfo;
	char Buffer[LDT_TABLE_SIZE + sizeof(PROCESS_LDT_INFORMATION) - sizeof(LDT_ENTRY)];


	printf("IsLDTPresent()=%x\n", IsLDTPresent());
	if (!IsLDTPresent()) {
		printf("Setting LDT\n");
		pProcessLdtInfo=(PPROCESS_LDT_INFORMATION)Buffer;

		memset(Buffer, 0, sizeof(Buffer));
		pProcessLdtInfo->Start=0;
		pProcessLdtInfo->Length=16;

		rc=NtSetInformationProcess(ghProcess,
								ProcessLdtInformation,
								Buffer,
								sizeof(Buffer));
		if (rc!=STATUS_SUCCESS) {
			printf("NtSetInformationProcess failed, rc=%x\n", rc);
			return;
		}
	} else {
		printf("LDT already present\n");
	}

	pProcessLdtInfo=(PPROCESS_LDT_INFORMATION)Buffer;
	pProcessLdtInfo->Start=0;
	pProcessLdtInfo->Length=LDT_TABLE_SIZE;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessLdtInformation,
							Buffer,
							sizeof(Buffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		ULONG nLdtEntries;
		ULONG i;
		nLdtEntries=pProcessLdtInfo->Length/sizeof(LDT_ENTRY);
		printf("%d entries found in LDT\n", nLdtEntries);
		for (i=0; i<nLdtEntries; i++) {
			ULONG Base;
			ULONG Limit;
			ULONG Selector;

			Base=((ULONG)pProcessLdtInfo->LdtEntries[i].HighWord.Bits.BaseHi)<<24;
			Base|=((ULONG)pProcessLdtInfo->LdtEntries[i].HighWord.Bits.BaseMid)<<16;
			Base|=((ULONG)pProcessLdtInfo->LdtEntries[i].BaseLow);

			Limit=((ULONG)pProcessLdtInfo->LdtEntries[i].HighWord.Bits.LimitHi)<<16;
			Limit|=((ULONG)pProcessLdtInfo->LdtEntries[i].LimitLow);

			Selector=i<<3;
			Selector|=4;
			Selector|=((ULONG)pProcessLdtInfo->LdtEntries[i].HighWord.Bits.Dpl);

			printf("Selector %x, Base =%x, Limit=%x\n", Selector, Base, Limit);
		}
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessLdtInformation', rc=%x\n", rc);
	}
}

void DumpLdtSizeInformation()
{
	/* No get method for this information class */
	/* LDT Size can only be reduced, it can not be increased */
	PROCESS_LDT_SIZE ProcessLdtSizeInfo;
	NTSTATUS rc;

	ProcessLdtSizeInfo.Length=8;
	rc=NtSetInformationProcess(ghProcess,
							ProcessLdtSize,
							&ProcessLdtSizeInfo,
							sizeof(ProcessLdtSizeInfo));
	if (rc==STATUS_SUCCESS) {
		printf("LDT size set to %x\n", ProcessLdtSizeInfo.Length);
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessLdtSize', rc=%x\n", rc);
	}
}

void DumpDefaultHardErrorMode()
{
	HARDERRORMODEINFO HardErrorModeInfoBuffer;
	ULONG OldErrorMode;
	NTSTATUS rc;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessDefaultHardErrorMode,
							&HardErrorModeInfoBuffer,
							sizeof(HardErrorModeInfoBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("HardErrorModeInfoBuffer.HardErrorMode  = %x\n", HardErrorModeInfoBuffer.HardErrorMode);
		OldErrorMode=HardErrorModeInfoBuffer.HardErrorMode;
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessDefaultHardErrorMode', rc=%x\n", rc);
	}

	HardErrorModeInfoBuffer.HardErrorMode=SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOALIGNMENTFAULTEXCEPT|SEM_NOOPENFILEERRORBOX;
	rc=NtSetInformationProcess(ghProcess,
							ProcessDefaultHardErrorMode,
							&HardErrorModeInfoBuffer,
							sizeof(HardErrorModeInfoBuffer));

	if (rc==STATUS_SUCCESS) {
		printf("Hard error mode set to SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOALIGNMENTFAULTEXCEPT|SEM_NOOPENFILEERRORBOX\n");
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessDefaultHardErrorMode', rc=%x\n", rc);
	}

	HardErrorModeInfoBuffer.HardErrorMode=OldErrorMode;
	rc=NtSetInformationProcess(ghProcess,
							ProcessDefaultHardErrorMode,
							&HardErrorModeInfoBuffer,
							sizeof(HardErrorModeInfoBuffer));

	if (rc==STATUS_SUCCESS) {
		printf("Hard error mode reverted back to original\n");
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessDefaultHardErrorMode', rc=%x\n", rc);
	}
}

void DumpIoPortHandlers()
{
	/* No get method for this information class */
	/* The set method only works from kernel mode, the structure of the Informationbuffer
	to be passed is not known */
}

void DumpPooledUsageAndLimits()
{
	/* No set method for this information class */

	NTSTATUS rc;
	POOLED_USAGE_AND_LIMITS PooledUsageAndLimitsInfo;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessPooledUsageAndLimits,
							&PooledUsageAndLimitsInfo,
							sizeof(PooledUsageAndLimitsInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("PooledUsageAndLimitsInfo.PeakPagedPoolUsage    = %x\n", PooledUsageAndLimitsInfo.PeakPagedPoolUsage);
		printf("PooledUsageAndLimitsInfo.PagedPoolUsage        = %x\n", PooledUsageAndLimitsInfo.PagedPoolUsage);
		printf("PooledUsageAndLimitsInfo.PagedPoolLimit        = %x\n", PooledUsageAndLimitsInfo.PagedPoolLimit);
		printf("PooledUsageAndLimitsInfo.PeakNonPagedPoolUsage = %x\n", PooledUsageAndLimitsInfo.PeakNonPagedPoolUsage);
		printf("PooledUsageAndLimitsInfo.NonPagedPoolUsage     = %x\n", PooledUsageAndLimitsInfo.NonPagedPoolUsage);
		printf("PooledUsageAndLimitsInfo.NonPagedPoolLimit     = %x\n", PooledUsageAndLimitsInfo.NonPagedPoolLimit);
		printf("PooledUsageAndLimitsInfo.PeakPagefileUsage     = %x\n", PooledUsageAndLimitsInfo.PeakPagefileUsage);
		printf("PooledUsageAndLimitsInfo.PagefileUsage         = %x\n", PooledUsageAndLimitsInfo.PagefileUsage);
		printf("PooledUsageAndLimitsInfo.PagefileLimit         = %x\n", PooledUsageAndLimitsInfo.PagefileLimit);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessPooledUsageAndLimits', rc=%x\n", rc);
	}
}


void DumpWorkingsetWatchInformation()
{
	NTSTATUS rc;
	char Buffer[0x2000];
	PPROCESS_WS_WATCH_INFORMATION pWorkingsetWatchInfo;
	static char x[10000];

	/* Enable watching working set */
	rc=NtSetInformationProcess(ghProcess,
							ProcessWorkingSetWatch,
							NULL,
							0);
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetInformationProcess failed with infoclass 'ProcessWorkingSetWatch', rc=%x\n", rc);
		return;
	}

	/* memset the array so that some faults will occur */
	memset(x, 0, sizeof(x));

	pWorkingsetWatchInfo=(PPROCESS_WS_WATCH_INFORMATION)Buffer;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessWorkingSetWatch,
							Buffer,
							sizeof(Buffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		while ((pWorkingsetWatchInfo->FaultingPc!=0)&&(pWorkingsetWatchInfo->FaultingVa!=0)) {
			printf("%08x %08x\n", pWorkingsetWatchInfo->FaultingPc, pWorkingsetWatchInfo->FaultingVa);
			pWorkingsetWatchInfo++;
		}
	} else if (rc==STATUS_NO_MORE_ENTRIES){
		printf("No faults occured due to memset\n");
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessWorkingSetWatch', rc=%x\n", rc);
	}
}

void DumpBaseMemoryWithDirectPortIO()
{
	unsigned short BaseMemory;
	outp( 0x70, 0x15 );
	BaseMemory = inp( 0x71 ); 
	outp( 0x70, 0x16 );
	BaseMemory += inp(0x71) << 8;
	printf("Base memory     = %dK\n", BaseMemory);
}

void DumpUserModeIOPL()
{
	/* No get method for this information class */
	/* If you set the IOPL to 3 using this method, you can do direct port I/O
	from Ring 3. However it needs SE_TCB_PRIVILEGE for operation */
	NTSTATUS rc;
	IOPLINFO IoplInfo;

	IoplInfo.Iopl=3;

	if (!EnableOrDisablePrivilege(SE_TCB_PRIVILEGE, FALSE)) {
		printf("Unable to enable SE_TCB_PRIVILEGE\n");
		return;
	}

	rc=NtSetInformationProcess(ghProcess,
							ProcessUserModeIOPL,
							&IoplInfo,
							sizeof(IoplInfo));
	EnableOrDisablePrivilege(SE_TCB_PRIVILEGE, TRUE);

	if (rc==STATUS_SUCCESS) {
		printf("IOPL set to %d\n", IoplInfo.Iopl);
		DumpBaseMemoryWithDirectPortIO();
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessUserModeIOPL', rc=%x\n", rc);
	}
}

void DumpAllignmentFaultFixup()
{
	/* No get method for this information class */
	/* Does not seem to have any effect on X86 processors */
	NTSTATUS rc;
	ALLIGNMENTFAULTFIXUPINFO AllignmentFaultFixupInfo;

	AllignmentFaultFixupInfo.bEnableAllignmentFaultFixup=TRUE;

	rc=NtSetInformationProcess(ghProcess,
							ProcessEnableAlignmentFaultFixup,
							&AllignmentFaultFixupInfo,
							sizeof(AllignmentFaultFixupInfo));
	if (rc==STATUS_SUCCESS) {
		printf("AlignmentFaultfixup enabled\n");
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessEnableAlignmentFaultFixup', rc=%x\n", rc);
	}
}

void DumpPriorityClass()
{
	/* No get method for this information class */
	NTSTATUS rc;
	static PRIORITYCLASSINFO PriorityClassInfo;

	if (!EnableOrDisablePrivilege(SE_INC_BASE_PRIORITY_PRIVILEGE, FALSE)) {
		printf("Unable to enable SE_SYSTEMTIME_PRIVILEGE\n");
		return;
	}

	PriorityClassInfo.PriorityClass=KRNL_HIGH_PRIORITY_CLASS;
	rc=NtSetInformationProcess(ghProcess,
							ProcessPriorityClass,
							&PriorityClassInfo,
							sizeof(PriorityClassInfo));
	EnableOrDisablePrivilege(SE_INC_BASE_PRIORITY_PRIVILEGE, TRUE);

	if (rc==STATUS_SUCCESS) {
		printf("PriorityClass set to KRNL_HIGH_PRIORITY_CLASS\n");
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessPriorityClass', rc=%x\n", rc);
	}
}

void DumpX86Information()
{
	/* No set method for this information class */
	/* The get method always returns 0 in X86InfoBuffer, the actual code is not implemented */
	NTSTATUS rc;
	X86INFO X86InfoBuffer;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessWx86Information,
							&X86InfoBuffer,
							sizeof(X86InfoBuffer),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("X86InfoBuffer.x86Info=%x\n", X86InfoBuffer.x86Info);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessWx86Information', rc=%x\n", rc);
	}
}

void DumpHandleCount()
{
	/* No set method for this information class */
	NTSTATUS rc;
	HANDLECOUNTINFO HandleCountInfo;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessHandleCount,
							&HandleCountInfo,
							sizeof(HandleCountInfo),
							NULL);

	if (rc==STATUS_SUCCESS) {
		printf("HandleCountInfo.HandleCount=%x\n", HandleCountInfo.HandleCount);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessHandleCount', rc=%x\n", rc);
	}
}

void DumpAffinityMask()
{
	/* No get method for this information class, The GetProcessAffinityMask function
	uses ProcessBasicInformation infoclass to extract the affinity mask of process*/
	NTSTATUS rc;
	AFFINITYMASKINFO AffinityMaskInfo;

	AffinityMaskInfo.AffinityMask=1;
	rc=NtSetInformationProcess(ghProcess,
							ProcessAffinityMask,
							&AffinityMaskInfo,
							sizeof(AffinityMaskInfo));

	if (rc==STATUS_SUCCESS) {
		printf("AffinityMask set for the process\n");
	} else {
		printf("NtSetInformationProcess failed with infoclass 'ProcessAffinityMask', rc=%x\n", rc);
	}
}

void DumpPriorityBoost()
{
	NTSTATUS rc;
	PRIORITYBOOSTINFO PriorityBoostInfo;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessPriorityBoost,
							&PriorityBoostInfo,
							sizeof(PriorityBoostInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessPriorityBoost', rc=%x\n", rc);
		return;
	}
	printf("PriorityBoostInfo.bPriorityBoostEnabled=%x\n", PriorityBoostInfo.bPriorityBoostEnabled);

	PriorityBoostInfo.bPriorityBoostEnabled=TRUE;
	rc=NtSetInformationProcess(ghProcess,
							ProcessPriorityBoost,
							&PriorityBoostInfo,
							sizeof(PriorityBoostInfo));
	if (rc!=STATUS_SUCCESS) {
		printf("NtSetInformationProcess failed with infoclass 'ProcessPriorityBoost', rc=%x\n", rc);
		return;
	}

	rc=NtQueryInformationProcess(ghProcess,
							ProcessPriorityBoost,
							&PriorityBoostInfo,
							sizeof(PriorityBoostInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessPriorityBoost', rc=%x\n", rc);
		return;
	}
	printf("PriorityBoostInfo.bPriorityBoostEnabled=%x\n", PriorityBoostInfo.bPriorityBoostEnabled);
}

void DumpDeviceMap()
{
	NTSTATUS rc;
	PROCESS_DEVICEMAP_INFORMATION ProcessDeviceMapInfo;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessDeviceMap,
							&ProcessDeviceMapInfo,
							sizeof(ProcessDeviceMapInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		ULONG i;
		printf("ProcessDeviceMapInfo.Query.DriveMap=%08x\n", ProcessDeviceMapInfo.Query.DriveMap);
		for (i=0; i<sizeof(ProcessDeviceMapInfo.Query.DriveType); i++) {
			if (!ProcessDeviceMapInfo.Query.DriveType[i]) {
				continue;
			}
			printf("%c:\\  ", 'A'+ i);
			switch(ProcessDeviceMapInfo.Query.DriveType[i]) {
				case DRIVE_NO_ROOT_DIR:
					printf("DRIVE_NO_ROOT_DIR\n");
					break;
				case DRIVE_REMOVABLE:
					printf("DRIVE_REMOVABLE\n");
					break;
				case DRIVE_FIXED:
					printf("DRIVE_FIXED\n");
					break;
				case DRIVE_REMOTE:
					printf("DRIVE_REMOTE\n");
					break;
				case DRIVE_CDROM:
					printf("DRIVE_CDROM\n");
					break;
				case DRIVE_RAMDISK:
					printf("DRIVE_RAMDISK\n");
					break;
			}
		}
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessDeviceMap', rc=%x\n", rc);
	}
}

void DumpSessionId()
{
	NTSTATUS rc;
	PROCESS_SESSION_INFORMATION ProcessSessionInfo;

	rc=NtQueryInformationProcess(ghProcess,
							ProcessSessionInformation,
							&ProcessSessionInfo,
							sizeof(ProcessSessionInfo),
							NULL);
	if (rc==STATUS_SUCCESS) {
		printf("ProcessSessionInfo.SessionId=%x\n", ProcessSessionInfo.SessionId);
	} else {
		printf("NtQueryInformationProcess failed with infoclass 'ProcessSessionInformation', rc=%x\n", rc);
	}
}


void func()
{
	ULONG i;
	NTSTATUS rc;
	char Buffer[10000];

	printf("%d\n", ProcessWow64Information);
	for (i=0; i<40; i++) {
		rc=NtQueryInformationProcess(NtCurrentProcess(),
								i,
								Buffer,
								sizeof(Buffer),
								NULL);
		if (rc!=STATUS_INVALID_INFO_CLASS) {
			printf("%d ", i);
		}
	}
	printf("\n");


	for (i=0; i<40; i++) {
		rc=NtSetInformationProcess(NtCurrentProcess(),
								i,
								Buffer,
								sizeof(Buffer));
		if (rc!=STATUS_INVALID_INFO_CLASS) {
			printf("%d ", i);
		}
	}
	printf("\n");

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


		ClientId.UniqueProcess=(HANDLE)atoi(argv[1]);
		ClientId.UniqueThread=(HANDLE)0;

		rc=NtOpenProcess(&ghProcess,
							PROCESS_ALL_ACCESS,
							&ObjAttr,
							&ClientId);
		if (rc!=STATUS_SUCCESS) {
			printf("NtOpenProcess failed, rc=%x\n", rc);
			return 0;
		}
	} else {
		ghProcess=NtCurrentProcess();
	}


	DumpBasicInformation();
	DumpQuotaLimitsInformation();
	DumpIoCounters();
	DumpVmCounters();
	DumpProcessTimes();
	DumpBasePriority();
	DumpRaisePriority();
	DumpDebugPort();
	DumpExceptionPort();
	DumpAccessToken();
	DumpLdtInformation();
	DumpLdtSizeInformation();
	DumpDefaultHardErrorMode();
	DumpIoPortHandlers();
	DumpPooledUsageAndLimits();
	DumpWorkingsetWatchInformation();
	DumpUserModeIOPL();
	DumpAllignmentFaultFixup();
	DumpPriorityClass();
	DumpX86Information();
	DumpHandleCount();
	DumpAffinityMask();
	DumpPriorityBoost();
	DumpDeviceMap();
	DumpSessionId();
}

