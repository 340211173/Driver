This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) and 
Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Install and Start EXTNDDRV.SYS using the following command

   INSTDRV extnddrv c:\undocnt\bin\extnddrv.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. The driver will add three new system services in DriverEntry 
   routine.

3. Run MYAPP.EXE. This will call the wrappers in MYNTDLL.DLL
   which will ultimately call down to services added in 
   EXTNDDRV.SYS. This is identical to path taken by some of
   the KERNEL32 APIs. i.e KERNEL32->NTDLL->NTOSKRNL.

4. The new system services in EXTNDDRV.SYS will print the 
   parameters passed to them in Debugger Window.

5. The output from the driver will be shown in debugger window.
   This output can be seen either by using a kernel mode debugger
   such as SoftICE or using a tools like DBGMON on 
   http://www.sysinternals.com site.

6. MYAPP.EXE will print the return values of the new system 
   services in EXTNDDRV.SYS
