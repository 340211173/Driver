#include <windows.h>
#include <stdio.h>
#include "addint.h"

main()
{
	char buffer[100];

	__try {
		_asm {
			lea edx, buffer
			int 22h
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("Exception occured, make sure that addint.sys is installed and started\n");
		return 0;
	}

	printf("Buffer filled by the interrupt handler = %s\n", buffer);
	return 0;
}
