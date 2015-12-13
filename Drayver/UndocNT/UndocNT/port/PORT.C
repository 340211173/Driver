/* Demonstrates the short message LPC provided by
the port object */

#define _X86_
#include <ntddk.h>
#include <stdio.h>

#include "undocnt.h"
#include "print.h"

/*Define the WIN32 calls we are using, since we can not include both NTDDK.H and
WINDOWS.H in the same 'C' file.*/
ULONG _stdcall GetCurrentProcessId();
ULONG _stdcall GetCurrentThreadId();
 


#define PORTNAME L"\\Windows\\MyPort"

/* Just makes a bitwise not of the data received in the message */
void ProcessMessageData(PLPCMESSAGE pLpcMessage)
{
	ULONG *ptr;
	ULONG i;

	ptr=(ULONG *)(pLpcMessage->MessageData);
	for (i=0; i<pLpcMessage->ActualMessageLength/sizeof(ULONG); i++) {
		ptr[i]=~ptr[i];
	}
	return;
}

BOOLEAN ProcessConnectionRequest(HANDLE PortHandle, 
							PLPCMESSAGE LpcMessage,
							PHANDLE pAcceptPortHandle)
{
	HANDLE AcceptPortHandle;
	int rc;

	*pAcceptPortHandle=NULL;

	MyPrintf("Got the connection request\n");
	PrintMessage(LpcMessage);
	ProcessMessageData(LpcMessage);

	rc=NtAcceptConnectPort(&AcceptPortHandle, 
						0, 
						LpcMessage,
						1, 
						0, 
						NULL);
	if (rc!=0) {
		MyPrintf("NtAcceptConnectPort failed, rc=%x\n", rc);
		return FALSE;
	}

	MyPrintf("AcceptPortHandle=%x\n", AcceptPortHandle);
	rc=NtCompleteConnectPort(AcceptPortHandle);
	if (rc!=0) {
		ZwClose(AcceptPortHandle);
		MyPrintf("NtCompleteConnectPort failed, rc=%x\n", rc);
		return FALSE;
	}
	*pAcceptPortHandle=AcceptPortHandle;
	return TRUE;
}

BOOLEAN ProcessLpcRequest(HANDLE PortHandle, PLPCMESSAGE LpcMessage)
{
	int rc;
	MyPrintf("Got the LPC request\n");
	PrintMessage(LpcMessage);
	ProcessMessageData(LpcMessage);

	rc=NtReplyPort(PortHandle, LpcMessage);
	if (rc!=0) {
		MyPrintf("NtReplyPort failed, rc=%x\n", rc);
		return FALSE;
	}
	return TRUE;
}

server(POBJECT_ATTRIBUTES ObjectAttr)
{
	BOOLEAN RetVal;
	int rc;
	HANDLE PortHandle;
	LPCMESSAGE LpcMessage;

	/* Create the named port */
	rc=NtCreatePort(&PortHandle, ObjectAttr, 0x0, 0x0, 0x00000);

	if (rc==0) {
		
		MyPrintf("Port created, PortHandle=%x %d\n", PortHandle,
					PortHandle);
		memset(&LpcMessage, 0, sizeof(LpcMessage));

		while (1) {
			HANDLE AcceptPortHandle;
			/* Wait for the message on the port*/
			rc=NtReplyWaitReceivePort(PortHandle, 
									NULL, 
									NULL,
									&LpcMessage);
			if (rc!=0) {
				MyPrintf("NtReplyWaitReceivePort failed, rc=%x\n", rc);
				ZwClose(PortHandle);
				return -1;
			}
			RetVal=TRUE;
			switch (LpcMessage.MessageType) {
				case LPC_CONNECTION_REQUEST: 
					RetVal=ProcessConnectionRequest(PortHandle, 
										&LpcMessage, 
										&AcceptPortHandle);
					break;
				case LPC_REQUEST:
					RetVal=ProcessLpcRequest(PortHandle, 
										&LpcMessage);
					break;
				default:
					PrintMessage(&LpcMessage);
					break;
			}
			if (RetVal==FALSE) {
				break;
			}
		}
	} else {
		MyPrintf("Error creating port, rc=%x\n", rc);
	}
}


client(PUNICODE_STRING uString)
{
	static int Param3;
	HANDLE PortHandle;
	ULONG ConnectDataBuffer[]={0, 1, 2, 3, 4, 5};
	int Size=sizeof(ConnectDataBuffer);
	ULONG i;
	ULONG Value=0xFFFFFFFF;
	int rc;
	LPCMESSAGE LpcMessage;

	MyPrintf("ClientProcessId=%x, ClientThreadId=%x\n",
				GetCurrentProcessId(),
				GetCurrentThreadId());

	rc=NtConnectPort(&PortHandle, 
					uString,
					&Param3, 
					0, 
					0, 
					0, 
					ConnectDataBuffer, 
					&Size);
	if (rc==0) {
		ULONG *ptr;
		MyPrintf("Connect success, PortHandle=%x %d\n",
				PortHandle, PortHandle);
		for (i=0; i<Size/sizeof(ULONG); i++) {
			MyPrintf("%x ", ConnectDataBuffer[i]);
		}

		MyPrintf("\n\n");
		rc=NtRegisterThreadTerminatePort(PortHandle);
		if (rc!=0) {
			MyPrintf("Unable to register thread termination port\n");
			ZwClose(PortHandle);
			return -1;
		}

		/* Demonstrates how to send a datagram using 
		NtRequestPort */
		memset(&LpcMessage, 0, sizeof(LpcMessage));
		LpcMessage.ActualMessageLength=0x08;
		LpcMessage.TotalMessageLength=0x20;
		ptr=(ULONG *)LpcMessage.MessageData;
		ptr[0]=0xBABABABA;
		ptr[1]=0xCACACACA;

		rc=NtRequestPort(PortHandle, &LpcMessage);


		while (1) {
			/* Fill in the message */
			memset(&LpcMessage, 0, sizeof(LpcMessage));
			LpcMessage.ActualMessageLength=0x08;
			LpcMessage.TotalMessageLength=0x20;
			ptr=(ULONG *)LpcMessage.MessageData;
			ptr[0]=Value;
			ptr[1]=Value-1;

			MyPrintf("Stop sending message (Y/N)? ");
			fflush(stdin);
			if (toupper(getchar())=='Y') {
				ZwClose(PortHandle);
				break;
			}

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
			Value-=2;
		}
	} else {
		MyPrintf("Connect failed, rc=%x %d\n", rc);
		return -1;
	}
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

