# await
 
Non blocking ( asynchronous ) event waiter

A C++ class that looks after setting up non-blocking waits for events to occur and the code to run ( event handlers ) when the events occur.

No synchronization is required, all the event handlers run in sequence in the same thread.

No libraries are required, everything is in a single standard C++17 include file `await/include/await.h`

## general usage

 - define blocking function with signature `void f()`
 - define event handler funtion with signature `void f()`
 - construct instance of cAwait
 - pass blocking and event handler functions using cAwait() method
 - start event manager by calling blocking method cAwait::run()

## Sample code

Code for demo applications is in the `src` folder.  They can be built using the makefile in folder `build/vscode`

 - `timertest.cpp` creates multiple timers that run concurrently
 - `cintest.cpp` demonstrates non-blocking keyboard input
 - `tcpserver.cpp` a simple TCP server that displays message received from a client
 - `tcpclient.cpp` a simple TCP client that sends message to a server as they are typed
