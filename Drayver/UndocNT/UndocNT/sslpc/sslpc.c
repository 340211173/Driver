#define _X86_

#include <ntddk.h>
#include <stdio.h>

#include "undocnt.h"
#include "..\port\print.h"

#define PORTNAME L"\\Windows\\MyPort"

#define SHARED_SECTION_SIZE 0x10000

typedef struct SharedLpcMessage {
	ULONG ServerBaseAddress;
	ULONG MessageOffset;
} SHAREDLPCMESSAGE, *PSHAREDLPCMESSAGE;

/*Define the WIN32 calls we are using, since we can not include both NTDDK.H and
WINDOWS.H in the same 'C' file.*/
ULONG _stdcall GetCurrentProcessId();
ULONG _stdcall GetCurrentThreadId();
HANDLE _stdcall CreateFileMappingA(
    HANDLE hFile,	// handle to file to map 
    PVOID lpFileMappingAttributes,	// optional security attributes 
    ULONG flProtect,	// protection for mapping object 
    ULONG dwMaximumSizeHigh,	// high-order 32 bits of object size  
    ULONG dwMaximumSizeLow,	// low-order 32 bits of object size  
    LPCSTR lpName 	// name of file-mapping object 
   );


/* Extract the message string from the shared section and
reverse it */
void ProcessMessageData(PLPCMESSAGE pLpcMessage)
{
	PSHAREDLPCMESSAGE SharedLpcMessage;
	char *ServerView;

	SharedLpcMessage=(PSHAREDLPCMESSAGE)(pLpcMessage->MessageData);
	ServerView=((char *)SharedLpcMessage->ServerBaseAddress)+
				SharedLpcMessage->MessageOffset;
	strrev(ServerView);
}

server(POBJECT_ATTRIBUTES ObjectAttr)
{
	LPCSECTIONMAPINFO SectionMapInfo;
	int rc;
	HANDLE PortHandle;
	LPCMESSAGE LpcMessage;


	/* Create the named port */
	rc=NtCreatePort(&PortHandle, ObjectAttr, 0x0, 0x0, 0x00000);
	if (rc==0) {
		HANDLE AcceptPortHandle;
		BOOLEAN FirstTime=TRUE;

		MyPrintf("Port created, PortHandle=%x %d\n", PortHandle,
					PortHandle);
		memset(&LpcMessage, 0, sizeof(LpcMessage));

		while (1) {
			if ((FirstTime) || (LpcMessage.MessageType==LPC_CONNECTION_REQUEST)) {
				/* If you are first time listening or if the previous
				message was a connection request then do not send
				any reply but just wait on the message */
				rc=NtReplyWaitReceivePort(PortHandle, NULL, NULL, &LpcMessage);
				FirstTime=FALSE;
			} else {
				/* Send a reply to the previous message and wait
				for the new message */
				MyPrintf("Sending reply and Waiting for the request ....\n");

				rc=NtReplyWaitReceivePort(PortHandle, 0, &LpcMessage, &LpcMessage);
				if (rc!=0) {
					MyPrintf("NtReplyWaitReceivePort failed, rc=%x\n", rc);
					return -1;
				}
			}

			if (LpcMessage.MessageType==LPC_CONNECTION_REQUEST) {
				MyPrintf("Got the connection request\n");
				PrintMessage(&LpcMessage);

				/* If you get the connection request accept
				and complete the request */
				memset(&SectionMapInfo, 0, sizeof(SectionMapInfo));
				SectionMapInfo.Length=0x0C;
				rc=NtAcceptConnectPort(&AcceptPortHandle, 0, &LpcMessage,
									1, 0, &SectionMapInfo);
				if (rc!=0) {
					MyPrintf("NtAcceptConnectPort failed, rc=%x\n", rc);
					return -1;
				}
				MyPrintf("AcceptPortHandle=%x\n", AcceptPortHandle);
				MyPrintf("SectionMapInfo.SectionSize=%x\n", SectionMapInfo.SectionSize);
				MyPrintf("SectionMapInfo.ServerBaseAddress=%x\n", SectionMapInfo.ServerBaseAddress);

				rc=NtCompleteConnectPort(AcceptPortHandle);
				if (rc!=0) {
					MyPrintf("NtCompleteConnectPort failed, rc=%x\n", rc);
					return -1;
				}
			} else if (LpcMessage.MessageType==LPC_REQUEST) {

				/* Process the message received and send the
				reply in the next iteration of while 1*/
				MyPrintf("Got the request\n");
				PrintMessage(&LpcMessage);
				ProcessMessageData(&LpcMessage);
			}
		}
	} else {
		MyPrintf("NtCreatePort failed, rc=%x\n", rc);
	}
}

