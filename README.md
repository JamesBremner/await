# await
 
Non blocking ( asynchronous ) event waiter

A C++ class that looks after setting up non-blocking waits for events to occur and the code to run ( event handlers ) when the events occur. [Usage Details](https://github.com/JamesBremner/await/wiki/Usage)

No synchronization is usually required. [details](https://github.com/JamesBremner/await/wiki/Synchronization)

No libraries are required, everything is in a single standard C++17 include file `await/include/await.h`

A frequent use case is to run a handler function after a specified wait time.  [Details](https://github.com/JamesBremner/await/wiki/Timers)

## Sample code

Code for demo applications is in the `src` folder.  They can be built using the makefile in folder `build/vscode`

 - `timertest.cpp` creates multiple timers that run concurrently
 - `cintest.cpp` demonstrates non-blocking keyboard input
 - `tcpserver.cpp` a simple TCP server that displays message received from a client
 - `tcpclient.cpp` a simple TCP client that sends message to a server as they are typed
 - `eventServer.cpp` handle events with delays and durations. Details: https://github.com/JamesBremner/await/wiki/Event-Server
