This code includes a sample client and server. The client is the same
as the demo display, which draws a random card when clicking. However,
rather than generate a random number locally, it uses XML-RPC to 
execute the random function on a separate server process. 

The server shows how to setup XML-RPC to execute remote functions and
return the output. The server also has an example of using sqlite3
to create, insert, update, and delete from a database.

To build, run:

$ ./compile

To run, in one terminal run:

$ ./server

In another terminal, run:

$ ./client

The client will generate a display.
