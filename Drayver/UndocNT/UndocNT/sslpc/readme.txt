This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) 
and Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Run "SSLPC.EXE" without any command line parameters. This
   will start the executable in server mode.

2. Run "SSLPC.EXE" with some parameter. Any junk parameter will
   do. e.g "SSLPC abcd". This will start the executable in client
   mode and will connect to the port created by server.

3. The client will ask for the data string to be sent to the server. 
   It will also ask where in shared memory, you need to keep this
   data.

4. The client will fill in the data string in the shared buffer and
   will pass an LPC message to the server. This LPC message will
   also contain the offet of the data in shared memory.

5. The server will dump the received LPC message and will string
   revert the data passed in shared memory and pass it back to the
   client.

6. The client will dump the reply.

7. Steps 3,4,5,6 are repeated till the user types "quit" as the 
   data string.
