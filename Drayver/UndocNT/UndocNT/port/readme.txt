This sample has been tested on Windows NT 3.51(SP5), 4.0(SP5) 
and Windows 2000 beta1 and beta2.

Steps to try out the sample
===========================

1. Run "PORT.EXE" without any command line parameters. This
   will start the executable in server mode.

2. Run "PORT.EXE" with some parameter. Any junk parameter will
   do. e.g "PORT abcd". This will start the executable in client
   mode and will connect to the port created by server.

3. The client will send two values to the server in an LPC message 
   and will wait for the reply.

4. The server, upon receiving the data from the client will do a
   bitwise not on the data passed and send reply to the client.
   Server will also dump the received LPC message.

5. The client upon receiving the reply will print these values.

6. The client will ask if user needs to stop sending data.
   If the user selecs "N", then steps 3,4,5,6 will be repeated.
   If the user selects "Y", the client program returns.
