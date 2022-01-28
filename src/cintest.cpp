#include "await.h"

raven::await::cAwait waiter;

class cCIN
{
public:
static std::string myString;
void operator()()
{
    std::cout << "type something: "; 
    std::cin >> myString;
}
};

std::string cCIN::myString;
cCIN theCIN;

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