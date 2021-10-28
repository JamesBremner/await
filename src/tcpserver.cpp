#include <iostream>
#include "await.h"
#include "cTCP.h"

raven::await::cAwait waiter;

raven::set::cTCP theTCP;

void readHandler()
{
    if (!theTCP.isConnected())
    {
        std::cout << "connection closed\n";

        // wait for a new connection request
        waiter(
            []
            { theTCP.acceptClient(); },
            []
            {
                std::cout << "client connected\n";
                waiter(
                    []
                    { theTCP.read(); },
                    readHandler);
            });
        return;
    }

    // display message received
    std::cout << "Msg read: " 
        << theTCP.readMsg() << "\n";

    // wait for another message
    waiter(
        []
        { theTCP.read(); },
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
        []
        { theTCP.acceptClient(); },
        []
        {
            std::cout << "client connected\n";
            waiter(
                []
                { theTCP.read(); },
                readHandler);
        });

    waiter.run();
}