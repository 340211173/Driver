#include <windows.h>

int GlobalVar=100;

#pragma code_seg("MyCodeSeg1")
_declspec(dllexport) int WINAPI NonRelocatableFunction()
{
	int x,y,z;

	x=y=z=0;

	return x;
}
#pragma code_seg()

#pragma code_seg("MyCodeSeg2")
_declspec(dllexport) int WINAPI  RelocatableFunction()
{
	GlobalVar=1000;
}
#pragma code_seg()

#pragma data_seg("SharedDataSeg")
_declspec(dllexport) int SharedVariable=0;
#pragma data_seg()

#pragma data_seg("NonSharedDataSeg")
_declspec(dllexport) char NonSharedVariable[4096]="Hello";
#pragma data_seg()

BOOL WINAPI DllMain (
					 HANDLE    hModule,
					 DWORD     dwFunction,
					 LPVOID    lpNot)
{
	return TRUE;
}