client(PUNICODE_STRING uString)
{
	static int Param3;
	HANDLE hFileMapping;
	LPCSECTIONINFO SectionInfo;
	LPCSECTIONMAPINFO SectionMapInfo;
	ULONG ServerBaseAddress;
	ULONG ClientBaseAddress;
	char *ClientView;
	int rc;
	LPCMESSAGE LpcMessage;
	HANDLE PortHandle;

	hFileMapping=CreateFileMappingA((HANDLE)0xFFFFFFFF, NULL, 
					PAGE_READWRITE, 0, SHARED_SECTION_SIZE, 
					NULL);

	if (hFileMapping==NULL) {
		MyPrintf("Unable to create file mapping\n");
		return -1;
	}
	memset(&SectionInfo, 0, sizeof(SectionInfo));
	memset(&SectionMapInfo, 0, sizeof(SectionMapInfo));

	SectionInfo.Length=0x18;
	SectionInfo.SectionHandle=hFileMapping;
	SectionInfo.SectionSize=SHARED_SECTION_SIZE;

	SectionMapInfo.Length=0x0C;


	MyPrintf("ClientProcessId=%x, ClientThreadId=%x\n",
				GetCurrentProcessId(),
				GetCurrentThreadId());

	rc=NtConnectPort(&PortHandle, uString,
					&Param3, &SectionInfo, NULL, 
					NULL, NULL, NULL);

	if (rc==0) {
		MyPrintf("PortHandle=%x\n", PortHandle);
		MyPrintf("Client Base address=%x\n", SectionInfo.ClientBaseAddress);
		MyPrintf("Server Base address=%x\n", SectionInfo.ServerBaseAddress);
		ServerBaseAddress=SectionInfo.ServerBaseAddress;
		ClientBaseAddress=SectionInfo.ClientBaseAddress;
	} else {
		MyPrintf("Connect failed, rc=%x %d\n", rc);
		return -1;
	}

	while (1) {
		static char MessageString[SHARED_SECTION_SIZE];
		int MessageOffset=0;
		PSHAREDLPCMESSAGE SharedLpcMessage;
		
		printf("Enter the message string, enter 'quit' to exit : ");
		gets(MessageString);
		if (stricmp(MessageString, "quit")==0) {
			ZwClose(PortHandle);
			return 0;
		}
		fflush(stdin);
		printf("Enter the offset in shared memory where the message is to be kept : ");
		scanf("%d", &MessageOffset);

		if ((MessageOffset+strlen(MessageString))>=SHARED_SECTION_SIZE) {
			MyPrintf("Message can not fit in shared memory window\n");
			return -1;
		}
		/* Fill in the message */
		memset(&LpcMessage, 0, sizeof(LpcMessage));
		LpcMessage.ActualMessageLength=0x08;
		LpcMessage.TotalMessageLength=0x20;
		SharedLpcMessage=(PSHAREDLPCMESSAGE)(LpcMessage.MessageData);
		MyPrintf("Server base address=%x\n", ServerBaseAddress); 
		SharedLpcMessage->ServerBaseAddress=ServerBaseAddress;
		SharedLpcMessage->MessageOffset=MessageOffset;

		ClientView=((char *)ClientBaseAddress)+MessageOffset;
		strcpy(ClientView, MessageString);
		getchar();

		/* Send the message and wait for the reply */
		MyPrintf("Sending request and waiting for reply....\n");
		rc=NtRequestWaitReplyPort(PortHandle, &LpcMessage, &LpcMessage);
		if (rc!=0) {
			MyPrintf("NtRequestWaitReplyport failed, rc=%x\n", rc);
			return -1;
		}

		/* Print the reply received */
		MyPrintf("Got the reply\n");
		PrintMessage(&LpcMessage);
		//
		MyPrintf("Reply = %s\n", ClientView);
	}
	getchar();
}


main(int argc, char **argv)
{
	OBJECT_ATTRIBUTES ObjectAttr;
	UNICODE_STRING uString;

	/* Initializes the object attribute structure */
	RtlInitUnicodeString(&uString, PORTNAME);
	InitializeObjectAttributes(&ObjectAttr, &uString, OBJ_CASE_INSENSITIVE, NULL, NULL);

	if (argc==1) {
		/* If no parameters are specified for the program
		act as the server */

		server(&ObjectAttr);
	} else {
		/* If any command line parameter is specified it
		acts as the client */
		client(&uString);
	}
	return 0;
}

