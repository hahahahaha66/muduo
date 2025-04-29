#include "muduo/net/EventLoop.h"
#include "muduo/net/tcp/InetAddress.h"
#include "muduo/net/tcp/TcpServer.h"

int main()
{
    EventLoop ha;
    InetAddress address(9000,"127.0.0.1");
    TcpServer haha(&ha,address,"haha");
    haha.start();
    return 0;
}