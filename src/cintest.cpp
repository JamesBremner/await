#include "await.h"

raven::await::cAwait waiter;

void cinHandler()
{
    std::cout << waiter.cin() << "\n";
    waiter.cin( cinHandler );
}
int main()
{
    waiter.cin( cinHandler );
    waiter.run();
}