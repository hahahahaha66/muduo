#ifndef POLLER_H
#define POLLER_H

#include "../base/noncopyable.h"
#include "Channel.h"
#include "../base/Timestamp.h"

#include <vector>
#include <unordered_map>

class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    bool hasChannel(Channel* channel) const;

    static Poller* newDefaultPoller(EventLoop* Loop);

protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;
};

#endif 