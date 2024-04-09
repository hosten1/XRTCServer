#include "base/socket.h"
#include <rtc_base/logging.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "socket.h"

namespace lrtc
{
    int Create_tcp_server(const char *addr, int port)
    {
        // 创建 TCP Socket
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0)
        {
            RTC_LOG(LS_ERROR) << " Failed to create socket to errno:" << errno;
            return -1;
        }
        //    设置 SO_REUSEADDR
        int on = 1;
        int ret = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (-1 == ret)
        {
            RTC_LOG(LS_ERROR) << "setsockopt SO_REUSEADDR error, errno:" << errno;
            close(serverSocket);
            return -1;
        }

        // 绑定服务器地址和端口
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 任何可用地址
        serverAddr.sin_port = htons(port);              // 使用端口8080
        if (addr && inet_aton(addr, &(serverAddr.sin_addr)) == 0)
        {
            RTC_LOG(LS_ERROR) << "invalid address!!" << errno;
            close(serverSocket);
            return -1;
        }

        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            RTC_LOG(LS_ERROR) << "Failed to bind socket, errno:" << errno;
            close(serverSocket);
            return -1;
        }

        // 监听传入连接请求
        if (listen(serverSocket, 4095) < 0)
        { // 允许最多4095个连接排队
            RTC_LOG(LS_ERROR) << "Failed to listen, errno:" << errno;
            ;
            close(serverSocket);
            return -1;
        }

        RTC_LOG(LS_INFO) << "Server listening on port " << port << "...";

        // // 接受传入连接请求
        // struct sockaddr_in clientAddr;
        // socklen_t clientLen = sizeof(clientAddr);
        // int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        // if (clientSocket < 0) {
        //     RTC_LOG(LS_ERROR)  << "Failed to accept connection, errno:" << errno;
        //     close(serverSocket);
        //     return -1;
        // }

        // RTC_LOG(LS_INFO) << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "\n";

        // // 在此处可以与客户端进行通信，发送和接收数据

        // // 关闭连接
        // close(clientSocket);
        // close(serverSocket);
        return serverSocket;
    }
    int generic_accept(int sock, struct sockaddr *clientAddr, socklen_t *clientLen)
    {
        if (sock < 0)
        {
            RTC_LOG(LS_ERROR) << "Invalid socket provided";
            return -1;
        }

        // 确保clientAddr和clientLen是有效指针
        if (!clientAddr || !clientLen)
        {
            RTC_LOG(LS_ERROR) << "Invalid client address or length provided";
            return -1;
        }

        int fd = -1;
        while (true)
        {
            fd = accept(sock, clientAddr, clientLen);
            if (fd < 0)
            {
                if (errno == EINTR)
                {
                    RTC_LOG(LS_INFO) << "accept interrupted, retrying";
                    sleep(0.1); // 退避一段时间
                    continue;
                }
                else
                {
                    RTC_LOG(LS_ERROR) << "Failed to accept connection err:" << strerror(errno) << ", errno:" << errno;
                    return -1;
                }
            }
            else
            {
                break;
            }
        }

        return fd;
    }
    int tcp_accept_client(int sock, char *host, int *cport)
    {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int fd = generic_accept(sock, (sockaddr *)&clientAddr, &clientLen);
        if (-1 == fd)
        {
            RTC_LOG(LS_ERROR) << "Failed to accept connection, errno:" << errno;
            return -1;
        }

        if (host)
        {
            strcpy(host, inet_ntoa(clientAddr.sin_addr));
        }
        if (cport)
        {
            *cport = ntohs(clientAddr.sin_port);
        }

        RTC_LOG(LS_INFO) << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "\n";
        return fd;
    }

    int sock_setnonblock(int sock)
    {
        int flags = fcntl(sock, F_GETFL, 0);
        if (-1 == flags)
        {
            RTC_LOG(LS_ERROR) << "fcntl(F_GETFL) failed, errno:" << strerror(errno) << " errno:" << errno;
            return -1;
        }

        flags |= O_NONBLOCK;
        if (-1 == fcntl(sock, F_SETFL, flags))
        {
            RTC_LOG(LS_ERROR) << "fcntl(F_SETFL) failed, errno:" << strerror(errno) << " errno:" << errno;
            return -1;
        }

        return 0;
    }

    int sock_setnodelay(int sock)
    {
        int yes = 1;
        int ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&yes, sizeof(yes));
        if (-1 == ret)
        {
            RTC_LOG(LS_ERROR) << "setsockopt TCP_NODELAY failed, errno:" << strerror(errno) << " errno:" << errno;
            return -1;
        }

        return 0;
    }

    int sock_peet_to_string(int sock, char *host, int *port)
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int ret = getpeername(sock, (struct sockaddr *)&addr, &len);
        if (-1 == ret)
        {
            if (host)
            {
                host[0]='?';
                host[1]='\0';
            }
            if (port)
            {
               *port = 0;
            }
            
            
            RTC_LOG(LS_ERROR) << "getsockname failed, errno:" << strerror(errno) << " errno:" << errno;
            return -1;
        }
        if (host)
        {
            memcpy(host, inet_ntoa(addr.sin_addr), INET_ADDRSTRLEN);
        }
        if (port)
        {
            *port = ntohs(addr.sin_port);
        }
        
        
        return 0;
    }

    int sock_read_data(int sock, char *buf, int len)
    {
        RTC_LOG(LS_INFO) << "sock_read_data sockFd:"<<sock;
        int nread = read(sock, buf, len);
        if (-1 == nread)
        {
            if (EAGAIN == errno)
            {
               nread = 0;

            }else{
                RTC_LOG(LS_WARNING) << "sock read failed, errno:" << strerror(errno) << " errno:" << errno;
                return -1;
            }
            
        }else if (0 == nread){
            RTC_LOG(LS_WARNING)<<"sock read 0 bytes, errno:" << strerror(errno) << " errno:" << errno;
                return -1;
        }else{
            return nread;
        }
        
        return 0;
    }
    int sock_write_data(int sock, const char *buf, const int len)
    {
        RTC_LOG(LS_INFO) << "sock_write_data sockFd:"<<sock << ", len = "<< len;

        int nwritten = write(sock,buf,len);
        if (-1 == nwritten)
        {
            if (EAGAIN == errno)
            {
               nwritten = 0;

            }else{
                RTC_LOG(LS_WARNING) << "sock write failed, errno:" << strerror(errno) << " errno:" << errno;
                return -1;
            }
            
        }else if (0 == nwritten){
            RTC_LOG(LS_WARNING)<<"sock write 0 bytes, errno:" << strerror(errno) << " errno:" << errno;
                return -1;
        }else{
            return nwritten;
        }

        return 0;
    }

} //namespace lrtc
