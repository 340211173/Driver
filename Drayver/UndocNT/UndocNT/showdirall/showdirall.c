#include <windows.h>
#include <stdio.h>
#include <winioctl.h>

#include "pagedirdrv.h"

main()
{
    char     completeDeviceName[64] = "";
    HANDLE   hDevice;
    BOOL     ret;

    strcpy (completeDeviceName,
            "\\\\.\\pagedirdrv"
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
		printf("Unable to open device driver, make sure that pagedirdrv.sys is installed\n");
		return 0;
    }	
    else
    {
        DWORD BytesReturned;

        ret=DeviceIoControl(hDevice, 
						(DWORD)IOCTL_PAGEDIRDRV_DUMPPAGEDIR,
                        NULL, 
						0, 
						NULL, 
						0,
                        &BytesReturned, 
						NULL);
		if (ret==FALSE) {
			printf("DeviceIoControl failed, rc=%d\n", GetLastError());
		} else {
			printf("Page directories for all the processes are dumped, Check the output in debugger window\n");
		}
        CloseHandle (hDevice);
    }
    return ret;
}
