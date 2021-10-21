#include "await.h"

raven::await::cAwait waiter;

void handler1()
{
    static int count = 0;
    std::cout << "timer1 " << count++ << std::endl;
    if( count < 20 )
        waiter.timer( 1000, handler1 );
}
void handler2()
{
    std::cout << "***********timer2"<< std::endl;
}
void handler3()
{
    std::cout << "************timer3"<< std::endl;
}
int main()
{
    waiter.timer( 1000, handler1 );
    waiter.timer( 10000, handler2 );
    waiter.timer( 15000, handler3 );
    waiter.run();
}