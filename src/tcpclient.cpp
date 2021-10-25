#include <iostream>
#include <chrono>
#include "await.h"
#include "cCIN.h"
#include "cTCP.h"

raven::await::cAwait waiter;

class cTimer
{
public:
    int myDelaymsecs;
    cTimer(int msecs) : myDelaymsecs(msecs)
    {
    }
    void operator()()
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(myDelaymsecs));
    }
};

cTCP theTCP;
bool serverConnectionSuccess;

void inputHandler()
{
    // user has typed something - send it to the server
    theTCP.send(theCIN.myString);

    // wait for more typing
    waiter(theCIN, inputHandler);
}

void ServerDisconnectHandler()
{
    if (!theTCP.isConnected())
    {
        std::cout << "Server disconnected\n";
        exit(1);
    }
    // ignore message and carry on
    waiter(
        []
        { theTCP.read(); },
        ServerDisconnectHandler);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "USAGE: tcpclient <IP address> <port number>\n";
        exit(1);
    }

    serverConnectionSuccess = false;

    // set IP address and port of server to connect to
    theTCP.server(
        argv[1],
        argv[2]);

    // wait for connection to server
    waiter(
        []
        { theTCP.serverWait(); },
        []
        {
            if (!theTCP.isConnected())
            {
                std::cout << "Server connection failed";
                exit(1);
            }
            serverConnectionSuccess = true;
            std::cout
                << "Connected to server\n"
                   "type something to be sent to server\n";

            // wait for user to type something
            waiter(
                theCIN,
                inputHandler);

            // wait for server to disconnect
            waiter(
                []
                { theTCP.read(); },
                ServerDisconnectHandler );
        });

    // abandon server wait if no connection after 20 secs
    cTimer timer20sec(20000);
    waiter(
        timer20sec,
        []
        {
            if (serverConnectionSuccess)
                return;

            std::cout << "No connection to server after 20 secs\n";
            exit(1);
        });

    // run the event manager
    waiter.run();
}