#include "EventLoop.h"
#include <cassert>

EventLoop::EventLoop() : looping_(false), threadId_(CurrentThread::tid())
{
    LOG_TRACE << "EventLoop Created" << this << "in thread" << threadId_;
    if(t_loopInThisThread) 
    {
        LODG_FATAL << "Another EventLoop" << t_loopInThisThread << "exists in this thread" << threadId_;
    }
    else 
    {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() 
{
    return t_loopInThisThread;
}
void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    LOG_TRACE << "EventLoop" << this << "stop looping";
    looping_ = false;
}