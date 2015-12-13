#include <windows.h>
#include <stdio.h>

#include "gate.h"
#include "getphys.h"

HANDLE hFileMapping;

/* Imported function/variable addresses */
static void *NonRelocatableFunction=NULL;
static void *RelocatableFunction=NULL;
static void *SharedVariable=NULL;
static void *NonSharedVariable=NULL;

HINSTANCE hDllInstance;

/* Loads MYDLL.DLL and initializes addresses of imported functions/
variables from MYDLL.DLL and locks the imported areas*/
int LoadDllAndInitializeVirtualAddresses()
{
	hDllInstance=LoadLibrary("MYDLL.DLL");
	if (hDllInstance==NULL) {
		printf("Unable to load MYDLL.DLL\n");
		return -1;
	}
	printf("MYDLL.DLL loaded at base address = %x\n", hDllInstance);

	NonRelocatableFunction=GetProcAddress(GetModuleHandle("MYDLL"),
							"_NonRelocatableFunction@0");
	RelocatableFunction=GetProcAddress(GetModuleHandle("MYDLL"),
							"_RelocatableFunction@0");
	SharedVariable=GetProcAddress(GetModuleHandle("MYDLL"),
							"SharedVariable");
	NonSharedVariable=GetProcAddress(GetModuleHandle("MYDLL"),
							"NonSharedVariable");
	if ((!NonRelocatableFunction)||(!RelocatableFunction)||(!SharedVariable)
			||(!NonSharedVariable)) {
		printf("Unable to get the virtual addresses for imports from MYDLL.DLL\n");
		FreeLibrary(hDllInstance);
		hDllInstance=0;
		return -1;
	}

	VirtualLock(NonRelocatableFunction, 1);
	VirtualLock(RelocatableFunction, 1);
	VirtualLock(SharedVariable, 1);
	VirtualLock(NonSharedVariable, 1);
	
	return 0;
}

/* Unlocks the imported areas and frees the MYDLL.DLL */
void FreeDll()
{
	VirtualUnlock(NonRelocatableFunction, 1);
	VirtualUnlock(RelocatableFunction, 1);
	VirtualUnlock(SharedVariable, 1);
	VirtualUnlock(NonSharedVariable, 1);

	FreeLibrary(hDllInstance);
	hDllInstance=0;
	NonRelocatableFunction=NULL;
	RelocatableFunction=NULL;
	SharedVariable=NULL;
	NonSharedVariable=NULL;
}

/* Converts the page attributes in readable form */
char *GetPageAttributesString(unsigned int PageAttr)
{
	static char buffer[100];

	strcpy(buffer, "");

	if (PageAttr&0x01) {
		strcat(buffer, "P  ");	
	} else {
		strcat(buffer, "NP ");	
	}

	if (PageAttr&0x02) {
		strcat(buffer, "RW ");	
	} else {
		strcat(buffer, "R  ");	
	}

	if (PageAttr&0x04) {
		strcat(buffer, "U ");	
	} else {
		strcat(buffer, "S ");	
	}

	if (PageAttr&0x40) {
		strcat(buffer, "D ");	
	} else {
		strcat(buffer, "  ");	
	}
	return buffer;
}


