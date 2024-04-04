#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_

#include <string>

namespace lrtc {
  struct SignalingServerOptions
  {
    std::string host;
    int port;
    int worker_num;
    int connect_timeout;
  };
  
  class SignalingServer
  {

  public:
    SignalingServer(/* args */);
    ~SignalingServer();
    int init(const char* conf_file);

    private:
    SignalingServerOptions options_;

    int listen_fd_ = -1;
  };


}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_
