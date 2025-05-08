#include "../muduo/net/EventLoop.h"
#include "../muduo/net/tcp/InetAddress.h"
#include "../muduo/net//tcp/TcpServer.h"
#include "../muduo/net/Channel.h"
#include <cstddef>
#include <functional>
#include <iomanip>  // std::setw, std::setfill
#include <sstream>  // std::ostringstream


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

    void start()
    {
        server_.start();
    }

    void dumpBufferHex(const char* data, size_t len) {
        std::ostringstream oss;
        oss << ">>> Received " << len << " bytes:\n";
    
        for (size_t i = 0; i < len; ++i) {
            if (i % 16 == 0)
                oss << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
    
            oss << std::setw(2) << std::setfill('0') << std::hex << (static_cast<unsigned int>(data[i]) & 0xff) << " ";
    
            if ((i + 1) % 16 == 0)
                oss << "\n";
        }
    
        if (len % 16 != 0)
            oss << "\n";
    
        // 打印可读字符
        oss << ">>> As string: ";
        for (size_t i = 0; i < len; ++i) {
            char ch = data[i];
            if (isprint(static_cast<unsigned char>(ch)) || ch == '\n')
                oss << ch;
            else
                oss << '.';
        }
    
        LOG_INFO << "\n" << oss.str();
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

        dumpBufferHex(data, len);

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
    EventLoop loop;

    InetAddress addr(8080);

    EchoServer server(&loop, addr, "EchoServer-01");

    server.start();

    loop.loop();

    return 0;

}