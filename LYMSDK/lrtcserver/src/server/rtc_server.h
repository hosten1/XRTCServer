#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_H_


#include <memory>
#include <string>
#include <thread>
#include <queue>
#include <mutex>

#include "base/event_loop.h"
#include "base/lrtc_server_def.h"
#include  "json/json.h"



namespace lrtc {
    struct  rtcServerOpeions
    {
        int worker_num;
        public:
          rtcServerOpeions():worker_num(1){}
    };
    
 class RtcServer
 {
 public:
       enum
        {
            MSG_QUIT = 0,
            MSG_RTC_MSG = 1,
        };

       RtcServer();
       ~RtcServer();
       int init(const char *conf_file);
       bool start();
       int stop();
       void joined();

       int send_rtc_msg(const std::shared_ptr<LRtcMsg> rtc_msg);
       void push_msg(std::shared_ptr<LRtcMsg> rtc_msg);
       const std::shared_ptr<LRtcMsg> pop_msg();

       friend void rtc_server_recv_notify(EventLoop * /*el*/, IOWatcher * /*w*/,
                                          int fd, int events, void *data);

   private:
       int _notify(int msg);
       void _process_notify(int msg);
       void _stop();
       void _process_rtc_msg();

   private:
       std::unique_ptr<EventLoop> event_loop_{nullptr};
       rtcServerOpeions options_;
       IOWatcher *pipe_watcher_{nullptr};
       std::unique_ptr<std::thread> ev_thread_;

       std::queue<std::shared_ptr<LRtcMsg>> q_msgs_;
       std::mutex q_thread_mutex_;

       int notify_recv_fd_ = -1;
       int notify_send_fd_ = -1;
 };
 

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_H_
