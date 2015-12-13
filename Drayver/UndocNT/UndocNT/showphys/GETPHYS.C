#include <windows.h>
#include <stdio.h>
#include "gate.h"

static short CallGateSelector;
char TouchVariable;

BOOL _GetPhysicalAddressAndPageAttributes(unsigned int VirtualAddress,
			unsigned int *PhysicalAddress, unsigned int *PageAttributes);

/* C function called from assembly langauage stub */
BOOL _stdcall CFuncGetPhysicalAddressAndPageAttributes(unsigned int VirtualAddress,
			unsigned int *PhysicalAddress, unsigned int *PageAttributes)
{
	unsigned int *PageTableEntry;

	*PhysicalAddress=0;
	*PageAttributes=0;

	PageTableEntry=(unsigned int *)(0xC0000000U+((VirtualAddress&0xFFFFF3FFU)>>0x0AU));
	if ((*PageTableEntry)&0x01) {
		*PhysicalAddress=((*PageTableEntry)&0xFFFFF000U)+(VirtualAddress&0x00000FFFU);
		*PageAttributes=(*PageTableEntry)&0x00000FFFU;
		return TRUE;
	} else {
		return FALSE;
	}
}


BOOL GetPhysicalAddressAndPageAttributes(void *VirtualAddress,
			unsigned int *PhysicalAddress, unsigned int *PageAttributes)
{
	BOOL rc;
	short farcall[3];

	if (!CallGateSelector) {
		return FALSE;
	}

	TouchVariable=*(char *)VirtualAddress;
	farcall[2]=CallGateSelector;
	
	_asm {
			push PageAttributes
			push PhysicalAddress
			push VirtualAddress
			call fword ptr [farcall]
			mov rc, eax
	}
	return rc;
}

int CreateRing0CallGate()
{
	DWORD rc;

	if (VirtualLock(_GetPhysicalAddressAndPageAttributes, 4096)==FALSE)
		return -1;
	if (VirtualLock(CFuncGetPhysicalAddressAndPageAttributes, 4096)==FALSE)
		return -1;

	VirtualLock(CFuncGetPhysicalAddressAndPageAttributes, 4096);

	rc=CreateCallGate(_GetPhysicalAddressAndPageAttributes, 3, &CallGateSelector);
	return rc;
}

int FreeRing0CallGate()
{
	DWORD rc;

	VirtualUnlock(_GetPhysicalAddressAndPageAttributes, 4096);
	VirtualUnlock(CFuncGetPhysicalAddressAndPageAttributes, 4096);

	rc=FreeCallGate(CallGateSelector);
	if (rc==SUCCESS) {
		CallGateSelector=0;
	}
	return rc;
}
