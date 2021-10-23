#include "await.h"
#include "cCIN.h"

raven::await::cAwait waiter;

// handle some keyboard input
void cinHandler()
{
    // check for stop request
    if( theCIN.myString == "stop") {
        waiter.stop();
        return;
    }

    // display input
    std::cout << "input: "<< theCIN.myString << std::endl;

    // wait for more input
    waiter( theCIN, cinHandler );
}

int main()
{
    waiter( theCIN, cinHandler );
    waiter.run();
}