/* Displays virtual to physical address mapping */
int DisplayVirtualAndPhysicalAddresses()
{
	DWORD pNonRelocatableFunction=0;
	DWORD pRelocatableFunction=0;
	DWORD pSharedVariable=0;	
	DWORD pNonSharedVariable=0;	

	DWORD aNonRelocatableFunction=0;
	DWORD aRelocatableFunction=0;
	DWORD aSharedVariable=0;	
	DWORD aNonSharedVariable=0;	

	DWORD pFileMappingView=0;	
	DWORD aFileMappingView=0;

	printf("\nVirtual address to Physical address mapping\n");
	printf("\n-----------------------------------------------------------------------\n");
	printf("Variable/function	Virtual		Physical	Page Attributes\n");
	printf("                 	Address		Address\n");
	printf("-----------------------------------------------------------------------\n");

	GetPhysicalAddressAndPageAttributes(NonRelocatableFunction,
						&pNonRelocatableFunction, &aNonRelocatableFunction);
	GetPhysicalAddressAndPageAttributes(RelocatableFunction,
						&pRelocatableFunction, &aRelocatableFunction);
	GetPhysicalAddressAndPageAttributes(SharedVariable,
						&pSharedVariable, &aSharedVariable);
	GetPhysicalAddressAndPageAttributes(NonSharedVariable,
						&pNonSharedVariable, &aNonSharedVariable);


	printf("NonRelocatableFunction	%8x	%8x		%s\n",
				NonRelocatableFunction, pNonRelocatableFunction,
				GetPageAttributesString(aNonRelocatableFunction));
	printf("RelocatableFunction	%8x	%8x		%s\n",
				RelocatableFunction, pRelocatableFunction,
				GetPageAttributesString(aRelocatableFunction));
	printf("SharedVariable		%8x	%8x		%s\n",
				SharedVariable, pSharedVariable,
				GetPageAttributesString(aSharedVariable));
	printf("NonSharedVariable	%8x	%8x		%s\n",
				NonSharedVariable, pNonSharedVariable,
				GetPageAttributesString(aNonSharedVariable));
	printf("-----------------------------------------------------------------------\n\n");

	return 0;
}

int FirstInstance()
{
	printf("***This is the first instance of the showphys program***\n\n");
	printf("Loading DLL MYDLL.DLL\n");

	if (LoadDllAndInitializeVirtualAddresses()!=0) {
		return -1;
	}

	DisplayVirtualAndPhysicalAddresses();
	printf("Now Run showphys from another DOS Box ...\n");
	getchar();
	FreeDll();
}

int NonFirstInstance()
{
	DWORD OldAttr;
	HINSTANCE hJunk;

	printf("***This is another instance of the showphys program***\n\n");
	printf("Loading DLL MYDLL.DLL at diffrent base address than that of the first instance\n");

	CopyFile("MYDLL.DLL", "JUNK.DLL", FALSE);

	hJunk=LoadLibrary("JUNK.DLL");

	if (hJunk==NULL) {
		printf("Could not find JUNK.DLL\n");
		return -1;
	}

	if (LoadDllAndInitializeVirtualAddresses()!=0) {
		FreeLibrary(hJunk);
		return -1;
	}
	FreeLibrary(hJunk);

	DisplayVirtualAndPhysicalAddresses();

	FreeDll();

	printf("Loading DLL MYDLL.DLL at same base address as that of the first instance\n");

	if (LoadDllAndInitializeVirtualAddresses()!=0) {
		return -1;
	}

	DisplayVirtualAndPhysicalAddresses();

	printf("....Modifying the code bytes at the start of NonRelocatableFunction\n");
	VirtualProtect(NonRelocatableFunction, 1, PAGE_READWRITE, &OldAttr);
	*(unsigned char *)NonRelocatableFunction=0xE9;


	printf("....Modifying the value of SharedVariable\n");
	*(char *)SharedVariable=0x10;

	printf("....Modifying the NonSharedVariable's value\n\n");
	*(char *)NonSharedVariable=0x10;

	DisplayVirtualAndPhysicalAddresses();

	FreeDll();

	return 0;
}

int DecideTheInstanceAndAct()
{
	hFileMapping=CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE,
					0, 0x1000, "MyFileMapping");
	if (hFileMapping==NULL) {
		printf("Unable to create file mapping\n");
		FreeDll();
		return -1;
	}
	if (GetLastError()==ERROR_ALREADY_EXISTS) {
		NonFirstInstance();
	} else {
		FirstInstance();
	}
}

main()
{
	int rc;
	DWORD PhysicalAddress=0, PageAttributes=0;

	/* Creates callgate to get PTE entries from ring 3 app */
	if ((rc=CreateRing0CallGate())!=SUCCESS) {
		printf("Unable to create callgate, rc=%x\n", rc);
		return -1;
	}
	DecideTheInstanceAndAct();

	/* Releases the callgate */
	FreeRing0CallGate();
}
