#define _X86_
#include <ntddk.h>
#include <stdio.h>

#include "undocnt.h"


BOOLEAN EnableOrDisablePrivilege(ULONG PrivilegeId, BOOLEAN bDisable)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES PrivilegeSet;
	NTSTATUS rc;

	rc=NtOpenProcessToken(NtCurrentProcess(),
						TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
						&hToken);

	if (rc!=STATUS_SUCCESS) {
		DbgPrint("NtOpenProcessToken failed, rc=%x\n", rc);
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
		DbgPrint("NtAdjustPrivilegesToken failed, rc=%x\n", rc);
		return FALSE;
	}
	return TRUE;
}


void InfoBasicSystemInfo()
{
	BASICSYSTEMINFO BasicSystemInfo;
	NTSTATUS rc;

	memset(&BasicSystemInfo, 0, sizeof(BasicSystemInfo));
	rc=NtQuerySystemInformation(SystemBasicInfo,
							&BasicSystemInfo,
							sizeof(BasicSystemInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemBasicInfo',  rc=%x\n", rc);
		return;
	}

	printf("AlwaysZero              = %d %x\n", BasicSystemInfo.AlwaysZero, BasicSystemInfo.AlwaysZero);
	printf("KeMaximumIncrement      = %d %x\n", BasicSystemInfo.KeMaximumIncrement, BasicSystemInfo.KeMaximumIncrement);
	printf("MmPageSize              = %d %x\n", BasicSystemInfo.MmPageSize, BasicSystemInfo.MmPageSize);
	printf("MmNumberOfPhysicalPages = %d %x\n", BasicSystemInfo.MmNumberOfPhysicalPages, BasicSystemInfo.MmNumberOfPhysicalPages);
	printf("MmLowestPhysicalPage    = %d %x\n", BasicSystemInfo.MmLowestPhysicalPage, BasicSystemInfo.MmLowestPhysicalPage);
	printf("MmHighestPhysicalPage   = %d %x\n", BasicSystemInfo.MmHighestPhysicalPage, BasicSystemInfo.MmHighestPhysicalPage);
	printf("MmLowestUserAddress     = %d %x\n", BasicSystemInfo.MmLowestUserAddress, BasicSystemInfo.MmLowestUserAddress);
	printf("MmLowestUserAddress1    = %d %x\n", BasicSystemInfo.MmLowestUserAddress1, BasicSystemInfo.MmLowestUserAddress1);
	printf("MmHighestUserAddress    = %d %x\n", BasicSystemInfo.MmHighestUserAddress, BasicSystemInfo.MmHighestUserAddress);
	printf("KeActiveProcessors      = %d %x\n", BasicSystemInfo.KeActiveProcessors, BasicSystemInfo.KeActiveProcessors);
	printf("KeNumberProcessors      = %d %x\n", BasicSystemInfo.KeNumberProcessors, BasicSystemInfo.KeNumberProcessors);
	printf("\n");
	return;
}

void InfoProcessorSystemInfo()
{
	PROCESSORSYSTEMINFO ProcessorSystemInfo;
	NTSTATUS rc;

	memset(&ProcessorSystemInfo, 0, sizeof(ProcessorSystemInfo));

	rc=NtQuerySystemInformation(SystemProcessorInfo,
							&ProcessorSystemInfo,
							sizeof(ProcessorSystemInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemProcessorInfo',  rc=%x\n", rc);
		return;
	}

	printf("KeProcessorArchitecture = %d %x\n", ProcessorSystemInfo.KeProcessorArchitecture, ProcessorSystemInfo.KeProcessorArchitecture);
	printf("KeProcessorLevel        = %d %x\n", ProcessorSystemInfo.KeProcessorLevel, ProcessorSystemInfo.KeProcessorLevel);
	printf("KeProcessorRevision     = %d %x\n", ProcessorSystemInfo.KeProcessorRevision, ProcessorSystemInfo.KeProcessorRevision);
	printf("AlwaysZero              = %d %x\n", ProcessorSystemInfo.AlwaysZero, ProcessorSystemInfo.AlwaysZero);
	printf("KeFeatureBits           = %d %x\n", ProcessorSystemInfo.KeFeatureBits, ProcessorSystemInfo.KeFeatureBits);

	printf("\n");
	return;
}

void InfoPerformanceSystemInfo()
{
	PERFORMANCEINFO PerformanceSystemInfo;
	NTSTATUS rc;

	memset(&PerformanceSystemInfo, 0, sizeof(PerformanceSystemInfo));

	rc=NtQuerySystemInformation(SystemPerformanceInfo,
							&PerformanceSystemInfo,
							sizeof(PerformanceSystemInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemPerformanceInfo',  rc=%x\n", rc);
		return;
	}
	printf("PerformanceSystemInfo.TotalProcessorTime=%I64x\n", PerformanceSystemInfo.TotalProcessorTime);
	printf("PerformanceSystemInfo.IoReadTransferCount=%I64x\n", PerformanceSystemInfo.IoReadTransferCount);
	printf("PerformanceSystemInfo.IoWriteTransferCount=%I64x\n", PerformanceSystemInfo.IoWriteTransferCount);
	printf("PerformanceSystemInfo.IoOtherTransferCount=%I64x\n", PerformanceSystemInfo.IoOtherTransferCount);
	printf("PerformanceSystemInfo.IoReadOperationCount=%x\n", PerformanceSystemInfo.IoReadOperationCount);
	printf("PerformanceSystemInfo.IoWriteOperationCount=%x\n", PerformanceSystemInfo.IoWriteOperationCount);
	printf("PerformanceSystemInfo.IoOtherOperationCount=%x\n", PerformanceSystemInfo.IoOtherOperationCount);
	printf("PerformanceSystemInfo.MmAvailablePages=%x\n", PerformanceSystemInfo.MmAvailablePages);
	printf("PerformanceSystemInfo.MmTotalCommitedPages=%x\n", PerformanceSystemInfo.MmTotalCommitedPages);
	printf("PerformanceSystemInfo.MmTotalCommitLimit=%x\n", PerformanceSystemInfo.MmTotalCommitLimit);
	printf("PerformanceSystemInfo.MmPeakLimit=%x\n", PerformanceSystemInfo.MmPeakLimit);
	printf("PerformanceSystemInfo.PageFaults=%x\n", PerformanceSystemInfo.PageFaults);
	printf("PerformanceSystemInfo.WriteCopies=%x\n", PerformanceSystemInfo.WriteCopies);
	printf("PerformanceSystemInfo.TransitionFaults=%x\n", PerformanceSystemInfo.TransitionFaults);
	printf("PerformanceSystemInfo.Unknown1=%x\n", PerformanceSystemInfo.Unknown1);
	printf("PerformanceSystemInfo.DemandZeroFaults=%x\n", PerformanceSystemInfo.DemandZeroFaults);
	printf("PerformanceSystemInfo.PagesInput=%x\n", PerformanceSystemInfo.PagesInput);
	printf("PerformanceSystemInfo.PagesRead=%x\n", PerformanceSystemInfo.PagesRead);
	printf("PerformanceSystemInfo.Unknown2=%x\n", PerformanceSystemInfo.Unknown2);
	printf("PerformanceSystemInfo.Unknown3=%x\n", PerformanceSystemInfo.Unknown3);
	printf("PerformanceSystemInfo.PagesOutput=%x\n", PerformanceSystemInfo.PagesOutput);
	printf("PerformanceSystemInfo.PageWrites=%x\n", PerformanceSystemInfo.PageWrites);
	printf("PerformanceSystemInfo.Unknown4=%x\n", PerformanceSystemInfo.Unknown4);
	printf("PerformanceSystemInfo.Unknown5=%x\n", PerformanceSystemInfo.Unknown5);
	printf("PerformanceSystemInfo.PoolPagedBytes=%x\n", PerformanceSystemInfo.PoolPagedBytes);
	printf("PerformanceSystemInfo.PoolNonPagedBytes=%x\n", PerformanceSystemInfo.PoolNonPagedBytes);
	printf("PerformanceSystemInfo.Unknown6=%x\n", PerformanceSystemInfo.Unknown6);
	printf("PerformanceSystemInfo.Unknown7=%x\n", PerformanceSystemInfo.Unknown7);
	printf("PerformanceSystemInfo.Unknown8=%x\n", PerformanceSystemInfo.Unknown8);
	printf("PerformanceSystemInfo.Unknown9=%x\n", PerformanceSystemInfo.Unknown9);
	printf("PerformanceSystemInfo.MmTotalSystemFreePtes=%x\n", PerformanceSystemInfo.MmTotalSystemFreePtes);
	printf("PerformanceSystemInfo.MmSystemCodepage=%x\n", PerformanceSystemInfo.MmSystemCodepage);
	printf("PerformanceSystemInfo.MmTotalSystemDriverPages=%x\n", PerformanceSystemInfo.MmTotalSystemDriverPages);
	printf("PerformanceSystemInfo.MmTotalSystemCodePages=%x\n", PerformanceSystemInfo.MmTotalSystemCodePages);
	printf("PerformanceSystemInfo.Unknown10=%x\n", PerformanceSystemInfo.Unknown10);
	printf("PerformanceSystemInfo.Unknown11=%x\n", PerformanceSystemInfo.Unknown11);
	printf("PerformanceSystemInfo.Unknown12=%x\n", PerformanceSystemInfo.Unknown12);
	printf("PerformanceSystemInfo.MmSystemCachePage=%x\n", PerformanceSystemInfo.MmSystemCachePage);
	printf("PerformanceSystemInfo.MmPagedPoolPage=%x\n", PerformanceSystemInfo.MmPagedPoolPage);
	printf("PerformanceSystemInfo.MmSystemDriverPage=%x\n", PerformanceSystemInfo.MmSystemDriverPage);
	printf("PerformanceSystemInfo.CcFastReadNoWait=%x\n", PerformanceSystemInfo.CcFastReadNoWait);
	printf("PerformanceSystemInfo.CcFastReadWait=%x\n", PerformanceSystemInfo.CcFastReadWait);
	printf("PerformanceSystemInfo.CcFastReadResourceMiss=%x\n", PerformanceSystemInfo.CcFastReadResourceMiss);
	printf("PerformanceSystemInfo.CcFastReadNotPossible=%x\n", PerformanceSystemInfo.CcFastReadNotPossible);
	printf("PerformanceSystemInfo.CcFastMdlReadNoWait=%x\n", PerformanceSystemInfo.CcFastMdlReadNoWait);
	printf("PerformanceSystemInfo.CcFastMdlReadWait=%x\n", PerformanceSystemInfo.CcFastMdlReadWait);
	printf("PerformanceSystemInfo.CcFastMdlReadResourceMiss=%x\n", PerformanceSystemInfo.CcFastMdlReadResourceMiss);
	printf("PerformanceSystemInfo.CcFastMdlReadNotPossible=%x\n", PerformanceSystemInfo.CcFastMdlReadNotPossible);
	printf("PerformanceSystemInfo.CcMapDataNoWait=%x\n", PerformanceSystemInfo.CcMapDataNoWait);
	printf("PerformanceSystemInfo.CcMapDataWait=%x\n", PerformanceSystemInfo.CcMapDataWait);
	printf("PerformanceSystemInfo.CcMapDataNoWaitMiss=%x\n", PerformanceSystemInfo.CcMapDataNoWaitMiss);
	printf("PerformanceSystemInfo.CcMapDataWaitMiss=%x\n", PerformanceSystemInfo.CcMapDataWaitMiss);
	printf("PerformanceSystemInfo.CcPinMappedDataCount=%x\n", PerformanceSystemInfo.CcPinMappedDataCount);
	printf("PerformanceSystemInfo.CcPinReadNoWait=%x\n", PerformanceSystemInfo.CcPinReadNoWait);
	printf("PerformanceSystemInfo.CcPinReadWait=%x\n", PerformanceSystemInfo.CcPinReadWait);
	printf("PerformanceSystemInfo.CcPinReadNoWaitMiss=%x\n", PerformanceSystemInfo.CcPinReadNoWaitMiss);
	printf("PerformanceSystemInfo.CcPinReadWaitMiss=%x\n", PerformanceSystemInfo.CcPinReadWaitMiss);
	printf("PerformanceSystemInfo.CcCopyReadNoWait=%x\n", PerformanceSystemInfo.CcCopyReadNoWait);
	printf("PerformanceSystemInfo.CcCopyReadWait=%x\n", PerformanceSystemInfo.CcCopyReadWait);
	printf("PerformanceSystemInfo.CcCopyReadNoWaitMiss=%x\n", PerformanceSystemInfo.CcCopyReadNoWaitMiss);
	printf("PerformanceSystemInfo.CcCopyReadWaitMiss=%x\n", PerformanceSystemInfo.CcCopyReadWaitMiss);
	printf("PerformanceSystemInfo.CcMdlReadNoWait=%x\n", PerformanceSystemInfo.CcMdlReadNoWait);
	printf("PerformanceSystemInfo.CcMdlReadWait=%x\n", PerformanceSystemInfo.CcMdlReadWait);
	printf("PerformanceSystemInfo.CcMdlReadNoWaitMiss=%x\n", PerformanceSystemInfo.CcMdlReadNoWaitMiss);
	printf("PerformanceSystemInfo.CcMdlReadWaitMiss=%x\n", PerformanceSystemInfo.CcMdlReadWaitMiss);
	printf("PerformanceSystemInfo.CcReadaheadIos=%x\n", PerformanceSystemInfo.CcReadaheadIos);
	printf("PerformanceSystemInfo.CcLazyWriteIos=%x\n", PerformanceSystemInfo.CcLazyWriteIos);
	printf("PerformanceSystemInfo.CcLazyWritePages=%x\n", PerformanceSystemInfo.CcLazyWritePages);
	printf("PerformanceSystemInfo.CcDataFlushes=%x\n", PerformanceSystemInfo.CcDataFlushes);
	printf("PerformanceSystemInfo.CcDataPages=%x\n", PerformanceSystemInfo.CcDataPages);
	printf("PerformanceSystemInfo.ContextSwitches=%x\n", PerformanceSystemInfo.ContextSwitches);
	printf("PerformanceSystemInfo.Unknown13=%x\n", PerformanceSystemInfo.Unknown13);
	printf("PerformanceSystemInfo.Unknown14=%x\n", PerformanceSystemInfo.Unknown14);
	printf("PerformanceSystemInfo.SystemCalls=%x\n", PerformanceSystemInfo.SystemCalls);
}

void InfoTimeSystemInfo()
{
	TIMESYSTEMINFO TimeSystemInfo;
	NTSTATUS rc;
	LARGE_INTEGER LocalTime;
	TIME_FIELDS TimeFields;

	memset(&TimeSystemInfo, 0, sizeof(TimeSystemInfo));

	rc=NtQuerySystemInformation(SystemTimeInfo,
							&TimeSystemInfo,
							sizeof(TimeSystemInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemTimeInfo',  rc=%x\n", rc);
		return;
	}

	RtlSystemTimeToLocalTime(&TimeSystemInfo.KeBootTime, &LocalTime);
	RtlTimeToTimeFields(&LocalTime, &TimeFields);
	printf("KeBootTime      = %02d-%02d-%02d, %02d:%02d:%02d\n", TimeFields.Day, TimeFields.Month, TimeFields.Year,
					TimeFields.Hour, TimeFields.Minute, TimeFields.Second);

	RtlSystemTimeToLocalTime(&TimeSystemInfo.KeSystemTime, &LocalTime);
	RtlTimeToTimeFields(&LocalTime, &TimeFields);

	printf("KeSystemTime    = %02d-%02d-%02d, %02d:%02d:%02d\n", TimeFields.Day, TimeFields.Month, TimeFields.Year,
					TimeFields.Hour, TimeFields.Minute, TimeFields.Second);
	printf("ExpTimeZoneBias = %X%X\n", TimeSystemInfo.ExpTimeZoneBias.HighPart, TimeSystemInfo.ExpTimeZoneBias.LowPart);
	printf("ExpTimeZoneId   = %X\n", TimeSystemInfo.ExpTimeZoneId);
	printf("Unused          = %X\n", TimeSystemInfo.Unused);

	printf("\n");

	return;
}

void InfoSystemPathSystemInfo()
{
	char Buffer[1000];
	NTSTATUS rc;

	/* This information class seems to be obsolete. May be used in very early versions
	of Windows NT. On Windows NT 3.51 SP5, Windows NT 4.0 and Windows 2000, this information
	class dumps the following message in Debugger Window 
	
	"Ex:SystemPathInformation now available via SharedUserData"
	The information class returns STATUS_BREAKPOINT, since the NTOSKRNL calls DbgBreakPoint
	functions which has int 3 instruction */
	rc=NtQuerySystemInformation(SystemPathInfo,
							Buffer,
							sizeof(Buffer),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemPathInfo',  rc=%x\n", rc);
		return;
	}
}

void InfoProcessListSystemInfo()
{
	PPROCESSTHREADSYSTEMINFO pProcessThreadSystemInfo;
	char Buffer[0x4000];
	ULONG BytesReturned;
	NTSTATUS rc;
	ULONG i;


	memset(Buffer, 'A', sizeof(Buffer));

	rc=NtQuerySystemInformation(SystemProcessThreadInfo,
							Buffer,
							sizeof(Buffer),
							&BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemProcessListInfo',  rc=%x\n", rc);
		return;
	}
	pProcessThreadSystemInfo=(PPROCESSTHREADSYSTEMINFO)Buffer;
	while (1) {
		printf("%x -> %S\n", pProcessThreadSystemInfo->ProcessId, pProcessThreadSystemInfo->ProcessName.Buffer?pProcessThreadSystemInfo->ProcessName.Buffer:L"Unknown");
		printf("\tNumber of threads           = %d\n", pProcessThreadSystemInfo->nThreads);
		printf("\tProcessCreateTime           = %I64x\n", pProcessThreadSystemInfo->ProcessCreateTime);
		printf("\tProcessUserTime             = %I64x\n", pProcessThreadSystemInfo->ProcessUserTime);
		printf("\tProcessKernelTime           = %I64x\n", pProcessThreadSystemInfo->ProcessKernelTime);
		printf("\tBasePriority                = %x\n", pProcessThreadSystemInfo->BasePriority);
		printf("\tParentProcessId             = %x\n", pProcessThreadSystemInfo->ParentProcessId);
		printf("\tHandleCount                 = %x\n", pProcessThreadSystemInfo->HandleCount);
		printf("\tPeakVirtualSizeBytes        = %x\n", pProcessThreadSystemInfo->PeakVirtualSizeBytes);
		printf("\tTotalVirtualSizeBytes       = %x\n", pProcessThreadSystemInfo->TotalVirtualSizeBytes);
		printf("\tnPageFaults                 = %x\n", pProcessThreadSystemInfo->nPageFaults);
		printf("\tPeakWorkingSetSizeBytes     = %x\n", pProcessThreadSystemInfo->PeakWorkingSetSizeBytes);
		printf("\tTotalWorkingSetSizeBytes    = %x\n", pProcessThreadSystemInfo->TotalWorkingSetSizeBytes);
		printf("\tPeakPagedPoolUsagePages     = %x\n", pProcessThreadSystemInfo->PeakPagedPoolUsagePages);
		printf("\tTotalPagedPoolUsagePages    = %x\n", pProcessThreadSystemInfo->TotalPagedPoolUsagePages);
		printf("\tPeakNonPagedPoolUsagePages  = %x\n", pProcessThreadSystemInfo->PeakNonPagedPoolUsagePages);
		printf("\tTotalNonPagedPoolUsagePages = %x\n", pProcessThreadSystemInfo->TotalNonPagedPoolUsagePages);
		printf("\tTotalPageFileUsageBytes     = %x\n", pProcessThreadSystemInfo->TotalPageFileUsageBytes);
		printf("\tPeakPageFileUsageBytes      = %x\n", pProcessThreadSystemInfo->PeakPageFileUsageBytes);
		printf("\tTotalPrivateBytes           = %x\n", pProcessThreadSystemInfo->TotalPrivateBytes);
		for (i=0; i<pProcessThreadSystemInfo->nThreads; i++) {
			printf("\t\tThreadKernelTime = %I64x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].ThreadKernelTime);
			printf("\t\tThreadUserTime   = %I64x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].ThreadUserTime);
			printf("\t\tThreadCreateTime = %I64x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].ThreadCreateTime);
			printf("\t\tStartEIP         = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].StartEIP);
			printf("\t\tThread Id        = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].ClientId.UniqueThread);
			printf("\t\tProcess Id       = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].ClientId.UniqueProcess);
			printf("\t\tDynamicPriority  = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].DynamicPriority);
			printf("\t\tBasePriority     = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].BasePriority);
			printf("\t\tnSwitches        = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].nSwitches);
			printf("\t\tUnknown          = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].Unknown);
			printf("\t\tWaitReason       = %x\n", pProcessThreadSystemInfo->ThreadSysInfo[i].WaitReason);
			printf("\n");
		}
		printf("\n\n");
		if (pProcessThreadSystemInfo->RelativeOffset==0) {
			break;
		}
		pProcessThreadSystemInfo=(PPROCESSTHREADSYSTEMINFO)((ULONG)pProcessThreadSystemInfo+pProcessThreadSystemInfo->RelativeOffset);
	}
	printf("\n\n");
}

void InfoSystemServiceDescriptorTables()
{
	/* The system service ONLY returns NumberOfSystemServiceTables field of 
	PSERVICEDESCRIPTORTABLESYSTEMINFO in return buffer in Free Build of the
	operating system. In Checked build, the system service returns all the 
	data in return buffer */
	char Buffer[4000];
	PSERVICEDESCRIPTORTABLESYSTEMINFO pServiceDescriptorTableSystemInfo;
	ULONG BytesReturned;
	NTSTATUS rc;
	ULONG i,j;
	ULONG *Counter;

	rc=NtQuerySystemInformation(SystemServiceDescriptorTableInfo,
							Buffer,
							sizeof(Buffer),
							&BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemServiceDescriptorTableInfo',  rc=%x\n", rc);
		return;
	}
	pServiceDescriptorTableSystemInfo=(PSERVICEDESCRIPTORTABLESYSTEMINFO)Buffer;

	printf("NumberOfSystemServiceTables = %x\n", pServiceDescriptorTableSystemInfo->NumberOfSystemServiceTables);
	Counter=(ULONG *)(&pServiceDescriptorTableSystemInfo->NumberOfServices[pServiceDescriptorTableSystemInfo->NumberOfSystemServiceTables]);

	for (i=0; i<pServiceDescriptorTableSystemInfo->NumberOfSystemServiceTables; i++) {
		printf("\tService Descriptor #%d, Services #%d\n", i, pServiceDescriptorTableSystemInfo->NumberOfServices[i]);
		printf("\t\tService Id  Number of times called\n");
		for (j=0; j<pServiceDescriptorTableSystemInfo->NumberOfServices[i]; j++) {
			printf("\t\t%03x          %08x\n", j, *Counter);
			Counter++;
		}
	}
	printf("\n\n");
}

void InfoSystemIoConfig()
{
	IOCONFIGSYSTEMINFO IoConfigSystemInfo;
	NTSTATUS rc;

	rc=NtQuerySystemInformation(SystemIoConfigInfo,
							&IoConfigSystemInfo,
							sizeof(IoConfigSystemInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemIoConfigInfo',  rc=%x\n", rc);
		return;
	}

	printf("DiskCount     = %d\n", IoConfigSystemInfo.DiskCount);
	printf("FloppyCount   = %d\n", IoConfigSystemInfo.FloppyCount);
	printf("CdRomCount    = %d\n", IoConfigSystemInfo.CdRomCount);
	printf("TapeCount     = %d\n", IoConfigSystemInfo.TapeCount);
	printf("SerialCount   = %d\n", IoConfigSystemInfo.SerialCount);
	printf("ParallelCount = %d\n", IoConfigSystemInfo.ParallelCount);

	printf("\n\n");
}

void InfoSystemProcessorInfo()
{
	/* Following code is written assuming Uniprocessor system, For multiprocessor
	system, one needs to pass array of PROCESSORTIMESYSTEMINFO containing
	number of elements based on number of processors in the system. One can
	get the number of processors using 'SystemBasicInfo' information class */
	PROCESSORTIMESYSTEMINFO ProcessorTimeSystemInfo;
	NTSTATUS rc;


	memset(&ProcessorTimeSystemInfo, 'A', sizeof(ProcessorTimeSystemInfo));
	rc=NtQuerySystemInformation(SystemProcessorTimeInfo,
							&ProcessorTimeSystemInfo,
							sizeof(ProcessorTimeSystemInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemProcessorTimeInfo',  rc=%x\n", rc);
		return;
	}

	printf("TotalProcessorTime     = %I64X\n", ProcessorTimeSystemInfo.TotalProcessorTime);
	printf("TotalProcessorRunTime  = %I64X\n", ProcessorTimeSystemInfo.TotalProcessorRunTime);
	printf("TotalProcessorUserTime = %I64X\n", ProcessorTimeSystemInfo.TotalProcessorUserTime);
	printf("TotalDPCTime           = %I64X\n", ProcessorTimeSystemInfo.TotalDPCTime);
	printf("TotalInterruptTime     = %I64X\n", ProcessorTimeSystemInfo.TotalInterruptTime);
	printf("TotalInterrupts        = %x\n", ProcessorTimeSystemInfo.TotalInterrupts);
	printf("Unused                 = %x\n", ProcessorTimeSystemInfo.Unused);

	printf("\n\n");
}

void InfoSystemNtGlobalFlagInfo()
{
	NTSTATUS rc;
	NTGLOBALFLAGSYSTEMINFO NtGlobalFlagSystemInfo;

	rc=NtQuerySystemInformation(SystemNtGlobalFlagInfo,
							&NtGlobalFlagSystemInfo,
							sizeof(NtGlobalFlagSystemInfo),
							NULL);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemNtGlobalFlagInfo',  rc=%x\n", rc);
		return;
	}

	printf("NtGlobalFlag = %x\n", NtGlobalFlagSystemInfo.NtGlobalFlag);

	if (!EnableOrDisablePrivilege(SE_DEBUG_PRIVILEGE, FALSE)) {
		DbgPrint("Unable to enable SE_DEBUG_PRIVILEGE\n");
		return;
	}

	rc=NtSetSystemInformation(SystemNtGlobalFlagInfo,
							&NtGlobalFlagSystemInfo,
							sizeof(NtGlobalFlagSystemInfo));
	EnableOrDisablePrivilege(SE_DEBUG_PRIVILEGE, TRUE);


	if (rc!=STATUS_SUCCESS) {
		printf("NtSetSystemInformation failed with Information class 'SystemNtGlobalFlagInfo',  rc=%x\n", rc);
		return;
	}
	printf("\n\n");
}

//Information class 10 returns STATUS_NOT_IMPLEMENTED

void InfoSystemModuleInformation()
{
	PDRIVERMODULESYSTEMINFO pDriverModuleSystemInfo;
	char Buffer[80000];
	NTSTATUS rc;
	ULONG i;
	ULONG BytesReturned;

	memset(Buffer, 'A', sizeof(Buffer));

	rc=NtQuerySystemInformation(SystemModuleInfo,
							Buffer,
							sizeof(Buffer),
							&BytesReturned);

	printf("BytesReturned=%x\n", BytesReturned);
	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemModuleInfo',  rc=%x\n", rc);
		return;
	}

	pDriverModuleSystemInfo=(PDRIVERMODULESYSTEMINFO)Buffer;
	printf("Number of driver modules = %d\n", pDriverModuleSystemInfo->nDriverModules);
	for (i=0; i<pDriverModuleSystemInfo->nDriverModules; i++) {
		printf("Unused            = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].Unused);
		printf("Always0           = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].Always0);
		printf("ModuleBaseAddress = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].ModuleBaseAddress);
		printf("ModuleSize        = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].ModuleSize);
		printf("Unknown           = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].Unknown);
		printf("ModuleEntryIndex  = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].ModuleEntryIndex);
		printf("ModuleNameLength  = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].ModuleNameLength);
		printf("ModulePathLength  = %x\n", pDriverModuleSystemInfo->DriverModuleInfo[i].ModulePathLength);
		printf("ModuleName        = %s\n", pDriverModuleSystemInfo->DriverModuleInfo[i].ModuleName);

		printf("\n");
	}

	printf("\n\n");
}

void InfoSystemLockInformation()
{
	char Buffer[90000];
	PSYSTEMRESOURCELOCKINFO pSystemResourceLockInfo;
	NTSTATUS rc;
	ULONG i;
	ULONG _stdcall RtlAcquirePebLock(void);
	ULONG _stdcall RtlReleasePebLock(void);

	memset(Buffer, 'A', sizeof(Buffer));

	RtlAcquirePebLock();
	rc=NtQuerySystemInformation(SystemResourceLockInfo,
							Buffer,
							sizeof(Buffer),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemResourceLockInfo',  rc=%x\n", rc);
		return;
	}
	RtlReleasePebLock();

	pSystemResourceLockInfo=(PSYSTEMRESOURCELOCKINFO)Buffer;

	printf("Number of System resources = %x\n", pSystemResourceLockInfo->nSystemResourceLocks);
	for (i=0; i<pSystemResourceLockInfo->nSystemResourceLocks; i++) {
		printf("%x %x\n", pSystemResourceLockInfo->ResourceLockInfo[i].ResourceAddress, pSystemResourceLockInfo->ResourceLockInfo[i].Unknown);
	}
}

//Information class 13,14,15 returns STATUS_NOT_IMPLEMENTED

void InfoSystemHandleInformation()
{
	PSYSTEMHANDLEINFO pSystemHandleInfo;
	NTSTATUS rc;
	char Buffer[100000];
	ULONG i;

	memset(Buffer, 'A', sizeof(Buffer));

	rc=NtQuerySystemInformation(SystemHandleInfo,
							&Buffer,
							sizeof(Buffer),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemHandleInfo',  rc=%x\n", rc);
		return;
	}

	pSystemHandleInfo=(PSYSTEMHANDLEINFO)Buffer;

	printf("Number of Handle Entries = %x\n", pSystemHandleInfo->nHandleEntries);

	printf("Pid       ObjType   ObjHnd    ObjPtr    AccessMask\n");

	for (i=0; i<pSystemHandleInfo->nHandleEntries; i++) {
		printf("%-8x  %-8x  %-8x  %-8x  %-8x\n", pSystemHandleInfo->HandleInfo[i].Pid,
								pSystemHandleInfo->HandleInfo[i].ObjectType,
								pSystemHandleInfo->HandleInfo[i].HandleValue,
								pSystemHandleInfo->HandleInfo[i].ObjectPointer,
								pSystemHandleInfo->HandleInfo[i].AccessMask);
	}

	printf("\n\n");
}

void InfoSystemObjectInformation()
{
	//TODO Info. Class 17 (SystemObjectInformation)
}


void InfoPageFileInformation()
{
	char Buffer[1000];
	NTSTATUS rc;
	PSYSTEMPAGEFILEINFO pSystemPageFileInfo;

	memset(Buffer, 'A', sizeof(Buffer));

	rc=NtQuerySystemInformation(SystemPageFileInformation,
							Buffer,
							sizeof(Buffer),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemPageFileInformation',  rc=%x\n", rc);
		return;
	}
	pSystemPageFileInfo=(PSYSTEMPAGEFILEINFO)Buffer;
	while (1) {
		printf("pSystemPageFileInfo->CurrentSizePages  = %x\n", pSystemPageFileInfo->CurrentSizePages);
		printf("pSystemPageFileInfo->TotalUsedPages    = %x\n", pSystemPageFileInfo->TotalUsedPages);
		printf("pSystemPageFileInfo->PeakUsedPages     = %x\n", pSystemPageFileInfo->PeakUsedPages);
		printf("pSystemPageFileInfo->uPagefileFileName = %S\n", pSystemPageFileInfo->uPagefileFileName.Buffer);

		if (pSystemPageFileInfo->RelativeOffset==0) {
			break;
		}
		pSystemPageFileInfo=(PSYSTEMPAGEFILEINFO)((ULONG)pSystemPageFileInfo+pSystemPageFileInfo->RelativeOffset);
	}
	printf("%x\n", sizeof(SYSTEMINSTRUCTIONEMULATIONCOUNTINFO));
}

void InfoInstructionEmulationCounts()
{
	SYSTEMINSTRUCTIONEMULATIONCOUNTINFO InstEmulationCountInfo;
	NTSTATUS rc;

	memset(&InstEmulationCountInfo, 0, sizeof(InstEmulationCountInfo));

	rc=NtQuerySystemInformation(SystemInstructionEmulationInfo,
							&InstEmulationCountInfo,
							sizeof(InstEmulationCountInfo),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemInstructionEmulationInfo',  rc=%x\n", rc);
		return;
	}
	printf("InstEmulationCountInfo.VdmSegmentNotPresentCount = %x\n", InstEmulationCountInfo.VdmSegmentNotPresentCount);
	printf("InstEmulationCountInfo.VdmINSWCount              = %x\n", InstEmulationCountInfo.VdmINSWCount);
	printf("InstEmulationCountInfo.VdmESPREFIXCount          = %x\n", InstEmulationCountInfo.VdmESPREFIXCount);
	printf("InstEmulationCountInfo.VdmCSPREFIXCount          = %x\n", InstEmulationCountInfo.VdmCSPREFIXCount);
	printf("InstEmulationCountInfo.VdmSSPREFIXCount          = %x\n", InstEmulationCountInfo.VdmSSPREFIXCount);
	printf("InstEmulationCountInfo.VdmDSPREFIXCount          = %x\n", InstEmulationCountInfo.VdmDSPREFIXCount);
	printf("InstEmulationCountInfo.VdmFSPREFIXCount          = %x\n", InstEmulationCountInfo.VdmFSPREFIXCount);
	printf("InstEmulationCountInfo.VdmGSPREFIXCount          = %x\n", InstEmulationCountInfo.VdmGSPREFIXCount);
	printf("InstEmulationCountInfo.VdmOPER32PREFIXCount      = %x\n", InstEmulationCountInfo.VdmOPER32PREFIXCount);
	printf("InstEmulationCountInfo.VdmADDR32PREFIXCount      = %x\n", InstEmulationCountInfo.VdmADDR32PREFIXCount);
	printf("InstEmulationCountInfo.VdmINSBCount              = %x\n", InstEmulationCountInfo.VdmINSBCount);
	printf("InstEmulationCountInfo.VdmINSWV86Count           = %x\n", InstEmulationCountInfo.VdmINSWV86Count);
	printf("InstEmulationCountInfo.VdmOUTSBCount             = %x\n", InstEmulationCountInfo.VdmOUTSBCount);
	printf("InstEmulationCountInfo.VdmOUTSWCount             = %x\n", InstEmulationCountInfo.VdmOUTSWCount);
	printf("InstEmulationCountInfo.VdmPUSHFCount             = %x\n", InstEmulationCountInfo.VdmPUSHFCount);
	printf("InstEmulationCountInfo.VdmPOPFCount              = %x\n", InstEmulationCountInfo.VdmPOPFCount);
	printf("InstEmulationCountInfo.VdmINTNNCount             = %x\n", InstEmulationCountInfo.VdmINTNNCount);
	printf("InstEmulationCountInfo.VdmINTOCount              = %x\n", InstEmulationCountInfo.VdmINTOCount);
	printf("InstEmulationCountInfo.VdmIRETCount              = %x\n", InstEmulationCountInfo.VdmIRETCount);
	printf("InstEmulationCountInfo.VdmINBIMMCount            = %x\n", InstEmulationCountInfo.VdmINBIMMCount);
	printf("InstEmulationCountInfo.VdmINWIMMCount            = %x\n", InstEmulationCountInfo.VdmINWIMMCount);
	printf("InstEmulationCountInfo.VdmOUTBIMMCount           = %x\n", InstEmulationCountInfo.VdmOUTBIMMCount);
	printf("InstEmulationCountInfo.VdmOUTWIMMCount           = %x\n", InstEmulationCountInfo.VdmOUTWIMMCount);
	printf("InstEmulationCountInfo.VdmINBCount               = %x\n", InstEmulationCountInfo.VdmINBCount);
	printf("InstEmulationCountInfo.VdmINWCount               = %x\n", InstEmulationCountInfo.VdmINWCount);
	printf("InstEmulationCountInfo.VdmOUTBCount              = %x\n", InstEmulationCountInfo.VdmOUTBCount);
	printf("InstEmulationCountInfo.VdmOUTWCount              = %x\n", InstEmulationCountInfo.VdmOUTWCount);
	printf("InstEmulationCountInfo.VdmLOCKPREFIXCount        = %x\n", InstEmulationCountInfo.VdmLOCKPREFIXCount);
	printf("InstEmulationCountInfo.VdmREPNEPREFIXCount       = %x\n", InstEmulationCountInfo.VdmREPNEPREFIXCount);
	printf("InstEmulationCountInfo.VdmREPPREFIXCount         = %x\n", InstEmulationCountInfo.VdmREPPREFIXCount);
	printf("InstEmulationCountInfo.VdmHLTCount               = %x\n", InstEmulationCountInfo.VdmHLTCount);
	printf("InstEmulationCountInfo.VdmCLICount               = %x\n", InstEmulationCountInfo.VdmCLICount);
	printf("InstEmulationCountInfo.VdmSTICount               = %x\n", InstEmulationCountInfo.VdmSTICount);
	printf("InstEmulationCountInfo.VdmBopCount               = %x\n", InstEmulationCountInfo.VdmBopCount);
}

//Information class 20 returns STATUS_INVALID_INFO_CLASS

void InfoSystemCacheInformation()
{
	SYSTEMCACHEINFO SystemCacheInformation;
	ULONG i;
	NTSTATUS rc;

	memset(&SystemCacheInformation, 'A', sizeof(SystemCacheInformation));

	rc=NtQuerySystemInformation(SystemCacheInfo,
							&SystemCacheInformation,
							sizeof(SystemCacheInformation),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemCacheInfo',  rc=%x\n", rc);
		return;
	}
	printf("SystemCacheInformation.TotalSize           = %x\n", SystemCacheInformation.TotalSize);
	printf("SystemCacheInformation.PeakSize            = %x\n", SystemCacheInformation.PeakSize);
	printf("SystemCacheInformation.TotalPageFaultCount = %x\n", SystemCacheInformation.TotalPageFaultCount);
	printf("SystemCacheInformation.MinWorkingSet       = %x\n", SystemCacheInformation.MinWorkingSet);
	printf("SystemCacheInformation.MaxWorkingSet       = %x\n", SystemCacheInformation.MaxWorkingSet);
	for (i=0; i<4; i++) {
		printf("SystemCacheInformation.Unused[%d]           = %x\n", i, SystemCacheInformation.Unused[i]);
	}
}

void InfoPoolTagSystemInfo()
{
	/* This information class will work only if Pool tracking is enabled in the system. Pool
	tracking is enabled by default in checked build of the operating system. For free 
	builds one can use the GFLAGS utility shipped with resource kit to enable pool tagging.
	If Pool tagging is not enabled, this information class returns STATUS_NOT_IMPLEMENTED*/
	PPOOLTAGSYSTEMINFO pPoolTagSystemInfo;
	char Buffer[90000];
	ULONG i;
	NTSTATUS rc;

	memset(Buffer, 'A', sizeof(Buffer));

	rc=NtQuerySystemInformation(SystemPoolTagInfo,
							Buffer,
							sizeof(Buffer),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemPoolTagInfo',  rc=%x\n", rc);
		return;
	}

	pPoolTagSystemInfo=(PPOOLTAGSYSTEMINFO)Buffer;
	printf("Tag   PagedAllocs PagedFrees PagedBytes NPagedAllocs NPagedFrees NPagedBytes\n");
	for (i=0; i<pPoolTagSystemInfo->nTags; i++) {
		ULONG j;
		char *p=(char *)(&pPoolTagSystemInfo->PoolTagInfo[i].PoolTag);
		for (j=0; j<4; j++) {
			printf("%c", isprint(p[j])?p[j]:' ');
		}
		printf("  %08d    %08d   %08d   %08d     %08d    %08d\n", pPoolTagSystemInfo->PoolTagInfo[i].PagedAllocs,
									pPoolTagSystemInfo->PoolTagInfo[i].PagedFrees,
									pPoolTagSystemInfo->PoolTagInfo[i].PagedBytes,
									pPoolTagSystemInfo->PoolTagInfo[i].NonPagedAllocs,
									pPoolTagSystemInfo->PoolTagInfo[i].NonPagedFrees,
									pPoolTagSystemInfo->PoolTagInfo[i].NonPagedBytes);
	}
}

void InfoSystemProcessorScheduleInfo()
{
	PROCESSORSCHEDULEINFO ProcessorScheduleInfo;
	NTSTATUS rc;
	memset(&ProcessorScheduleInfo, 'A', sizeof(ProcessorScheduleInfo));

	rc=NtQuerySystemInformation(SystemProcessorScheduleInfo,
							&ProcessorScheduleInfo,
							sizeof(ProcessorScheduleInfo),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemProcessorScheduleInfo',  rc=%x\n", rc);
		return;
	}
	printf("ProcessorScheduleInfo.nContextSwitches = %x\n", ProcessorScheduleInfo.nContextSwitches);
	printf("ProcessorScheduleInfo.nDPCQueued       = %x\n", ProcessorScheduleInfo.nDPCQueued);
	printf("ProcessorScheduleInfo.nDPCRate         = %x\n", ProcessorScheduleInfo.nDPCRate);
	printf("ProcessorScheduleInfo.TimerResolution  = %x\n", ProcessorScheduleInfo.TimerResolution);
	printf("ProcessorScheduleInfo.nDPCBypasses     = %x\n", ProcessorScheduleInfo.nDPCBypasses);
	printf("ProcessorScheduleInfo.nAPCBypasses     = %x\n", ProcessorScheduleInfo.nAPCBypasses);
}

void InfoSystemDPCInfo()
{
	DPCSYSTEMINFO DPCSystemInfo;
	NTSTATUS rc;

	memset(&DPCSystemInfo, 'A', sizeof(DPCSystemInfo));

	rc=NtQuerySystemInformation(SystemDpcInfo,
							&DPCSystemInfo,
							sizeof(DPCSystemInfo),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemDpcInfo',  rc=%x\n", rc);
		return;
	}
	printf("DPCSystemInfo.Unused                 = %x\n", DPCSystemInfo.Unused);
	printf("DPCSystemInfo.KiMaximumDpcQueueDepth = %x\n", DPCSystemInfo.KiMaximumDpcQueueDepth);
	printf("DPCSystemInfo.KiMinimumDpcRate       = %x\n", DPCSystemInfo.KiMinimumDpcRate);
	printf("DPCSystemInfo.KiAdjustDpcThreshold   = %x\n", DPCSystemInfo.KiAdjustDpcThreshold);
	printf("DPCSystemInfo.KiIdealDpcRate         = %x\n", DPCSystemInfo.KiIdealDpcRate);
}

void InfoSystemTimerInfo()
{
	TIMERSYSTEMINFO TimerSystemInfo;
	NTSTATUS rc;

	memset(&TimerSystemInfo, 'A', sizeof(TimerSystemInfo));

	rc=NtQuerySystemInformation(SystemTimerInfo,
							&TimerSystemInfo,
							sizeof(TimerSystemInfo),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemTimerInfo',  rc=%x\n", rc);
		return;
	}
	printf("TimerSystemInfo.KeTimeAdjustment      = %x\n", TimerSystemInfo.KeTimeAdjustment);
	printf("TimerSystemInfo.KeMaximumIncrement    = %x\n", TimerSystemInfo.KeMaximumIncrement);
	printf("TimerSystemInfo.KeTimeSynchronization = %x\n", TimerSystemInfo.KeTimeSynchronization);
}

void InfoSystemProcessorFaultCountInfo()
{
	PROCESSORSYSTEMFAULTCOUNTINFO ProcessorSystemFaultCountInfo;
	NTSTATUS rc;

	memset(&ProcessorSystemFaultCountInfo, 'A', sizeof(ProcessorSystemFaultCountInfo));

	rc=NtQuerySystemInformation(SystemProcessorFaultCountInfo,
							&ProcessorSystemFaultCountInfo,
							sizeof(ProcessorSystemFaultCountInfo),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemProcessorFaultCountInfo',  rc=%x\n", rc);
		return;
	}
	printf("ProcessorSystemFaultCountInfo.nAlignmentFixup      = %x\n", ProcessorSystemFaultCountInfo.nAlignmentFixup);
	printf("ProcessorSystemFaultCountInfo.nExceptionDispatches = %x\n", ProcessorSystemFaultCountInfo.nExceptionDispatches);
	printf("ProcessorSystemFaultCountInfo.nFloatingEmulation   = %x\n", ProcessorSystemFaultCountInfo.nFloatingEmulation);
	printf("ProcessorSystemFaultCountInfo.Unknown              = %x\n", ProcessorSystemFaultCountInfo.Unknown);
}

void InfoSystemDebugger()
{
	DEBUGGERSYSTEMINFO DebuggerSystemInfo;
	NTSTATUS rc;

	memset(&DebuggerSystemInfo, 'A', sizeof(DebuggerSystemInfo));

	rc=NtQuerySystemInformation(SystemDebuggerInfo,
							&DebuggerSystemInfo,
							sizeof(DebuggerSystemInfo),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemDebuggerInfo',  rc=%x\n", rc);
		return;
	}
	printf("DebuggerSystemInfo.bKdDebuggerEnabled = %x\n", DebuggerSystemInfo.bKdDebuggerEnabled);
	printf("DebuggerSystemInfo.bKdDebuggerPresent = %x\n", DebuggerSystemInfo.bKdDebuggerPresent);
}

void InfoSystemQuota()
{
	QUOTAINFO QuotaInfo;
	NTSTATUS rc;

	rc=NtQuerySystemInformation(SystemQuotaInfo,
							&QuotaInfo,
							sizeof(QuotaInfo),
							NULL);

	if (rc!=STATUS_SUCCESS) {
		printf("NtQuerySystemInformation failed with Information class 'SystemQuotaInfo',  rc=%x\n", rc);
		return;
	}

	printf("CmpGlobalQuota           = %x\n", QuotaInfo.CmpGlobalQuota);
	printf("CmpGlobalQuotaUsed       = %x\n", QuotaInfo.CmpGlobalQuotaUsed);
	printf("MmSizeofPagedPoolInBytes = %x\n", QuotaInfo.MmSizeofPagedPoolInBytes);
	printf("\n\n");
}

main()
{
	InfoBasicSystemInfo();
	InfoProcessorSystemInfo();
	InfoPerformanceSystemInfo();
	InfoTimeSystemInfo();
	InfoSystemPathSystemInfo();
	InfoProcessListSystemInfo();
	InfoSystemServiceDescriptorTables();
	InfoSystemIoConfig();
	InfoSystemProcessorInfo();
	InfoSystemNtGlobalFlagInfo(); 
	InfoSystemModuleInformation();
	InfoSystemLockInformation();
	InfoSystemHandleInformation();
	InfoSystemObjectInformation();
	InfoPageFileInformation();
	InfoInstructionEmulationCounts();
	InfoSystemCacheInformation();
	InfoPoolTagSystemInfo();
	InfoSystemProcessorScheduleInfo();
	InfoSystemDPCInfo();
	InfoSystemTimerInfo();
	InfoSystemProcessorFaultCountInfo();
	InfoSystemDebugger();
	InfoSystemQuota();
	return 0;

}
