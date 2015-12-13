typedef int NTSTATUS;
__declspec(dllexport) NTSTATUS SampleService0(void);
__declspec(dllexport) NTSTATUS SampleService1(int param);
__declspec(dllexport) NTSTATUS SampleService2(int param1, int param2);
__declspec(dllexport) NTSTATUS SampleService3(int param1, int param2,
				   int param3);

__declspec(dllexport) NTSTATUS SampleService4(int param1, int param2,
				   int param3, int param4);
__declspec(dllexport) NTSTATUS SampleService5(int param1, int param2,
				   int param3, int param4, 
				   int param5);
__declspec(dllexport) NTSTATUS SampleService6(int param1, int param2,
				   int param3, int param4, 
				   int param5, int param6);
