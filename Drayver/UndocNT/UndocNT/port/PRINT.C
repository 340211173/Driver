#define _X86_
#include <ntddk.h>
#include <stdio.h>
#include <stdarg.h>

char *MessageType[]={
	"UNUSED_MSG_TYPE",
	"LPC_REQUEST",
	"LPC_REPLY",
	"LPC_DATAGRAM",
	"LPC_LOST_REPLY",
	"LPC_PORT_CLOSED",
	"LPC_CLIENT_DIED",
	"LPC_EXCEPTION",
	"LPC_DEBUG_EVENT",
	"LPC_ERROR_EVENT",
	"LPC_CONNECTION_REQUEST"
};

#include "undocnt.h"

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

void PrintMessage(PLPCMESSAGE pLpcMessage)
{
	ULONG *ptr;
	ULONG i;

	MyPrintf("ActualMessageLength=%x, TotalMessageLength=%x\n",
			pLpcMessage->ActualMessageLength,
			pLpcMessage->TotalMessageLength);

	MyPrintf("MessageType=");
	if (pLpcMessage->MessageType > LPC_CONNECTION_REQUEST) {
		MyPrintf("Unknown\n");
	} else {
		MyPrintf("%s\n", MessageType[pLpcMessage->MessageType]);
	}
	
	MyPrintf("ClientProcessId=%x, ClientThreadId=%x\n",
			pLpcMessage->ClientProcessId,
			pLpcMessage->ClientThreadId);
	MyPrintf("MessageId=%x\n", 
			pLpcMessage->MessageId);
	MyPrintf("SharedSectionSize=%x\n", 
			pLpcMessage->SharedSectionSize);
	MyPrintf("Message Data=");

	ptr=(ULONG *)(pLpcMessage->MessageData);

	/*for (i=0; i<pLpcMessage->ActualMessageLength/sizeof(ULONG); i++) {
		MyPrintf("%d ", ptr[i]);
	}*/
	for (i=0; i<pLpcMessage->ActualMessageLength/sizeof(ULONG); i++) {
		MyPrintf("%x ", ptr[i]);
	}

	MyPrintf("\n\n");
	return;
}
