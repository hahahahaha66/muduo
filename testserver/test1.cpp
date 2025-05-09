#include "../muduo/net/EventLoop.h"
#include "../muduo/net/tcp/InetAddress.h"
#include "../muduo/net//tcp/TcpServer.h"
#include "../muduo/net/Channel.h"
#include "../muduo/logging/LogFile.h"
#include "../muduo/logging/AsyncLogging.h"

#include <cstddef>
#include <functional>
#include <iomanip>  // std::setw, std::setfill
#include <memory>
#include <sstream>  // std::ostringstream
#include <csignal>

std::unique_ptr<AsyncLogging> asynclog;

void asyncOutput(const char* msg, int len)
{
    //std::cout << "Log length: " << len << std::endl;

    if (len > 0) {
        asynclog->append(msg, len);

        // 检查 msg 是否已经包含 \n，若无再追加
        if (msg[len - 1] != '\n') {
            asynclog->append("\n", 1);
        }
    }
}

class EchoServer{
public:
    EchoServer(EventLoop* loop, const InetAddress& addr, const std::string& name)
        : server_(loop, addr, name),
          loop_(loop)
    {
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        server_.setThreadNum(3);
    }

    ~EchoServer()
    {
    }

    void start()
    {
        server_.start();
    }


private:
    
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
            LOG_INFO << "Connection UP : " << conn->peerAddress().toIpPort().c_str();
        else
            LOG_INFO << "Connection DOWN : " << conn->peerAddress().toIpPort().c_str();
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
    {
        const char* data = buf->peek();
        size_t len = buf->readableBytes();

        //dumpBufferHex(data, len);

        while (true) {
            const char* eol = static_cast<const char*>(
                memchr(buf->peek(), '\n', buf->readableBytes()));
            if (eol) {
                std::string line(buf->peek(), eol); // 不含 '\n'
                buf->retrieveUntil(eol + 1); // 跳过 '\n'
                conn->send(line + "\n");
            } else {
                break; // 没有完整的一行，退出等待更多数据
            }
        }
    }

    EventLoop* loop_;
    TcpServer server_;

};


int main() {
    asynclog = std::make_unique<AsyncLogging>("test", 500 * 1000 * 1000);

    Logger::setOutput(asyncOutput);
    asynclog->start();

    EventLoop loop;

    InetAddress addr(8080);

    EchoServer server(&loop, addr, "EchoServer-01");

    server.start();

    loop.loop();

    return 0;

}