#ifndef SOCKET_H
#define SOCKET_H

#include "../../base/noncopyable.h"
#include "InetAddress.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

class InetAddress;

class Socket : noncopyable
{
public:
    explicit Socket(int sockfd )
        : sockfd_(sockfd)
    {
    }

    ~Socket();

    int fd() const { return sockfd_; };

    void bindAddress(const InetAddress& localdaddr);

    void listen();

    int accept(InetAddress* perraddr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};

#endif