#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_

#include <string>
#include <thread>
#include "base/event_loop.h"

namespace lrtc
{
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
    enum
    {
      MSG_QUIT = 0,
    };
    SignalingServer(/* args */);
    ~SignalingServer();
    int init(const char *conf_file);
    bool start();
    int stop();
    int notify(int msg);
    void joined();

    friend void signaling_server_recv_notifi_cb(EventLoop *el,
                                                IOWatcher *w,
                                                int fd,
                                                int events,
                                                void *data);

  private:
    void on_recv_notify(int msg);
    void _stop();

  private:
    SignalingServerOptions options_;
    std::unique_ptr<EventLoop> loop_ = nullptr;
    IOWatcher *io_watcher_ = nullptr;
    // 线程通讯用
    IOWatcher *pipe_watcher_ = nullptr;
    TimerWatcher *timer_watcher_ = nullptr;
    std::unique_ptr<std::thread> ev_thread_;

    int listen_fd_ = -1;
    int notify_recv_fd_ = -1;
    int notify_send_fd_ = -1;
  };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_
