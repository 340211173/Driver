/* Should be compiled in release mode */
#define _X86_

#include <ntddk.h>
#include <string.h>
#include <stdio.h>

#include "undocnt.h"
#include "gate.h"


/*Define the WIN32 calls we are using, since we can not include both NTDDK.H and
WINDOWS.H in the same 'C' file.*/
typedef struct _OSVERSIONINFO{  
    ULONG dwOSVersionInfoSize; 
    ULONG dwMajorVersion; 
    ULONG dwMinorVersion; 
    ULONG dwBuildNumber; 
    ULONG dwPlatformId; 
    CCHAR szCSDVersion[ 128 ]; 
} OSVERSIONINFO, *LPOSVERSIONINFO; 
BOOLEAN _stdcall GetVersionExA(LPOSVERSIONINFO);
PVOID _stdcall VirtualAlloc(PVOID, ULONG, ULONG, ULONG);

/* Max vad entries */
#define MAX_VAD_ENTRIES		0x200



/* Following variables are accessed in RING0.ASM */
ULONG NtVersion;		   
ULONG PebOffset;
ULONG VadRootOffset;

#pragma pack(1)
typedef struct VadInfo {
	void *VadLocation;
	VAD Vad;
} VADINFO, *PVADINFO;
#pragma pack()

VADINFO VadInfoArray[MAX_VAD_ENTRIES];
int VadInfoArrayIndex;
PVAD VadTreeRoot;


/* Recursive function which walks the vad tree and fills up the
global VadInfoArray with the Vad entries. Function is limited by
the MAX_VAD_ENTRIES. Other VADs after this are not stored */
void _stdcall VadTreeWalk(PVAD VadNode)
{
	if (VadNode==NULL) {
		return;
	}

	VadTreeWalk(VadNode->LeftLink);

	if (VadInfoArrayIndex<MAX_VAD_ENTRIES) {
		VadInfoArray[VadInfoArrayIndex].VadLocation=VadNode;
		VadInfoArray[VadInfoArrayIndex].Vad.StartingAddress=VadNode->StartingAddress;
		VadInfoArray[VadInfoArrayIndex].Vad.EndingAddress=VadNode->EndingAddress;
		if (NtVersion==5) {
			(ULONG)VadInfoArray[VadInfoArrayIndex].Vad.StartingAddress<<=12;
			(ULONG)VadInfoArray[VadInfoArrayIndex].Vad.EndingAddress+=1;
			(ULONG)VadInfoArray[VadInfoArrayIndex].Vad.EndingAddress<<=12;
			(ULONG)VadInfoArray[VadInfoArrayIndex].Vad.EndingAddress-=1;
		}

		VadInfoArray[VadInfoArrayIndex].Vad.ParentLink=VadNode->ParentLink;
		VadInfoArray[VadInfoArrayIndex].Vad.LeftLink=VadNode->LeftLink;
		VadInfoArray[VadInfoArrayIndex].Vad.RightLink=VadNode->RightLink;
		VadInfoArray[VadInfoArrayIndex].Vad.Flags=VadNode->Flags;
		VadInfoArrayIndex++;
	}
	VadTreeWalk(VadNode->RightLink);
}

/* C function called through assembly stub */
void _stdcall CFuncDumpVad(PVAD VadRoot)
{
	VadTreeRoot=VadRoot;
	VadInfoArrayIndex=0;
	VadTreeWalk(VadRoot);
}


/* Displays the Vad tree */
void VadTreeDisplay()
{
	int i;
	printf("VadRoot is located @%08x\n\n", VadTreeRoot);
	printf("Vad@      Starting  Ending    Parent    LeftLink  RightLink\n");
	for (i=0; i<VadInfoArrayIndex; i++) {
		printf("%08x  %08x  %08x  %8x  %08x  %08x\n",
				VadInfoArray[i].VadLocation, 
				VadInfoArray[i].Vad.StartingAddress, 
				VadInfoArray[i].Vad.EndingAddress,	
				VadInfoArray[i].Vad.ParentLink, 
				VadInfoArray[i].Vad.LeftLink, 
				VadInfoArray[i].Vad.RightLink);
	}
}

void SetDataStructureOffsets()
{
	switch (NtVersion) {
		case 3:	PebOffset=0x40;
				VadRootOffset=0x170;
				break;
		case 4:	PebOffset=0x44;
				VadRootOffset=0x170;
				break;
		case 5:	PebOffset=0x44;
				VadRootOffset=0x194;
				break;
	}
}

main()
{
	USHORT CallGateSelector;
	int rc;
	short farcall[3];
	void DumpVad(void);
	void *ptr;
	OSVERSIONINFO VersionInfo;

	VersionInfo.dwOSVersionInfoSize=sizeof(VersionInfo);
	if (GetVersionExA(&VersionInfo)==TRUE) {
		NtVersion=VersionInfo.dwMajorVersion;
	}

	if ((NtVersion<3)||(NtVersion>5)) {
		printf("Unsupported NT version, exiting...");
		return 0;
	}
	SetDataStructureOffsets();

	/* Creates call gate to read vad tree from Ring 3 */
	rc=CreateCallGate(DumpVad, 0, &CallGateSelector);

	if (rc==SUCCESS) {
		farcall[2]=CallGateSelector;
		
        _asm {
                call fword ptr [farcall]
        }

		printf("Dumping the Vad tree ...\n\n");
		VadTreeDisplay();

		printf("\n\nAllocating memory using VirtualAlloc\n");
		ptr=VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_READONLY);
		if (ptr==NULL) {
			printf("Unable to allocate memory\n");
			goto Quit;
		}
		printf("Memory allocated @%x\n", ptr);
	
        _asm {
                call fword ptr [farcall]
        }

		printf("\n\nDumping the Vad tree again...\n\n");
		VadTreeDisplay();

Quit:
		rc=FreeCallGate(CallGateSelector);
		if (rc!=SUCCESS) {
			printf("FreeCallGate failed, CallGateSelector=%x, rc=%x\n",
						CallGateSelector, rc);
		}

	} else {
		printf("CreateCallGate failed, rc=%x\n", rc);
	}
	return 0;
}
