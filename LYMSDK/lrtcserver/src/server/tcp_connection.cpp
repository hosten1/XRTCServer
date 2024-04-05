#include "server/tcp_connection.h"
#include <cstring>

#include "base/socket.h"

namespace lrtc
{
    TcpConnection::TcpConnection(int fd, const char *ip, int port) : fd_(fd), port_(port)
    {
        memset(ip_, 0, sizeof(ip_));
        if (ip)
        {
            strncpy(ip_, ip, sizeof(ip_) - 1);
        }
    }

    TcpConnection::TcpConnection(int fd) : fd_(fd)
    {
        sock_peet_to_string(fd_, ip_, &port_);
    }

    TcpConnection::~TcpConnection()
    {
    }

} // namespace tcp_connection
