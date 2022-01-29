#include "await.h"

raven::await::cAwait waiter;

std::string myString;

/// Blocking wait for input from keyboard
void input()
{
    std::cout << "type something: "; 
    std::cin >> myString;    
}

// handle some keyboard input
void cinHandler()
{
    // check for stop request
    if( myString == "stop") {
        waiter.stop();
        return;
    }

    // display input
    std::cout << "input: "<< myString << std::endl;

    // wait for more input
    waiter( input, cinHandler );
}

int main()
{
    waiter( input, cinHandler );
    waiter.run();
}