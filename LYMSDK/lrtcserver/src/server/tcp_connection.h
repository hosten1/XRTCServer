#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_

#include <string>
#include "base/event_loop.h"


namespace lrtc
{
    class TcpConnection
    {
    public:
        TcpConnection(int fd, const char *ip, int port);
        TcpConnection(int fd);
        ~TcpConnection();

        int send(const char *buf, int len);
        int recv(char *buf, int len);
        int close();

        int get_fd() const { return fd_; }
        const char *get_ip() const { return ip_; }
        int get_port() const { return port_; }
        IOWatcher *io_watcher_;
    private:
        int fd_;
        char ip_[64];
        int port_;
       
        int recv_len_;
        int send_len_;
        int recv_buf_len_;
        char *recv_buf_;
        char *send_buf_;
    };

}; // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_
