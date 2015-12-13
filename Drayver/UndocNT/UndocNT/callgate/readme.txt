This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) and 
Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Install and Start CALLGATE.SYS using the following command

   INSTDRV callgate c:\undocnt\bin\callgate.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. Run CGATEAPP.EXE. This will call wrapper functions in 
   CGATEDLL.DLL to create callgate. CGATEDLL.DLL will in
   turn call down to CALLGATE.SYS

3. CGATEAPP.EXE will execute functions at ring 0 using the callgate
   created in step 2.

4. The function executed at ring 0 does direct port I/O to get
   the base and extended memory specified on CMOS chip and will
   also display the contents of CPU control registers.

   The base memory, extended memory and the contents of CPU control
   registers will be shown on screen.

5. When you are done, you may unload the CALLGATE driver using the
   following commmand

   INSTDRV callgate remove
