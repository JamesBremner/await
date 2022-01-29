#include <time.h>
#include <stdio.h>

#include "await.h"

class cTimerTest
{
public:
    cTimerTest()
        : count(0)
    {
        // store start time
        time(&start);

        // start periodic timer to display time passing
        // by printing a dot as each second passes
        waiter(1000,
               [this]
               { periodic(); });

        // schedule handler2 after 10 seconds
        waiter(10000,
               [this]
               { handler2(); });

        // schedule handler3 after 15 seconds
        waiter(15000,
               [this]
               { handler3(); });

        waiter.run();
    }
    void periodic()
    {
        // print a dot every second

        std::cout << ". " << std::flush;

        // do not run for ever
        if (count++ > 2000)
            waiter.stop();

        // schedule next display
        waiter(1000,
               [this]
               { periodic(); });
    }

    void handler2()
    {
        // display time since start when handler ran
        time(&end);
        std::cout << "\n***********timer10sec after "
                  << difftime(end, start) << std::endl;
    }
    void handler3()
    {
        // display time since start when handler ran
        time(&end);
        std::cout << "\n************timer15sec after "
                  << difftime(end, start) << std::endl;

        // all handlers run so can stop
        waiter.stop();
    }

private:
    raven::await::cAwait waiter;  // Non blocking ( asynchronous ) event waiter
    time_t start, end;            // times
    int count;                    // count of seconds running
};

int main()
{
    cTimerTest TT;
}