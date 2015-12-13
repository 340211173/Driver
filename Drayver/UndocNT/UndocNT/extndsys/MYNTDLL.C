#include <windows.h>
#include <stdio.h>

#include <winioctl.h>
#include "extnddrv.h"

typedef int NTSTATUS;

int ServiceStart;

__declspec(dllexport) NTSTATUS SampleService0(void)
{
	_asm {
		mov eax, ServiceStart
		int 2eh
	}
}

__declspec(dllexport) NTSTATUS SampleService1(int param)
{
	void **stackframe=(void **)&param;

	_asm {
		mov eax, ServiceStart
		add eax, 1
		mov edx, stackframe
		int 2eh
	}
}

__declspec(dllexport) NTSTATUS SampleService2(int param1, int param2)
{
	void **stackframe=(void **)&param1;

	_asm {
		mov eax, ServiceStart
		add eax, 2
		mov edx, stackframe
		int 2eh
	}
}

BOOL SetStartingServiceId()
{
    HANDLE   hDevice;
    BOOL     ret;

    hDevice = CreateFile ("\\\\.\\extnddrv",
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL
                          );

    if (hDevice == ((HANDLE)-1))
    {
		MessageBox(0, "Unable to open handle to driver", "Error", MB_OK);
        ret = FALSE;
    }
    else
    {
		DWORD BytesReturned;
		ret=DeviceIoControl(hDevice,
							(DWORD)IOCTL_EXTNDDRV_GET_STARTING_SERVICEID,
							NULL,
							0,
							&ServiceStart,
							sizeof(ServiceStart),
							&BytesReturned,
							NULL);
							
		if (ret) {
			if (BytesReturned!=sizeof(ServiceStart)) {
				MessageBox(0, "DeviceIoControl failed", "Error", MB_OK);
				ret=FALSE;
			} else {
				ret = TRUE;
			}
		} else {
			MessageBox(0, "DeviceIoControl failed", "Error", MB_OK);
		}
        CloseHandle (hDevice);
    }
    return ret;
}



BOOL WINAPI DllMain(HANDLE hModule, DWORD Reason, LPVOID lpReserved)
{
    switch (Reason) {
    case DLL_PROCESS_ATTACH:
        //
        // We're being loaded - save our handle
        //
		return SetStartingServiceId();
    default:
        return TRUE;
    }

}
