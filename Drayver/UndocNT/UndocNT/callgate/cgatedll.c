#include <windows.h>
#include <winioctl.h>

#include "callgate.h"
#include "gate.h"

HANDLE hCallgateDriver=INVALID_HANDLE_VALUE;
WORD CodeSelectorArray[8192];


void OpenCallgateDriver()
{
    char     completeDeviceName[64] = "";

    strcpy (completeDeviceName,
            "\\\\.\\callgate"
            );


    hCallgateDriver = CreateFile (completeDeviceName,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL
                          );


}

void CloseCallgateDriver()
{
	if (hCallgateDriver!=INVALID_HANDLE_VALUE) {
		CloseHandle(hCallgateDriver);
	}
}


int WINAPI CreateCallGate(void *FunctionAddress,
										int NumberOfParameters,
										PWORD pSelector)
{
	CallGateInfo_t CallGateInfo;
	DWORD BytesReturned;

	if (hCallgateDriver==INVALID_HANDLE_VALUE) {
		return ERROR_DRIVER_NOT_FOUND;
	}

	if (!pSelector)
		return ERROR_BAD_PARAMETER;
	
	memset(&CallGateInfo, 0, sizeof(CallGateInfo));
    CallGateInfo.FunctionLinearAddress=FunctionAddress;
    CallGateInfo.NumberOfParameters=NumberOfParameters;

    if (!DeviceIoControl(hCallgateDriver,
                    (DWORD)IOCTL_CALLGATE_CREATE,
                    &CallGateInfo,
                    sizeof(CallGateInfo),
                    &CallGateInfo,
                    sizeof(CallGateInfo),
                    &BytesReturned,
                    NULL)) {
		return ERROR_IOCONTROL_FAILED;
	}
	*pSelector=CallGateInfo.CallGateSelector;
	CodeSelectorArray[CallGateInfo.CallGateSelector]=CallGateInfo.CodeSelector;
	return SUCCESS;
}

int WINAPI FreeCallGate(WORD CallGateSelector)
{
	CallGateInfo_t CallGateInfo;
	DWORD BytesReturned;

	if (hCallgateDriver==INVALID_HANDLE_VALUE) {
		return ERROR_DRIVER_NOT_FOUND;
	}
	if (CallGateSelector>=sizeof(CodeSelectorArray)/sizeof(CodeSelectorArray[0])) {
		return ERROR_BAD_PARAMETER;
	}

	
	memset(&CallGateInfo, 0, sizeof(CallGateInfo));
    CallGateInfo.CallGateSelector=CallGateSelector;
    CallGateInfo.CodeSelector=CodeSelectorArray[CallGateSelector];

    if (!DeviceIoControl(hCallgateDriver,
                    (DWORD)IOCTL_CALLGATE_RELEASE,
                    &CallGateInfo,
                    sizeof(CallGateInfo),
                    &CallGateInfo,
                    sizeof(CallGateInfo),
                    &BytesReturned,
                    NULL)) {
		return ERROR_IOCONTROL_FAILED;
	}
	return SUCCESS;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD Reason, LPVOID lpReserved)
{
    switch (Reason) {

    case DLL_PROCESS_ATTACH:
		OpenCallgateDriver();
      	return	TRUE;
	case DLL_PROCESS_DETACH:
		CloseCallgateDriver();
		return TRUE;
	default:
		return TRUE;
	}
}
