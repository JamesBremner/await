#include <time.h>
#include <stdio.h>

#include "await.h"

raven::await::cAwait waiter;

time_t start,end;

void handler1()
{
    static int count = 0;
    std::cout << "timer1 " << count++ << std::endl;
    if( count < 20 )
        waiter.timer( 1000, handler1 );
}
void handler2()
{
    time (&end);
    double dif = difftime (end,start);
    std::cout << "***********timer2 after "<< dif << std::endl;
}
void handler3()
{
    time (&end);
    double dif = difftime (end,start);
    std::cout << "************timer3 after"<< dif << std::endl;
}
int main()
{
    time (&start);
    waiter.timer( 1000, handler1 );
    waiter.timer( 10000, handler2 );
    waiter.timer( 15000, handler3 );
    waiter.run();
}