#include <windows.h>
#include <stdio.h>

#include <winioctl.h>
#include "hookint.h"

#define MAX_SERVICES	0x1000


void PrintServiceUsage(HANDLE DeviceHandle)
{
	DWORD BytesReturned;
	BOOL rc;
	DWORD NumberOfServices;
	DWORD i;
	DWORD ServiceCounters[MAX_SERVICES];


	rc=DeviceIoControl(DeviceHandle,
					(DWORD)IOCTL_HOOKINT_SYSTEM_SERVICE_USAGE,
					NULL,
					0,
					ServiceCounters,
					sizeof(ServiceCounters),
					&BytesReturned,
					NULL);
	if (!rc) {
		printf("DeviceIoControl failed, rc=%d\n", GetLastError());
		return;
	}
	NumberOfServices=ServiceCounters[0];

	printf("Service Id    Number of times service called\n");
	for (i=0; i<NumberOfServices; i++) {
		printf("%03x           %08x\n", i, ServiceCounters[i+1]);
	}
}



main()
{
    char     completeDeviceName[64] = "";
    HANDLE   hDevice;

    strcpy (completeDeviceName,
            "\\\\.\\hookint"
            );


    hDevice = CreateFile (completeDeviceName,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL
                          );

    if (hDevice == ((HANDLE)-1))
    {
		printf("Unable to open device driver, make sure that hookint.sys is installed\n");
		return 0;
    }	
    else
    {
		PrintServiceUsage(hDevice);
        CloseHandle (hDevice);
    }
    return 0;
}
