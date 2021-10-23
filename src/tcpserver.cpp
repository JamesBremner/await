#include <iostream>
#include "await.h"
#include "cTCP.h"

raven::await::cAwait waiter;

cTCP theTCP;

void readHandler();

void acceptFunctor()
{
    theTCP.acceptClient();
}
void readFunctor()
{
    theTCP.read();
}

void acceptHandler()
{
    std::cout << "client connected\n";
    waiter(readFunctor, readHandler);
}

void readHandler()
{
    if (!theTCP.isConnected())
    {
        std::cout << "connection closed\n";

        // wait for a new connection request
        waiter(
            acceptFunctor,
            acceptHandler);
        return;
    }

    std::cout << "Msg read: " << theTCP.readMsg() << "\n";

    waiter(
        readFunctor,
        readHandler);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "USAGE: tcpserver <port number>\n";
        exit(1);
    }

    theTCP.server(
        "",
        argv[1]);

    waiter(
        acceptFunctor,
        acceptHandler);

    waiter.run();
}