#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <condition_variable>

namespace raven
{
    namespace await
    {
        /** non blocking event waiter
        */
        class cAwait
        {
        public:
            /** non-blocking wait for a blocking function to run
             * @param[in] blocker blocking function to run
             * @param[in] handler function to run when blocking function completes
             */
            void operator()(
                std::function<void()> blocker,
                std::function<void()> handler)
            {
                new std::thread(block, this, blocker, handler);
            }

            /** Run the event handlers in the order they were posted
            *
            *    This will never return
            *    It will keep running ( even if there is no work )
            *    until Stop() is called from one of the event handlers
            * 
            *    All the event handlers will run in sequence in the
            *    thread that calls this.
            *
            */
            void run()
            {
                // keep on running
                while (1)
                {
                    // if handler queue is empty,
                    // wait for notification that handler has been posted
                    // ( otherwise thus is a busy loop that runs the CPU ragged )
                    std::mutex cvMutex;
                    std::unique_lock<std::mutex> lck(cvMutex);
                    myHandlerWaiting.wait( lck, [this]{return (bool)myQ.size();});

                    // return if stop flag set
                    if (myStopFlag)
                        break;

                    // get copy of next handler if one waiting
                    if (get_next_handler())
                    {
                        /* execute handler

                        This is where all the application code is executed
                        in this thread one by one
                        ( even if they were posted 'simultaineously' )
                        */
                        myHandlerCopy();
                    }
                }
            }

            /**  Stop the event handler running
            *
            * This should be called from an event handler
            */
            void stop()
            {
                myStopFlag = true;
                std::lock_guard<std::mutex> lock(myMutex);
                std::function<void()> f;
                myQ.push( f );
                myHandlerWaiting.notify_one();
            }

        private:
            typedef std::function<void()> handler_t;
            bool myStopFlag;           ///< true if stop requested
            handler_t myHandlerCopy;   ///< currently running event handler
            std::queue<handler_t> myQ; ///< the queue of event handlers ready to run
            std::mutex myMutex;        ///< prevents contention between threads trying to access the handler queue
            std::condition_variable myHandlerWaiting;

            /** Post an event handler to be run as soon as possible

            @param[in] f the event handler

            This can be called from any thread
            ( the queue is protected by a mutex )
            but all the handlers will be invoked
            from the same thread as called by this function
            */
            void post(handler_t f)
            {
                // protect queue from other threads
                std::lock_guard<std::mutex> lock(myMutex);

                // ass handler to queue
                myQ.push(f);

                // notify that a handler is waiting
                myHandlerWaiting.notify_one();
            }

            /**  get copy of next handler if one waiting
             *
             * @return true if event handler waiting
             * 
             * A copy of the handler will be made in the attribute myHandlerCopy
             * 
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
            /** Run blocking function, post even handler on completion
             * @param[in] blocker
             * @param[in] handler
             * 
             * This is run in its own thread by cAwait()
             */
            void block(
                std::function<void()> blocker,
                std::function<void()> handler)
            {
                blocker();
                post(handler);
            }
        };
    }
}
