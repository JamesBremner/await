# await
 
Non blocking ( asynchronous ) event waiter

A C++ class that looks after setting up non-blocking waits for events to occur and the code to run ( event handlers ) when the events occur.

No synchronization is usually required.

No libraries are required, everything is in a single standard C++17 include file `await/include/await.h`

## general usage

 - define blocking wait function with signature `void f()`
 - define event handler function with signature `void f()`
 - construct instance of cAwait
 - register blocking wait function and associated event handler functions using cAwait() method
 - start event manager by calling blocking method cAwait::run()

Alternatives to registering functions with signature `void f()`

 - Use a functor.  That is a class with a method `void operator()()`
 - Use a lambda.  This looks like `[]{ ...code... }`

## Timed wait
A frequent use case is to run a handler function after a specified wait time.  For this, the cAwait provides a convenience method

            /** non-blocking wait for a time, then run function in execution thread
             * @param[in] msecs to wait before running hanler
             * @param[in] handler function to run after wait
             */
            void operator()(
                int msecs,
                std::function<void()> handler)
                
This can be used to schedule a periodic regular event, like this

```
void myClass::repeater()
{
   // ... do something ...
   
   // schedule a repeat in 1 second
   await(
       1000,
       [this]
       { repeater(); });
}
```

## Synchronization

No synchronization is usually required. 

Each blocking wait function runs in its own thread, so if more than one is waiting they should not access the same data without synchronization.  Usually, data does not need to be shared between blocking wait functions.  ( Let me know if you have a use case that seems to need synchronization )

All the event handlers run in sequence in the same thread, so there are no synchronization worries.

## Sample code

Code for demo applications is in the `src` folder.  They can be built using the makefile in folder `build/vscode`

 - `timertest.cpp` creates multiple timers that run concurrently
 - `cintest.cpp` demonstrates non-blocking keyboard input
 - `tcpserver.cpp` a simple TCP server that displays message received from a client
 - `tcpclient.cpp` a simple TCP client that sends message to a server as they are typed
 - `eventServer.cpp` handle events with delays and durations. Details: https://github.com/JamesBremner/await/wiki/Event-Server
