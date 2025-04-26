#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "../../base/noncopyable.h"
#include "../../base/Timestamp.h"
#include "Callback.h"
//#include "Buffer.h"
#include "InetAddress.h"
#include "Socket.h"

#include <cstddef>
#include <memory>
#include <string>
#include <atomic>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstddef>
#include <functional>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


class Channel;
class EventLoop;
class Socket;

class TcpConnection : noncopyable,
    public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop,
                  const std::string& nameArg,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);

    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    //const修饰返回值，无法通过引用修改对应的值
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const {return peerAddr_; }

    bool connected() const { return state_ == kConnected; }
    
    //发送数据
    void send(const std::string& buf);
    void send(Buffer* buf);

    void shutdown();

    //保存自定义的回调函数
    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writecompletecallback_ = cb;}
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
    {
        highwatermarkcallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    void connectEstablished();  //连接建立
    void connectDestroyed();  //连接销毁

private:
    enum StateE
    {
        kDisconnected,  //已经断开连接
        kConnecting,  //正在连接
        kConnected,  //已连接
        kDisconnecting,  //正在断开连接
    };
    void setState(StateE state) { state_ = state; }

    //注册不同事件的回调函数
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    
    void sendInLoop(const void* message, size_t len);
    void sendInLoop(const std::string& message);
    void shutdownInLoop();

    EventLoop* loop_;
    const std::string name_;
    std::atomic_int state_;
    bool reading_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    
    const InetAddress localAddr_;  //服务器地址
    const InetAddress peerAddr_;  //对端地址

    ConnectionCallback connectionCallback_;  //新连接的回调
    MessageCallback messageCallback_;  //有读写消息的回调
    WriteCompleteCallback writecompletecallback_;  //消息发送完成以后的回调
    CloseCallback closecallback_;  //客户端关闭的回调
    HighWaterMarkCallback highwatermarkcallback_;  //超出水位的回调
    size_t highWaterMark_;

    // Buffer inputBuffer_;
    // Buffer outputBuffer_;

};

#endif