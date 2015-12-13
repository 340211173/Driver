// StartDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "psapi.h"
#include "ioctl.h"
#define DRIVERNAME    _T("DriverExample")
#define DRIVERBINARY  _T("C:\\Documents and Settings\\Администратор\\Мои документы\\Visual Studio 2010\\Projects\\SuperDriver\\Debug\\SuperDriver.sys")

// Функция установки драйвера на основе SCM вызовов
BOOL InstallDriver(SC_HANDLE  scm, LPCTSTR DriverName, LPCTSTR driverExec)
{
	SC_HANDLE Service =
		CreateService(scm,    // открытый дескриптор к SCManager
		DriverName,      // имя сервиса - Example
		DriverName,      // для вывода на экран
		SERVICE_ALL_ACCESS,    // желаемый доступ
		SERVICE_KERNEL_DRIVER, // тип сервиса
		SERVICE_DEMAND_START,  // тип запуска
		SERVICE_ERROR_NORMAL,  // как обрабатывается ошибка
		driverExec,            // путь к бинарному файлу
		// Остальные параметры не используются - укажем NULL
		NULL,    // Не определяем группу загрузки
		NULL, NULL, NULL, NULL);
	if (Service == NULL) // неудача
	{
		DWORD err = GetLastError();
		if (err == ERROR_SERVICE_EXISTS) {/* уже установлен */ }
		// более серьезная ощибка:
		else  printf("ERR: CanТt create service. Err=%d\n", err);
		// (^^ Ётот код ошибки можно подставить в ErrLook):
		return FALSE;
	}
	CloseServiceHandle(Service);
	return TRUE;
}

// Функция удаления драйвера на основе SCM вызовов
BOOL RemoveDriver(SC_HANDLE scm, LPCTSTR DriverName)
{
	SC_HANDLE Service =
		OpenService(scm, DriverName, SERVICE_ALL_ACCESS);
	if (Service == NULL) return FALSE;
	BOOL ret = DeleteService(Service);
	if (!ret) { /* неудача при удалении драйвера */ }

	CloseServiceHandle(Service);
	return ret;
}

// Функция запуска драйвера на основе SCM вызовов
BOOL StartDriver(SC_HANDLE  scm, LPCTSTR DriverName)
{
	SC_HANDLE Service =
		OpenService(scm, DriverName, SERVICE_ALL_ACCESS);
	if (Service == NULL) return FALSE; /* open failed */
	BOOL ret =
		StartService(Service, // дескриптор
		0,       // число аргументов
		NULL); // указатель  на аргументы
	if (!ret) // неудача
	{
		DWORD err = GetLastError();
		if (err == ERROR_SERVICE_ALREADY_RUNNING)
			ret = TRUE; // OK, драйвер уже работает!
		else { /* другие проблемы */ }
	}

	CloseServiceHandle(Service);
	return ret;
}
// Функция останова драйвера на основе SCM вызовов
BOOL StopDriver(SC_HANDLE  scm, LPCTSTR DriverName)
{
	SC_HANDLE Service =
		OpenService(scm, DriverName, SERVICE_ALL_ACCESS);
	if (Service == NULL)  // Невозможно выполнить останов драйвера
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	SERVICE_STATUS serviceStatus;
	BOOL ret =
		ControlService(Service, SERVICE_CONTROL_STOP, &serviceStatus);
	if (!ret)
	{
		DWORD err = GetLastError();
		// дополнительная диагностика
	}

	CloseServiceHandle(Service);
	return ret;
}


// Соберем вместе действия по установке, запуску, останову
// и удалению драйвера (для обобщения сведений).
// (Однако пользоваться этой функцией в данном примере не придется.)
/* Закомментируем ее.
void Test_SCM_Installation(void)
{
SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
if(scm == NULL) // неудача
{
// Получаем код ошибки и ее текстовый эквивалент
unsigned long err = GetLastError();
PrintErrorMessage(err); // см. п. 2.1.5
return;
}
BOOL res;
res = InstallDriver(scm, DRIVERNAME, DRIVERBINARY );
// Ошибка может оказаться не фатальной. Продолжаем:
res = StartDriver  (scm, DRIVERNAME );
if(res)
{
//Е Здесь следует разместить функции работы с драйвером
.. .. ..
res = StopDriver   (scm, DRIVERNAME );
if(res) res = RemoveDriver (scm, DRIVERNAME );
}
CloseServiceHandle(scm);
return;
}
*/

