This sample has been tested on Windows NT 3.51(SP5), 4.0 (SP5) and 
Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Make sure that CALLGATE.SYS is installed and started. 

   If the driver is not installed or not started, it can be installed 
   and started using the following command.

   INSTDRV CALLGATE c:\undocnt\bin\callgate.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. Run SHOWDIR.EXE. It will display its own page directory and wait
   for the keystroke.

3. Run another instance of SHOWDIR.EXE

4. You may redirect the output from first and second instance SHOWDIR.EXE
   to two different files.

5. Look for the differences in these two files. You will observe that two
   entries in upper 2GB address space are different for each instance.
   More information about this behaviour can be found in Chapter 4.


