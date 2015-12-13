#define IOCTL_SEND_DATA_STRUCT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0800, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA) 
#define IOCTL_JUST_CHECK_STATUS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

#ifndef _ENUMPROC_
#define _ENUMPROC_

typedef struct TMegaDriverData{
	char procToFind[255];
	char procToOpen[255];
	char status[255];
}MegaDriverData, PMegaDriverData;

typedef struct _EXAMPLE_DEVICE_EXTENSION { 
	PDEVICE_OBJECT	fdo; 
	UNICODE_STRING	ustrSymLinkName; // L"\\DosDevices\\Example" 
} EXAMPLE_DEVICE_EXTENSION, *PEXAMPLE_DEVICE_EXTENSION;

int DataSize = sizeof(MegaDriverData);

typedef enum _SYSTEM_INFORMATION_CLASS { 
	SystemBasicInformation, // 0 
	SystemProcessorInformation, // 1 
	SystemPerformanceInformation, // 2
	SystemTimeOfDayInformation, // 3
	SystemNotImplemented1, // 4
	SystemProcessesAndThreadsInformation, // 5
	SystemCallCounts, // 6
	SystemConfigurationInformation, // 7
	SystemProcessorTimes, // 8
	SystemGlobalFlag, // 9
	SystemNotImplemented2, // 10
	SystemModuleInformation, // 11
	SystemLockInformation, // 12
	SystemNotImplemented3, // 13
	SystemNotImplemented4, // 14
	SystemNotImplemented5, // 15
	SystemHandleInformation, // 16
	SystemObjectInformation, // 17
	SystemPagefileInformation, // 18
	SystemInstructionEmulationCounts, // 19
	SystemInvalidInfoClass1, // 20
	SystemCacheInformation, // 21
	SystemPoolTagInformation, // 22
	SystemProcessorStatistics, // 23
	SystemDpcInformation, // 24
	SystemNotImplemented6, // 25
	SystemLoadImage, // 26
	SystemUnloadImage, // 27
	SystemTimeAdjustment, // 28
	SystemNotImplemented7, // 29
	SystemNotImplemented8, // 30
	SystemNotImplemented9, // 31
	SystemCrashDumpInformation, // 32
	SystemExceptionInformation, // 33
	SystemCrashDumpStateInformation, // 34
	SystemKernelDebuggerInformation, // 35
	SystemContextSwitchInformation, // 36
	SystemRegistryQuotaInformation, // 37
	SystemLoadAndCallImage, // 38
	SystemPrioritySeparation, // 39
	SystemNotImplemented10, // 40
	SystemNotImplemented11, // 41
	SystemInvalidInfoClass2, // 42
	SystemInvalidInfoClass3, // 43
	SystemTimeZoneInformation, // 44
	SystemLookasideInformation, // 45
	SystemSetTimeSlipEvent, // 46
	SystemCreateSession, // 47
	SystemDeleteSession, // 48
	SystemInvalidInfoClass4, // 49
	SystemRangeStartInformation, // 50
	SystemVerifierInformation, // 51
	SystemAddVerifier, // 52
	SystemSessionProcessesInformation // 53
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_THREAD_INFORMATION {
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
	ULONG ContextSwitchCount;
	LONG State;
	LONG WaitReason;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG NextEntryDelta;
	ULONG ThreadCount;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY BasePriority;
	ULONG ProcessId;
	ULONG InheritedFromProcessId;
	ULONG HandleCount;
	ULONG Reserved2[2];
	VM_COUNTERS VmCounters;
	IO_COUNTERS IoCounters;
	SYSTEM_THREAD_INFORMATION Threads[1];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;
//самый хитрый код, собстно ZwQuerySystemInformation
NTSYSAPI
	NTSTATUS
	NTAPI
	ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

#endif