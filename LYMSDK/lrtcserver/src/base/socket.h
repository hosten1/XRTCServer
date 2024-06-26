#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_SOCKET_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_SOCKET_H_

#include<sys/socket.h>

namespace lrtc {
int Create_tcp_server(const char* addr,int port);
int tcp_accept_client(int sock,char *host,int *cport);
int sock_setnonblock(int sock);
int sock_setnodelay(int sock);
int sock_peet_to_string(int sock,char *host,int *port);
int sock_read_data(int sock,char *buf,int len);
int sock_write_data(int sock,const char *buf,const int len);

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_BASE_SOCKET_H_
