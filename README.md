# await
 
Non blocking ( asynchronous ) event waiter

A C++ class that looks after setting up non-blocking waits for events to occur and the code to run ( event handlers ) when the events occur. [Usage Details](https://github.com/JamesBremner/await/wiki/Usage)

No synchronization is usually required.

No libraries are required, everything is in a single standard C++17 include file `await/include/await.h`

## Timers
A frequent use case is to run a handler function after a specified wait time.  [Details](https://github.com/JamesBremner/await/wiki/Timers)


## Synchronization

No synchronization is usually required. 

Each blocking wait function runs in its own thread, so if more than one is waiting they should not access the same data without synchronization.  Usually, data does not need to be shared between blocking wait functions.  ( Let me know if you have a use case that seems to need synchronization )

All the event handlers run in sequence in the same thread, so there are no synchronization worries.

## Long duration event handlers

If some of the event handlers block for a long duration, compared to others, then by default they will block the fast event handlers running.  This may or may not be what is wanted.  If it is required that the fast handlers run immediatly, no matter if long duration handlers are already running, then the long duration handlers must be spawned in their own threads.  Now you will need to worry about synchronization and the possible impact on overall performance of context switching/

## Sample code

Code for demo applications is in the `src` folder.  They can be built using the makefile in folder `build/vscode`

 - `timertest.cpp` creates multiple timers that run concurrently
 - `cintest.cpp` demonstrates non-blocking keyboard input
 - `tcpserver.cpp` a simple TCP server that displays message received from a client
 - `tcpclient.cpp` a simple TCP client that sends message to a server as they are typed
 - `eventServer.cpp` handle events with delays and durations. Details: https://github.com/JamesBremner/await/wiki/Event-Server
