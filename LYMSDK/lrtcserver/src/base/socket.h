#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_SOCKET_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_SOCKET_H_

#include<sys/socket.h>

namespace lrtc {
int Create_tcp_server(const char* addr,int port);

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_BASE_SOCKET_H_
