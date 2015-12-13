This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) 
and Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Install and Start HOOKINT.SYS using the following command

   INSTDRV hookint c:\undocnt\bin\hookint.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. The driver will hook interrupt 0x2Eh (system service interrupt)
   and will start maintaining the counter for number of times 
   each system service is called.
  
3. Run HOOKINTAPP.EXE. This application will gather the service
   counters from HOOKINT.SYS and print them.

4. You may run HOOKINTAPP.EXE as many times you want. The counters
   will be seen to increase as the time passes by.

5. When you are done, stop and unload HOOKINT.SYS using the following
   command.

   INSTDRV hookint remove
