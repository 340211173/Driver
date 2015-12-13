This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) and 
Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Install and Start HOOKSYS.SYS using the following command

   INSTDRV hooksys c:\undocnt\bin\hooksys.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. The driver will hook NtCreateFile system service it its
   DriverEntry routine.

3. Perform file system operations such as "Type a file". The
   driver will trap the call and display the file accessed and
   the return code from the NtCreateFile system service.

4. The output from the driver will be shown in debugger window.
   This output can be seen either by using a kernel mode debugger
   such as SoftICE or using a tools like DBGMON on 
   http://www.sysinternals.com site.
