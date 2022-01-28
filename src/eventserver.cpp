#include <ctime>
#include <chrono>
#include <map>
#include "await.h"

/// Schedule event handlers to run after specified delays
class cEventServer
{
public:
    // CTOR - specifies user prompt abd starts regular status display
    cEventServer();

    /// start server running
    void run();

private:

    /// Event structure
    struct sEvent
    {
        std::string name;       ///< event name
        int delay;              ///< msec delay from arrive to handler running
        int duration;           ///< handler duration
    };

    /** Non blocking event waiter - looks after multi-threading
     * 
     * Sets up non-blocking waits for an event to occur
     * and the code to run ( event handler ) when the event occur.
     * https://github.com/JamesBremner/await
     */
    raven::await::cAwait waiter;

    /// Prompt displayed to user when readey for arrival of new event
    std::string myPrompt;

    /// Input from user
    std::string myInput;

    /// Status of events that have arrived, map keyed by the event name
    std::map<std::string, std::string> mympStatus;

    //////////////////////  Methods ///////////////////////////

    /// wait for a new event to arrive
    void waitForInput();

    /// schedule event handler after a delay
    void handleInput();

    /** Parse input string
     * @return event structure
     */
    sEvent parseInput();

    /// event handler
    void handleEvent(sEvent E);

    /// display status
    void status();
};

cEventServer::cEventServer()
{
    myPrompt = "enter event name, delay and duration ( milliseconds ) e.g. \"test 4000 2000\"\n";
    waiter(
        2000,               // display status every 2 seconds
        [this]
        { status(); });
}
void cEventServer::run()
{
    waitForInput();
    waiter.run();
}

void cEventServer::waitForInput()
{
    waiter(
        [this]
        {
            std::cout << myPrompt;
            std::getline(std::cin, myInput);
        },
        [this]
        {
            handleInput();
        });
}

cEventServer::sEvent cEventServer::parseInput()
{
    sEvent E;

    /// check for stop request
    if (myInput == "stop")
    {
        waiter.stop();
    }
    int p = myInput.find(" ");
    E.name = myInput.substr(0, p);
    E.delay = atoi(myInput.substr(p + 1).c_str());
    p = myInput.find(" ", p + 1);
    E.duration = atoi(myInput.substr(p + 1).c_str());
    return E;
}

void cEventServer::handleInput()
{
    // parse event string
    auto E = parseInput();

    mympStatus[E.name] = "waiting";

    // run handler when time comes
    waiter(
        E.delay,
        [this, E]
        { handleEvent(E); });

    // wait for more input
    waitForInput();
}

void cEventServer::handleEvent(sEvent E)
{
    auto tt = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    std::cout << "\n"
              << ctime(&tt) << " "
              << E.name << " handler running\n";

    mympStatus[E.name] = "running";

    waiter(
        E.duration,
        [this, E]
        {
            auto tt = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now());
            std::cout << "\n"
                      << ctime(&tt) << " "
                      << E.name << " handler finished\n";
            mympStatus[E.name] = "done";
        });
}
void cEventServer::status()
{
    if( mympStatus.size() )
        std::cout << "\t\tStatus:\n";
    for (auto s : mympStatus)
    {
        std::cout << "\t\t" << s.first << " " << s.second << "\n";
    }
    waiter(
        2000,
        [this]
        { status(); });
}

int main()
{
    cEventServer S;
    S.run();
}