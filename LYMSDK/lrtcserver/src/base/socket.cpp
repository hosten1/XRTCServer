#include "socket.h"
#include <rtc_base/logging.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>

namespace lrtc
{
    int Create_tcp_server(const char *addr, int port)
    {
        // 创建 TCP Socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        RTC_LOG(LS_ERROR) << " Failed to create socket to errno:" << errno;
        return -1;
    }
//    设置 SO_REUSEADDR
    int on = 1;
    int ret = setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
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
    serverAddr.sin_port = htons(port); // 使用端口8080
    if (addr && inet_aton(addr,&(serverAddr.sin_addr)) == 0)
    {
        RTC_LOG(LS_ERROR) << "invalid address!!" << errno;
        close(serverSocket);
        return -1;
    }
    
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        RTC_LOG(LS_ERROR) << "Failed to bind socket, errno:" << errno;
        close(serverSocket);
        return -1;
    }

    // 监听传入连接请求
    if (listen(serverSocket, 4095) < 0) { // 允许最多4095个连接排队
        RTC_LOG(LS_ERROR) << "Failed to listen, errno:" << errno;;
        close(serverSocket);
        return -1;
    }

   RTC_LOG(LS_INFO) << "Server listening on port " <<port<<"...";

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
    
} // namespace lrtc
