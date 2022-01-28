#include <time.h>
#include <stdio.h>

#include "await.h"

raven::await::cAwait waiter;

time_t start, end;

void handler1()
{
    static int count = 0;
    std::cout << ". " << std::flush;
    if (count++ > 2000)
        waiter.stop();
    waiter(1000, handler1);
}
void handler2()
{
    time(&end);
    double dif = difftime(end, start);
    std::cout << "\n***********timer10sec after " << dif << std::endl;
}
void handler3()
{
    time(&end);
    double dif = difftime(end, start);
    std::cout << "\n************timer15sec after " << dif << std::endl;
    waiter.stop();
}


int main()
{
    time(&start);
    
    waiter(1000, handler1);
    waiter(10000, handler2);
    waiter(15000, handler3);
    waiter.run();
}