#include "../muduo/net/tcp/TcpClient.h"
#include "../muduo/net/EventLoop.h"
#include "../muduo/net/tcp/InetAddress.h"
#include "../muduo/net/Channel.h"
#include "../muduo/net/tcp/Connector.h"
#include <functional>
#include "../muduo/net/tcp/Acceptor.h"

#include <string>
#include <iostream>
#include <thread>

class EchoClient {
public:
    EchoClient(EventLoop* loop, const InetAddress& serverAddr)
        : client_(loop, serverAddr, "client"),
          loop_(loop)
        {
            client_.setConnectionCallback(std::bind(&EchoClient::onConnection, this, std::placeholders::_1));
            client_.setMessageCallback(std::bind(&EchoClient::onMessage, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        }

    void connect()
    {
        client_.connect();
    }


private:
    void onConnection(const TcpConnectionPtr& conn) {
        if (conn->connected())
        {
            std::cout << "Connected to server. Type messages:\n";
            conn_ = conn;

            // 使用另一个线程持续读取 stdin 并发送给服务器
            std::thread([conn]() {
                std::string line;
                char s = s;
                while (std::getline(std::cin, line)) {
                    //line = s + line;
                    line += "\n";
                    LOG_INFO << "Sending line: [" << line << "], size = " << line.size();
                    conn->send(line);
                }
            }).detach();
        }
        else  
        {
            LOG_INFO << "Disconnected from server";
            loop_->quit();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        std::string msg = buf->retrieveAllAsString();
        std::cout << "Echo from server : " << msg << std::endl;
    }

    EventLoop* loop_;
    TcpClient client_;
    TcpConnectionPtr conn_;
};

int main ()
{
    EventLoop loop;
    InetAddress serveraddr(8080);

    EchoClient client(&loop, serveraddr);
    client.connect();
    loop.loop();

    return 0;
}

// int main() {
//     int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

//     sockaddr_in servaddr;
//     bzero(&servaddr, sizeof(servaddr));
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_port = htons(8080);
//     ::inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

//     if (::connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
//         perror("connect");
//         return 1;
//     }

//     std::string line;
//     while (std::getline(std::cin, line)) {
//         line += '\n';
//         ::write(sockfd, line.c_str(), line.size());  // 只发送真实数据
//     }

//     return 0;
// }
