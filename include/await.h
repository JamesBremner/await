#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <future>
#include <mutex>
#include <queue>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace raven
{
    namespace await
    {
        /** Event handler type

        It is a std::function for a function returning void and taking no parameters.
        All event handlers need to be like this.
        */

        typedef std::function<void()> handler_t;

        /** non blocking event waiter
        */
        class cAwait
        {
        public:
            cAwait() : myConnectSocket(INVALID_SOCKET)
            {
            }

            /** non blocking wait for cin input
             *  @param[in] f the event handler
             */
            void cin(handler_t f)
            {
                mycinHandler = f;
                new std::thread(cin_block, this);
            }
            /** get string from most recent cin event */
            std::string cin() const
            {
                return mycinString;
            }
            /** non blocking wait for tcp client connection request
             * @param[in] port to listen for connection reuests
             * @param[in] f the event handler
             */
            void accept(
                const std::string &port,
                handler_t f)
            {
                myacceptHandler = f;
                myacceptPort = port;
                initWinSock();
                new std::thread(accept_block, this);
            }
            std::string acceptPort() const
            {
                return myacceptPort;
            }
            /// true if valid TCP connection
            bool isTCPConnected()
            {
                return myConnectSocket != INVALID_SOCKET;
            }
            /** non blocking wait for message to be received fromTCP peer
             *  @param[in] f the event handler
             * 
             * If no peer connected, throws exception
             * 
             * Event handler runs both for a message reacived
             * and for the connected closed or errored.
             * The handler should check status with isTCPConnected()
             */
            void TCPread(handler_t f)
            {
                if (!isTCPConnected())
                    throw std::runtime_error("TCP read on invalid socket");
                myTCPreadHandler = f;
                new std::thread(tcp_read_block, this);
            }
            /** get string from most recent TCP read event */
            std::string TCPmsg() const
            {
                return std::string(myTCPbuf);
            }
            /** Attempt connection to TCP server
             * @param[in] ip server IP address
             * @param[in] port server port listening for connection request
             * @param[in] f the event handler
             */
            void TCPConnect(
                const std::string &ip,
                const std::string port,
                handler_t f)
            {
                myServerIP = ip;
                myServerPort = port;
                myTCPconnectHandler = f;
                tcp_connect_block();
            }
            /** send message to peer
            *  @param[in] msg
            */
            void TCPsend(const std::string &msg)
            {
                if (myConnectSocket == INVALID_SOCKET)
                    throw std::runtime_error("send on invalid socket");
                ::send(
                    myConnectSocket,
                    msg.c_str(),
                    (int)msg.length(), 0);
            }

            /** Run the event handlers in the order they were posted
            *
            *    This will never return
            *    It will keep running ( even if there is no work )
            *    until Stop() is called from one of the event handlers
            *
            */
            void run()
            {
                // keep on running
                while (1)
                {
                    // return if stop flag set
                    if (myStopFlag)
                        break;

                    // get copy of next handler if one waiting
                    if (get_next_handler())
                    {
                        /* 
                        execute handler

                        This is where all the application code is executed
                        in this thread one by one
                        ( even if they were posted 'simultaineously' )
                        */

                        myHandlerCopy();
                    }
                    else
                    {

                        // yield to other threads if no event handlers waiting
                        yield();
                    }
                }
            }

            /**  Stop the event handler running

    This should be called from an event handler

    */

            void Stop()
            {
                myStopFlag = true;
            }

        private:
            bool myStopFlag;           ///< true if stop requested
            handler_t myHandlerCopy;   ///< currently running event handler
            std::queue<handler_t> myQ; ///< the queue of event handlers ready to run
            std::mutex myMutex;        ///< prevents contention between threads trying to access the handler queue

            std::string mycinString;

            std::string myacceptPort;
            SOCKET myAcceptSocket;  //< socket listening for clients
            SOCKET myConnectSocket; //< socket connected to another tcp
            std::string myRemoteAddress;
            char myTCPbuf[1024];
            std::string myServerIP;
            std::string myServerPort;

            handler_t mycinHandler;
            handler_t myacceptHandler;
            handler_t myTCPreadHandler;
            handler_t myTCPconnectHandler;

            /** Post an event handler to be run as soon as possible

                @param[in] f the event handler

    This can be called from any thread
    ( the queue is protected by a mutex )
    but all the handlers will be invoked on by one
    from the same thread as called by this function

    */
            void post(handler_t f)
            {
                std::lock_guard<std::mutex> lock(myMutex);
                myQ.push(f);
            }

            /**  get copy of next handler if one waiting

        @return true if event handler waiting

        A copy of the handler will be made in the attribute myHandlerCopy

    */

            bool get_next_handler()
            {
                // prevent access to queue by other threads
                std::lock_guard<std::mutex> lock(myMutex);

                // do nothing if no handler waiting
                if (myQ.empty())
                    return false;

                // move handler from queue to attribute copy
                myHandlerCopy = myQ.front();
                myQ.pop();

                return true;
            }

            /** yield to other threads if no event handlers waiting */
            void yield()
            {
                // prevent access to queue by other threads
                std::lock_guard<std::mutex> lock(myMutex);

                // yield to other threads if no handlers waiting
                if (myQ.empty())
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(1));
            }

            void cin_block()
            {
                std::cin >> mycinString;
                post(mycinHandler);
            }

            // Initialize Winsock
            void initWinSock()
            {
                WSADATA wsaData;
                if (WSAStartup(MAKEWORD(2, 2), &wsaData))
                {
                    throw std::runtime_error("Winsock init failed");
                }
            }

            void accept_block()
            {
                if (isTCPConnected())
                {
                    std::cout << "second connection rejected";
                    return;
                }

                struct addrinfo *result = NULL,
                                hints;

                ZeroMemory(&hints, sizeof(hints));
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;
                hints.ai_flags = AI_PASSIVE;

                int error = getaddrinfo(
                    NULL, myacceptPort.c_str(),
                    &hints, &result);
                if (error)
                {
                    throw std::runtime_error(
                        "getaddrinfo failed " + std::to_string(error));
                }

                myAcceptSocket = ::socket(
                    result->ai_family,
                    result->ai_socktype,
                    result->ai_protocol);
                if (myAcceptSocket == INVALID_SOCKET)
                {
                    throw std::runtime_error("socket failed");
                }

                if (::bind(myAcceptSocket,
                           result->ai_addr,
                           (int)result->ai_addrlen) == SOCKET_ERROR)
                {
                    closesocket(myAcceptSocket);
                    myAcceptSocket = INVALID_SOCKET;
                    throw std::runtime_error("bind failed");
                }

                if (::listen(
                        myAcceptSocket,
                        SOMAXCONN) == SOCKET_ERROR)
                {
                    closesocket(myAcceptSocket);
                    myAcceptSocket = INVALID_SOCKET;
                    throw std::runtime_error("listen failed");
                }

                std::cout << "listening for client on port " << myacceptPort << "\n";

                struct sockaddr_in client_info;
                int size = sizeof(client_info);
                SOCKET s = ::accept(
                    myAcceptSocket,
                    (sockaddr *)&client_info,
                    &size);
                if (s == INVALID_SOCKET)
                {
                    std::cout << "invalid socket\n";
                    return;
                }

                myConnectSocket = s;
                myRemoteAddress = inet_ntoa(client_info.sin_addr);

                closesocket(myAcceptSocket);

                std::cout << "client " << myRemoteAddress << " accepted\n";

                // post accept handler
                post(myacceptHandler);
            }
            void tcp_read_block()
            {
                // clear old message
                ZeroMemory(myTCPbuf, 1024);

                // wait to receive message
                int r = ::recv(
                    myConnectSocket,
                    (char *)myTCPbuf, 1024, 0);

                // check for message received
                // if no message or error, assume connection closed
                if (r <= 0)
                {
                    std::cout << "connection closed\n";
                    closesocket(myConnectSocket);
                    myConnectSocket = INVALID_SOCKET;
                }

                // post read complete message
                post(myTCPreadHandler);
            }
            void tcp_connect_block()
            {
                initWinSock();

                struct addrinfo *result = NULL,
                                hints;

                ZeroMemory(&hints, sizeof(hints));
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;

                if (getaddrinfo(
                        myServerIP.c_str(),
                        myServerPort.c_str(),
                        &hints, &result))
                {
                    throw std::runtime_error("getaddrinfo failed");
                }

                myConnectSocket = ::socket(
                    result->ai_family,
                    result->ai_socktype,
                    result->ai_protocol);
                if (myConnectSocket == INVALID_SOCKET)
                {
                    throw std::runtime_error("socket failed");
                }

                std::cout << "try connect to " << myServerIP << ":" << myServerPort << "\n";
                if (::connect(
                        myConnectSocket,
                        result->ai_addr,
                        (int)result->ai_addrlen) == SOCKET_ERROR)
                {
                    closesocket(myConnectSocket);
                    myConnectSocket = INVALID_SOCKET;
                    throw std::runtime_error("connect failed " + std::to_string(WSAGetLastError()));
                }
                else
                {
                    post(myTCPconnectHandler);
                }
            }
        };

        /**  @brief A base timer to invoke a function at some later date

    This is an abstract base class.  To use it must be specialized
    and the function handle_timer_event must be over-ridden

    e.g.

<pre>

class cTimedFunction : public raven::event::cTimer
{
public:
    void handle_timer_event()
    {
        // do something useful
    }
};

cTimedFunction tf;
tf.WaitThenPost( 1000 );    // do something useful 1 second from now

</pre>
*/

        // class cTimer
        //     {
        //     public:
        //         /** Non-blocking delayed run of handle_timer_event()

        // @param[in] msec  how long to wait before running handler

        // This returns immediatly.  After the delay is completed
        // ( the delay uses a sleeping thread, so consumes almost no CPU )
        // the function will be posted to the event queue where it will be run
        // as soon as possible after any other posted event handlers

        // */
        //         void WaitThenPost(int msecs)
        //         {
        //             WaitThenPost(
        //                 msecs,
        //                 handler_t(std::bind(&cTimer::handle_timer_event, this)));
        //         }

        //         /** Over-ride this with the code you want to execute
        // when the timer expires.

        // Even if you are not going to use this, it must be overridden,
        // like this

        // void handle_timer_event() {}

        // */

        //         virtual void handle_timer_event() = 0;

        //         /** Non-blocking delayed run of a supplied function

        // @param[in] msec  how long to wait before running handler
        // @param[in] f supplied event handler

        // The supplied function must return void, but can use parameters

        // e.g.

        // <pre>
        //     WaitThenPost(
        //         msecs,
        //         cEventQueue::handler_t( std::bind(&myTimer::myFunction, this, param1, param2 ) ) );
        // </pre>

        // */

        //         void WaitThenPost(int msecs, handler_t f)
        //         {
        //             // run blocking wait and post in a sleeping thread

        //             /*

        //     Note that we store the future that is returned in an attribute
        //     even though we do not use it for antything

        //     The thread will not start if the return is 'disregarded'
        //     i.e. if the return is destroyed.  We must keep it in scope
        //     to keep the thread running

        //     */
        //             myfuture = std::async(
        //                 std::launch::async,         // insist on starting immediatly
        //                 &cTimer::BlockWaitThenPost, // run the blocking wait and post
        //                 this,                       // for this instance
        //                 msecs,                      // length to delay
        //                 f);                         // function to post when the delay completes
        //         }

        //     private:
        //         /**  Blocking wait and post */
        //         void BlockWaitThenPost(int msec, handler_t f)
        //         {
        //             // sleep for reuired delay
        //             std::this_thread::sleep_for(
        //                 std::chrono::milliseconds(msec));

        //             // post the event handler to the event queue
        //             // ( It will run in the main thread,
        //             //  not this one which now dies )

        //             theEventQueue.Post(f);
        //         }

        //         std::future<void> myfuture; ///< this keeps the running sleeping thread in scope
        //     };

        //     /** Stop the event handler after a delay */

        //     class cTimedStop : public cTimer
        //     {
        //     public:
        //         /** ctor

        // @param[in] secs to wait, then stop the event handler

        // Construct this before the call the theEventQueue.Run()
        // to limit the run to the specified period.

        // */

        //         cTimedStop(int secs)
        //         {
        //             WaitThenPost(1000 * secs);
        //         }

        //         /**  event handler to stop them all */

        //         void handle_timer_event()
        //         {
        //             theEventQueue.Stop();
        //         }
        //     };

    }
}
