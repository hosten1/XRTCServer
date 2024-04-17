#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_

#include <string>
#include <thread>
#include <vector>

#include "base/event_loop.h"

// #include "server/signaling_server_options.h"

// #include "api/task_queue/task_queue_factory.h"
// #include "api/task_queue/default_task_queue_factory.h"

// #include <rtc_base/task_queue.h>
// #include <rtc_base/task_utils/repeating_task.h>
// #include <rtc_base/task_utils/to_queued_task.h>
#include "server/settings.h"

namespace lrtc
{
  class SignalingWork;

  class SignalingServer
  {

  public:
    enum
    {
      MSG_QUIT = 0,
    };
    SignalingServer();
    ~SignalingServer();
    int init(const SignalingServerOptions &options);
    bool start();
    int stop();
    int notify(int msg);
    void joined();

    friend void signaling_server_recv_notifi_cb(EventLoop *el, IOWatcher *w,
                                                int fd, int events, void *data);
    friend void accep_new_conn(EventLoop *el, IOWatcher *w,
                               int fd, int events, void *data);

  private:
    void on_recv_notify(int msg);
    void _stop();
    int _create_worker(int worker_id);
    void _dispatch_new_conn(int fd);

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

    std::vector<std::unique_ptr<SignalingWork>> workers_;
    size_t next_works_index_ = 0;
  };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_SERVER_H_
