#include <time.h>
#include <stdio.h>

#include "await.h"

raven::await::cAwait waiter;

class cTimer
{
public:
    int myDelaymsecs;
    cTimer( int msecs ) : myDelaymsecs( msecs )
    {}
    void operator()()
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(myDelaymsecs));
    }
};

cTimer timer1sec( 1000 );
cTimer timer10sec( 10000 );
cTimer timer15sec( 15000 );

time_t start, end;

void handler1()
{
    static int count = 0;
    std::cout << ". " << std::endl;
    if (count++ > 20)
        waiter.stop();
    waiter(timer1sec, handler1);
}
void handler2()
{
    time(&end);
    double dif = difftime(end, start);
    std::cout << "***********timer10sec after " << dif << std::endl;
}
void handler3()
{
    time(&end);
    double dif = difftime(end, start);
    std::cout << "************timer15sec after " << dif << std::endl;
}


int main()
{
    time(&start);
    
    waiter(timer1sec, handler1);
    waiter(timer10sec, handler2);
    waiter(timer15sec, handler3);
    waiter.run();
}