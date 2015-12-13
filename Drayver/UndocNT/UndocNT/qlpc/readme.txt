This sample will work only on Windows NT 3.51

Steps to try out the sample
===========================

1. Run "QLPC.EXE" without any command line parameters. This
   will start the executable in server mode and will ask
   for the client's process id and thread id.

2. Run "QLPC.EXE" with some parameter. Any junk parameter will
   do. e.g "QLPC abcd". This will start the executable in client
   mode and will display the process id and thread id of the
   client and will wait for a keystroke.

3. Switch to the server's window and enter the process id
   and thread id obtained in previous step.

4. After this switch to the client's window and press "enter" key.

5. Both the server and client will start issuing 2Bh and 2Ch 
   interrupts. And you will see the controlled context switching
   in action. Just like CSRSS does it for USER and GDI calls in
   Windows NT 3.51.
