#ifndef EPOLLPOLLER_H
#define EPOLLPOLLER_H

#include "../logging/Logging.h"
#include "EventLoop.h"
#include "Poller.h"
#include "../base/Timestamp.h"

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>

class EPollPoller : public Poller
{
    using EventList = std::vector<epoll_event>;

public:
    EPollPoller(EventLoop* Loop);
    ~EPollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    void update(int operation, Channel* channel);

    int epollfd_;
    EventList events_;
};

#endif