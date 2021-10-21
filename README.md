# await
 
Non blocking ( asynchronous ) event waiter

A C++ class that looks after setting up non-blocking waits for events to occur and the code to run ( event handlers ) when the events occur.

No libraries are required, everything is in a single standard C++17 include file `await/include/await.h`

## Events

 - keyboard input
 - tcp connection request from tcp client
 - tcp connection accept from tcp server
 - tcp read from peer

## general usage

 - define event handler funtion
 - construct instance of cAwait
 - pass event handler function using relevent asynchronous cAwait method
 - start event manager by calling blocking method cAwait::run()

## Sample code

Code from demo application is in the `src` folder.  They can be built using the makefile in folder `build/vscode`

 - `cintest.cpp` demonstrates non-blocking keyboard input
 - `tcpserver.cpp` a simple TCP server that displays message received from a client
 - `tcpclient.cpp` a simple TCP client that sends message to a server as they are typed
