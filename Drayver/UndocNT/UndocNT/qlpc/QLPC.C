#define _X86_

#include <ntddk.h>
#include <stdio.h>
#include <stdarg.h>

#include "undocnt.h"

/*Define the WIN32 calls we are using, since we can not include both NTDDK.H and
WINDOWS.H in the same 'C' file.*/

HANDLE _stdcall OpenProcess(
    ULONG dwDesiredAccess,	// access flag 
    BOOLEAN bInheritHandle,	// handle inheritance flag 
    ULONG dwProcessId 	// process identifier 
   );	
ULONG _stdcall GetLastError();

BOOLEAN _stdcall DuplicateHandle(
    HANDLE hSourceProcessHandle,	// handle to process with handle to duplicate 
    HANDLE hSourceHandle,	// handle to duplicate 
    HANDLE hTargetProcessHandle,	// handle to process to duplicate to 
    PHANDLE lpTargetHandle,	// pointer to duplicate handle 
    ULONG dwDesiredAccess,	// access for duplicate handle 
    BOOLEAN bInheritHandle,	// handle inheritance flag
    ULONG dwOptions 	// optional actions 
   );
#define DUPLICATE_SAME_ACCESS 0x00000002

HANDLE _stdcall GetCurrentProcess();
HANDLE _stdcall GetCurrentThread();
ULONG _stdcall GetCurrentProcessId();
ULONG _stdcall GetCurrentThreadId();
 


#define EVENTPAIRNAME	L"\\MyEventPair"

void MyPrintf(const char *fmt, ...)
{
        static va_list args;
        static char buf[10000];
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        //OutputDebugString ((buf));
		printf(buf);
        return;
}


server()
{
	static HANDLE EventPairHandle;
	HANDLE ClientEventPairHandle;

	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING uString;
	ULONG ClientPid, ClientTid;
	HANDLE ClientProcessHandle, ClientThreadHandle;
	ULONG OpenThreadParam[2];
	int rc;
	


	memset(&ObjectAttr, 0, sizeof(ObjectAttr));
	ObjectAttr.Length=sizeof(ObjectAttr);
	RtlInitUnicodeString(&uString, EVENTPAIRNAME);
	ObjectAttr.ObjectName=&uString;
	rc=NtCreateEventPair(&EventPairHandle, STANDARD_RIGHTS_ALL, &ObjectAttr);
	if (rc==0) {
		MyPrintf("EventPairHandle=%x\n", EventPairHandle);
	} else {
		MyPrintf("Unable to create event pair, rc=%x\n", rc);
		return -1;
	}

	MyPrintf("Enter pid and tid of the client : ");
	scanf("%d%d", &ClientPid, &ClientTid);
	
	ClientProcessHandle=OpenProcess(PROCESS_ALL_ACCESS,
									FALSE,
									ClientPid);
	if (ClientProcessHandle==NULL) {
		rc=GetLastError();
		MyPrintf("Unable to open handle to process, rc=%x %d\n", 
					rc, rc);
		return -1;
	}

	memset(&ObjectAttr, 0, sizeof(ObjectAttr));
	ObjectAttr.Length=sizeof(ObjectAttr);
	OpenThreadParam[0]=ClientPid;
	OpenThreadParam[1]=ClientTid;

	rc=NtOpenThread(&ClientThreadHandle,
					THREAD_ALL_ACCESS,
					&ObjectAttr,
					OpenThreadParam);
	if (rc!=0) {
		MyPrintf("NtOpenThread failed, rc=%x\n", rc);
		return -1;
	}
	MyPrintf("ClientProcessHandle = %x\n", ClientProcessHandle);
	MyPrintf("ClientThreadHandle  = %x\n", ClientThreadHandle);

	rc=DuplicateHandle(GetCurrentProcess(),
					EventPairHandle,
					ClientProcessHandle,
					&ClientEventPairHandle,
					0,
					FALSE,
					DUPLICATE_SAME_ACCESS);
	if (rc==FALSE) {
		rc=GetLastError();
		MyPrintf("DuplicateHandle failed, rc=%x %d\n", rc);
		return -1;
	}

	MyPrintf("Client EventPair handle = %x\n", ClientEventPairHandle);
	rc=ZwSetInformationThread(GetCurrentThread(),
								8,
								&EventPairHandle,
								4);

	if (rc!=0) {
		MyPrintf("NtSetInformationThread failed in server, rc=%x\n", rc);
		return -1;
	}

	rc=ZwSetInformationThread(ClientThreadHandle,
								8,
								&EventPairHandle,
								4);

	if (rc!=0) {
		MyPrintf("NtSetInformationThread failed in client, rc=%x\n", rc);
		return -1;
	}

	while (1) {
		ULONG ret_val;
		_asm int 2Ch
		_asm mov ret_val, eax
		if (ret_val!=0) {
			MyPrintf("int 2C returned error, rc=%x\n", ret_val);
		} else {
			MyPrintf("int 2C returned\n");
		}
	}
}

client()
{
	MyPrintf("Client Process id = %d\n", GetCurrentProcessId());
	MyPrintf("Client Thread id = %d\n", GetCurrentThreadId());
	getchar();
	while (1) {
		ULONG ret_val;

		_asm int 2Bh
		_asm mov ret_val, eax
		if (ret_val!=0) {
			MyPrintf("int 2B returned error, rc=%x\n", ret_val);
		} else {
			MyPrintf("int 2B returned\n");
		}
		//getchar();
	}
	return 0;
}


main(int argc, char **argv)
{
	if (argc==1) {
		server();
	} else {
		client();
	}
	return 0;
}
