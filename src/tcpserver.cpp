#include <iostream>
#include "await.h"

raven::await::cAwait waiter;

void readHandler();

void acceptHandler()
{
    std::cout << "client connected\n";
    waiter.TCPread(readHandler);
}

void readHandler()
{
    if (!waiter.isTCPConnected())
    {
        std::cout << "connection closed\n";

        // wait for a new connection request
        waiter.accept(
            waiter.acceptPort(),
            acceptHandler);
            return;
    }

    std::cout << "Msg read: " << waiter.TCPmsg() << "\n";

    waiter.TCPread(readHandler);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "USAGE: tcpserver <port number>\n";
        exit(1);
    }

    waiter.accept(
        argv[1],
        acceptHandler);

    waiter.run();
}