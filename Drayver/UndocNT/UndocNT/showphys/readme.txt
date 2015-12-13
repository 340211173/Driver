This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) and 
Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Make sure that CALLGATE.SYS is installed and started. 

   If the driver is not installed or not started, it can be installed 
   and started using the following command.

   INSTDRV CALLGATE c:\undocnt\bin\callgate.sys

   (Assuming the samples in the book are copied in c:\undocnt
   directory)

2. Run SHOWPHYS.EXE. It will display the physical addresses for some
   of the components in MYDLL.DLL loaded in its own address space and
   will wait for a keystroke.

3. Run another instance of SHOWPHYS.EXE. It will rearrange the loading
   of MYDLL.DLL so that, MYDLL.DLL won't load at preferred base address
   and will show the physical addresses for some of the components
   in MYDLL.DLL. Next, it will arrange to load MYDLL.DLL at the preferred
   base address and again show the physical addresses for the various
   components. Next, it will try to touch some code/data in MYDLL.DLL in
   its own adderss space and again dump the physical addresses.

4. You may redirect the output of second instance of SHOWPHYS.EXE to some 
   file and then compare it with the output shown in first instance.
   More details about this can be found in Chapter 4.
