/*
CGATEAPP.C

Copyright (C) 1997 Prasad Dabak and Sandeep Phadke and Milind Borate

Sample application which uses CGATEDLL.DLL API for creating callgates
*/

#include <windows.h>
#include <stdio.h>
#include "gate.h"


void DumpBaseMemory()
{
	unsigned short BaseMemory;
	outp( 0x70, 0x15 );
	BaseMemory = inp( 0x71 ); 
	outp( 0x70, 0x16 );
	BaseMemory += inp(0x71) << 8;
	printf("Base memory     = %dK\n", BaseMemory);
}

void DumpExtendedMemory()
{
	unsigned short ExtendedMemory;

	outp( 0x70, 0x17 );
	ExtendedMemory = inp( 0x71 ); 
	outp( 0x70, 0x18 );
	ExtendedMemory += inp(0x71) << 8;
	printf("Extended memory = %dK\n", ExtendedMemory);
}

void DumpControlRegisters()
{
	DWORD mcr0, mcr2, mcr3;

	_asm {
		mov eax, cr0
		mov mcr0, eax;
	}
	_asm {
		mov eax, cr2
		mov mcr2, eax;
	}
	_asm {
		mov eax, cr3
		mov mcr3, eax;
	}

	printf("CR0             = %x\n", mcr0);
	printf("CR2             = %x\n", mcr2);
	printf("CR3             = %x\n", mcr3);
}



void cfunc()   
{
	DumpBaseMemory();
	DumpExtendedMemory();
	DumpControlRegisters();
}

/* Declare the function present in RING0.ASM */
void func(void);

main()
{
	WORD CallGateSelector;
	int rc;
	short farcall[3];

	__try {
		cfunc();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("Direct port I/O and CPU control registers access without callgate raised exception!!\n");
	}

	printf("Now, performing direct port I/O and Control register access using callgates..\n\n");

	/* Create a callgate for function 'func' which takes '3' parameters 
	and get the callgate selector value in 'CallGateSelector'*/
	rc=CreateCallGate(func, 0, &CallGateSelector);

	/* Check if callgate creation succeeds */
	if (rc==SUCCESS) {
		/*Prepare for making the far call. Forget about the offset 
		portion of far call, so no need to think about first two
		elements of farcall array */
		farcall[2]=CallGateSelector;
        _asm {
				/*Make a far call*/
                call fword ptr [farcall]
        }

		/* Release the callgate created using CreateCallGate*/
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
