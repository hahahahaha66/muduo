#include "../muduo/net/tcp/TcpClient.h"
#include "../muduo/net/EventLoop.h"
#include "../muduo/net/tcp/InetAddress.h"
#include "../muduo/net/Channel.h"
#include "../muduo/net/tcp/Connector.h"
#include <functional>
#include "../muduo/net/tcp/Acceptor.h"

int main() {
    EventLoop loop;

    InetAddress server(8080);
    TcpClient client(&loop, server, "client");
    client.connect();
    return 0;
}
