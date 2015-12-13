/* Should be compiled in release mode to run properly */
#include <windows.h>
#include <string.h>

#include <stdio.h>
#include "gate.h"

/* Global array to hold the page directory */
DWORD PageDirectory[1024];

/* C functions calld from the assembly stub */
void _stdcall CFuncGetPageDirectory()
{
	DWORD *PageDir=(DWORD *)0xC0300000;
	int i=0;

	for (i=0; i<1024; i++) {
		PageDirectory[i]=PageDir[i];
	}
}

/* Displays the contents of page directory. Starting virtual address
represented by the page directory entry is shown followed by the 
physical page address of the page table */
void DisplayPageDirectory()
{
	int i;
	int ctr=0;

	printf("Page directory for the process, pid=%x\n", GetCurrentProcessId());
	for (i=0; i<1024; i++) {
		if (PageDirectory[i]&0x01) {
			if ((ctr%4)==0) {
				printf("\n");
			}
			printf("%08x:%08x ", i<<22, PageDirectory[i]&0xFFFFF000);
			ctr++;
		}
	}
	printf("\n");
}

main()
{
	WORD CallGateSelector;
	int rc;
	short farcall[3];
	/* Assembly stub which is called through callgate */
	void GetPageDirectory(void);

	/* Creates a callgate to read the page directory from Ring 3 */
	rc=CreateCallGate(GetPageDirectory, 0, &CallGateSelector);


	if (rc==SUCCESS) {
		farcall[2]=CallGateSelector;
	    _asm {
                call fword ptr [farcall]
        }

		DisplayPageDirectory();
		getchar();

		/* Releases the callgate */
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
