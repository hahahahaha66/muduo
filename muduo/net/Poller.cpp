#include "Poller.h"

Poller::Poller(EventLoop* loop) : ownweLoop_(loop)
{
}

Poller::~Poller() 
{
}

Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) 
    {
        LOG_TRACE << numEvents << "events happended";
        fillActiveChannels(numEvents, activeChannels);
    }
    else if (numEvents == 0) 
    {
        LOG_TRACE << "nothing happended";
    }
    else {
        LOG_SYSERR << "Poller::poll()";
    }

    return now;
}
