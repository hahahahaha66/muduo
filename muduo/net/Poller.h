#include <vector>
#include <map>
#include <poll.h>
#include "EventLoop.h"
struct pollfd;

class Channel;

class Poller 
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    ~Poller();

    Timestamp poll(int timeoutMs, ChannelList* activeChannels);

    void updateChannel(Channel* channel);

    void assertInLoopChannel(){ ownerLoop_->assertInLoopThread(); }

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    using PollFdList = std::vector<struct pollfd>;
    using ChannelMap = std::map<int, Channel>;

    EventLoop* ownweLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};
