This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) and 
Windows 2000 beta2.

Steps to try out the sample
===========================

1. Make sure that CALLGATE.SYS is installed and started. 

   If the driver is not installed or not started, it can be installed 
   and started using the following command.

   INSTDRV CALLGATE c:\undocnt\bin\callgate.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. Run VADDUMP.EXE. It will display its own VAD tree. After this, it
   will allocate a piece of memory using the VirtualAlloc call and then
   it will again dump its VAD tree. The VAD entry for the memory allocated
   using VirtualAlloc will be shown in this dump.

3. You may redirect the output of VADDUMP.EXE to some file and then see
   the dumps.
