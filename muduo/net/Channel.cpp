#include "Channel.h"
#include <csignal>
#include "../logging/Logging.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLL_IN | POLL_PRI;
const int Channel::kWriteEvent = POLL_OUT;

Channel::Channel(EventLoop* loop, int fdArg)
    : loop_(loop), 
    fd_(fdArg), 
    events_(0), 
    revents_(0),
    index_(-1) 
{
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent()
{
    if (revents_ & POLLNVAL)//POLLNVAL 无效的文件描述符
    {
        LOG_WARN << "Channel::handle_event() POLLNVAL";
    }

    if (revents_ & (POLLERR | POLLNVAL))//POLLERR 出现错误 
    {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))//POLLIN 有数据可读 POLLPRI 有紧急数据可读
    {                                             //POLLEDHUP 对端关闭连接
        if(readCallback_) readCallback_();
    }
    if (revents_ & POLLOUT) //POLLOUT 可以写数据
    {
        if (writeCallback_) writeCallback_();
    }
}
