#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_WORKER_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_WORKER_H_

#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>

#include "base/lock_free_queue.h"
#include "base/lock_free_queue.h"
#include "base/lrtc_server_def.h"
#include "json/json.h"
#include "server/settings.h"

namespace lrtc
{
    class EventLoop;
    class TimerWatcher;
    class IOWatcher;
    class RTPStreamManager;
    class TcpConnection;

    class RtcWorker
    {
    public:
        enum
        {
            MSG_QUIT = 0,
            MSG_RTC_MSG = 1,
        };
        RtcWorker(int work_id, const struct RtcServerOptions &option);
        ~RtcWorker();
        int init();
        bool start();
        int stop();
        void joined();
        void push_msg(std::shared_ptr<LRtcMsg> rtc_msg);
        bool pop_msg(std::shared_ptr<LRtcMsg> *rtc_msg);
        int notify_new_conn(int fd);
        int send_rtc_msg(const std::shared_ptr<LRtcMsg> rtc_msg);

        friend void rtc_worker_recv_notify(EventLoop *el, IOWatcher *w, int fd,
                                           int events, void *data);
        friend void rtc_worker_conn_io_cb(EventLoop * /*el*/, IOWatcher * /*w*/,
                                          int fd, int events, void *data);
        friend void rtc_worker_conn_timer_cb(EventLoop *el, TimerWatcher * /*w*/, void *data);

    private:
        int _notify(int msg);
        void on_recv_notify(int msg);
        void _stop();
        void _process_rtc_msg();
        void _process_push_rtcmsg(std::shared_ptr<LRtcMsg> rtcmsg);
        void _process_answer_rtcmsg(std::shared_ptr<LRtcMsg> rtcmsg);

        void _read_query(int fd);
        void _close_connection(TcpConnection *conn);
        void _remove_connection(TcpConnection *conn);
        void _process_timeout(TcpConnection *conn);
        int _process_request_msg(TcpConnection *conn, Json::Value root, uint32_t log_id);

    private:
        int work_id_;
        std::unique_ptr<EventLoop> el_;

        IOWatcher *pipe_watcher_ = nullptr;
        std::unique_ptr<std::thread> ev_thread_;
        LockFreeQueue<std::shared_ptr<LRtcMsg>> q_msg_;

        int notify_recv_fd_ = -1;
        int notify_send_fd_ = -1;

        struct RtcServerOptions options_;

        std::unique_ptr<RTPStreamManager> rtp_stream_manager_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_WORKER_H_
