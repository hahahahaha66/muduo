#ifndef TCPCLIENT_C
#define TCPCLIENT_C

#include "Callback.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "Connector.h"
#include <memory>
#include <mutex>

class Connector;


class TcpClient : noncopyable
{
public:
    using ConnectorPtr = std::shared_ptr<Connector>;

    TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string& nameArg);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return connection_;
    }

private:
    void newConnection(int sockfd);

    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr  connector_;
    const std::string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    bool retry_;
    bool connect_;

    int nextConnId_;
    mutable std::mutex mutex_;
    TcpConnectionPtr connection_ ;

};



#endif