#include <iostream>
#include "await.h"
#include "cCIN.h"
#include "cTCP.h"

raven::await::cAwait waiter;

cTCP theTCP;

void lineHandler()
{
    // user has typed something - send it to the server
    theTCP.send(theCIN.myString);

    // wait for more typing
    waiter(theCIN, lineHandler);
}

void connectFunctor()
{
    theTCP.connectToServer();
}
void connectHandler()
{
    std::cout
        << "Connected to server\n"
           "type something to be sent to server\n";
    return;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "USAGE: tcpclient <IP address> <port number>\n";
        exit(1);
    }

    theTCP.server(
        argv[1],
        argv[2]);

    // attempt connection to server
    waiter(
        connectFunctor,
        connectHandler );

    // wait for user to type something
    waiter(
        theCIN,
        lineHandler);

    // run the event manager
    waiter.run();
}