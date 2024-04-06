#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_OPTIONS_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_OPTIONS_H_

#include <string>

namespace lrtc {
  // class SignalingServer;
  struct SignalingServerOptions
  {
    std::string host;
    int port;
    int worker_num;
    int connect_timeout;
  };

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_OPTIONS_H_
