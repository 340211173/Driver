This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) and 
Windows 2000 beta2.

Steps to try out the sample
===========================

1. Make sure that PAGEDIRDRV.SYS is installed and started. 

   If the driver is not installed or not started, it can be installed 
   and started using the following command.

   INSTDRV pagedirdrv c:\undocnt\bin\pagedirdrv.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. Run SHOWDIRALL.EXE. It will display its own page directories for 
   all the processes running in the system.

3. The output will be shown in the debugger window using the DbgPrint
   calls.

4. The output can be seen either from a kernel debugger such as SoftICE 
   or using a tool such as DBGMON on http:\\www.sysinternals.com
