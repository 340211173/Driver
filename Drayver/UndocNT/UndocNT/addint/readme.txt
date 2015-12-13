This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) 
and Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Install and Start ADDINT.SYS using the following command

   INSTDRV addint c:\undocnt\bin\addint.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. The driver will add a new interrupt 22h to the kernel.
  
3. Run ADDINTAPP.EXE. This application will issue "int 22h" 
   instruction to call the handler in ADDINT.SYS

4. The handler will fill in the message string in the buffer
   pointed to by EDX register.

5. ADDINTAPP.EXE will display this message. 

6. When you are done, stop and unload ADDINT.SYS using the 
   following command.

   INSTDRV addint remove
