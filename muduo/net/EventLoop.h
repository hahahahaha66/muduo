#include <thread>
#include "../base/CurrentThread.h"

class EventLoop 
{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    EventLoop* getEventLoopOfCurrentThread();

    void assertInLoopThread()
    {
        if(!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const
    {
        return threadId_ == CurrentThread::tid();
    }

private:

    void abortNotInLoopThread();

    bool looping_;
    const std::thread::id threadId_;
};