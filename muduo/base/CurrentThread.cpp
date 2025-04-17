#include "CurrentThread.h"
#include <csignal>
#include <sys/syscall.h>
#include <unistd.h>

namespace CurrentThread 
{
    __thread int t_cachedTid = 0;

    void catchTid ()
    {
        if (t_cachedTid == 0)
        {   
            //系统调用，获取线程id
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }
}