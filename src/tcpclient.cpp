#include <iostream>
#include "await.h"

raven::await::cAwait waiter;

void lineHandler()
{
    // user has typed something - send it to the server
    waiter.TCPsend(waiter.cin());

    // wait for more typing
    waiter.cin(lineHandler);
}

void connectHandler()
{
    std::cout
        << "Connected to server\n"
        "type something send to server\n";
    return;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "USAGE: tcpclient <IP address> <port number>\n";
        exit(1);
    }

    // attempt connection to server
    waiter.TCPConnect(
        argv[1],
        argv[2],
        connectHandler );

    // wait for user to tpe something
    waiter.cin(lineHandler);

    // run the event manager
    waiter.run();
}