#include "Socket.h"
#include "../../logging//Logging.h"
#include "InetAddress.h"

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::~Socket()
{
    ::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localaddr)
{
    if (::bind(sockfd_, (sockaddr*)localaddr.getSockAddr(), sizeof(sockaddr_in)) != 0)
    {
        LOG_FATAL << "bind sockfd:" << sockfd_ << " failed";
    }
}

void Socket::listen()
{
    if (::listen(sockfd_, 1024) != 0)
    {
        LOG_FATAL << "listen sockfd:" << sockfd_ << " failed";
    }
}

int Socket::accept(InetAddress* peeraddr)
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(addr));

    int connfd = ::accept4(sockfd_, (sockaddr*)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
    {
        peeraddr->setSockAddr(addr);
    }
    else 
    {
        LOG_ERROR << "accept4() failed";
    }
    return connfd;
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR << "shutdownWrite error";
    }
}


