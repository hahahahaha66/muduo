#include "TcpClient.h"
#include "../../logging/Logging.h"
#include "Callback.h"
#include "Connector.h"
#include "../EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

#include <functional>
#include <memory>
#include <mutex>
#include <stdio.h>

namespace detail
{
    using ConnectorPtr = std::shared_ptr<Connector>;

    void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
    {
        loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }

    void removeConnector(const ConnectorPtr& connector)
    {
        //connector->
    }
}


TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string& nameArg)
    : loop_(loop),
      connector_(new Connector(loop, serverAddr)),
      name_(nameArg),
      connectionCallback_(defaultConnectionCallback),
      messageCallback_(defaultMessageCallback),
      retry_(false),
      connect_(true),
      nextConnId_(1)
{
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
    LOG_INFO << "TcpClinet::TcpLient[" << name_ << "] - connector " << connector_.get();
}

TcpClient::~TcpClient()
{
    LOG_INFO << "TcpClient::~TcpClient[" << name_ << "] - connector " << connector_.get();
    TcpConnectionPtr conn;
    bool unique = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }

    if (conn)
    {
        CloseCallback cb = std::bind(&detail::removeConnection, loop_, std::placeholders::_1);
        loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if (unique)
        {
            conn->forceClose();
        }
    }
    else 
    {
        connector_->stop();
        loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    }
}

void TcpClient::connect()
{
    LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to" << connector_->serverAddress().toIpPort();
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect()
{
    connect_ = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (connect_)
        {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    InetAddress peerAddr(Connector::getPeerAddr(sockfd));
}
