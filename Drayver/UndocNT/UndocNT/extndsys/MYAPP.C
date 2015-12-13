/* This is a sample console application which calls the newly
added services. The services are called through a wrapper DLL 
The application simply prints the return values from the newly
added services */

#include <windows.h>
#include <stdio.h>


#include "myntdll.h"

main()
{
	printf("SampleService0 returned = %x\n", SampleService0());
	printf("SampleService1 returned = %x\n", SampleService1(0x10));
	printf("SampleService2 returned = %x\n", SampleService2(0x10, 0x20));

	return 0;
}