#define SCM_SERVICE
// ^^^^^^^^^^^^^^^^ вводим элемент условной компиляции, при помощи
// которого можно отключать использование SCM установки драйвера
// в тексте данного приложения. (Здесь Ц использование SCM включено.)

// Основная функция тестирующего приложения.
// Здесь минимум внимания уделен диагностике ошибочных ситуаций.
// В действительно рабочих приложениях следует уделить этому
// больше внимания!

int __cdecl main(int argc, char* argv[])
{
#ifdef SCM_SERVICE
	// Используем сервис SCM для запуска драйвера.
	BOOL res; // Получаем доступ к SCM :
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scm == NULL) return -1; // неудача

	// Делаем попытку установки драйвера
	res = InstallDriver(scm, DRIVERNAME, DRIVERBINARY);
	if (!res) // Неудача, но возможно, он уже инсталлирован
		printf("Cannot install service");

	res = StartDriver(scm, DRIVERNAME);
	if (!res)
	{
		printf("Cannot start driver!");
		res = RemoveDriver(scm, DRIVERNAME);
		if (!res)
		{
			printf("Cannot remove driver!");
		}
		CloseServiceHandle(scm); // Отключаемся от SCM
		return -1;
	}
#endif

	HANDLE hHandle =           // Получаем доступ к драйверу
		CreateFile(L"\\\\.\\Example",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hHandle == INVALID_HANDLE_VALUE)
	{
		printf("ERR: can not access driver Example.sys !\n");
		return (-1);
	}
	DWORD BytesReturned;    // Переменная для хранения числа
	// переданных байт
	// Последовательно выполняем обращения к драйверу
	// с различными кодами IOCTL:

	unsigned long ioctlCode = IOCTL_PRINT_DEBUG_MESS;
	if (!DeviceIoControl(hHandle,
		ioctlCode,
		NULL, 0,	// Input
		NULL, 0,	// Output
		&BytesReturned,
		NULL))
	{
		printf("Error in IOCTL_PRINT_DEBUG_MESS!");
		return(-1);
	}

	ioctlCode = IOCTL_CHANGE_IRQL;
	if (!DeviceIoControl(hHandle,
		ioctlCode,
		NULL, 0,	// Input
		NULL, 0,	// Output
		&BytesReturned,
		NULL))
	{
		printf("Error in IOCTL_CHANGE_IRQL!");
		return(-1);
	}

	// Следующий тест. Получаем 1 байт данных из драйвера.
	// По окончании данного вызова переменная xdata должна
	// содержать значение 33:
	unsigned char xdata = 0x88;
	ioctlCode = IOCTL_SEND_BYTE_TO_USER;
	if (!DeviceIoControl(hHandle,
		ioctlCode,
		NULL, 0,  // Input
		&xdata, sizeof(xdata),// Output
		&BytesReturned,
		NULL))
	{
		printf("Error in IOCTL_SEND_BYTE_TO_USER!");
		return(-1);
	}

	// Вывод диагностического сообщения в консольном окне:
	printf("IOCTL_SEND_BYTE_TO_USER: BytesReturned=%d xdata=%d",
		BytesReturned, xdata);

	// Выполнение следующего теста в Windows NT приведет к
	// фатальному сбою операционной системы (намеренно выполненное
	// падение ОС может быть полезно при изучении, например,
	// организации crash dump файла и работы с отладчиком).
	/*
	ioctlCode=IOCTL_MAKE_SYSTEM_CRASH;
	if( !DeviceIoControl(   hHandle,
	ioctlCode,
	NULL, 0,		        // Input
	NULL, 0,	// Output
	&BytesReturned,
	NULL )  )
	{
	printf( "Error in IOCTL_MAKE_SYSTEM_CRASH!" );
	return(-1);
	}
	*/
	// Закрываем дескриптор доступа к драйверу:
	CloseHandle(hHandle);


#ifdef SCM_SERVICE
	// Останавливаем и удаляем драйвер. Отключаемся от SCM.
	res = StopDriver(scm, DRIVERNAME);
	if (!res)
	{
		printf("Cannot stop driver!");
		CloseServiceHandle(scm);
		return -1;

	}

	res = RemoveDriver(scm, DRIVERNAME);
	if (!res)
	{
		printf("Cannot remove driver!");
		CloseServiceHandle(scm);
		return -1;
	}

	CloseServiceHandle(scm);
#endif

	return 0;
}