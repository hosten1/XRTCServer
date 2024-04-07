#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_WORKER_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_WORKER_H_

#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>

#include "base/event_loop.h"
#include "base/lock_free_queue.h"
#include "server/tcp_connection.h"
#include "server/rtc_server_options.h"


namespace lrtc
{
    class RtcWorker
    {
    public:
            enum
        {
            MSG_QUIT = 0,
            MSG_NEW_CONN = 1,
        };
        RtcWorker(int work_id, const struct rtcServerOptions& option);
        ~RtcWorker();
        int init();
        bool start();
        int stop();
        void joined();
        int notify_new_conn(int fd);

        friend void rtc_worker_recv_notify(EventLoop *el, IOWatcher *w, int fd,
                                                 int events, void *data);
        friend void rtc_worker_conn_io_cb(EventLoop * /*el*/, IOWatcher * /*w*/,
                               int fd, int events, void *data);
        friend void rtc_worker_conn_timer_cb(EventLoop *el, TimerWatcher * /*w*/, void *data);

    private:
        int _notify(int msg);
        void on_recv_notify(int msg);
        void _stop();
        void _accept_new_connection(const int fd);
        void _read_query(int fd);
        void _close_connection( TcpConnection *conn);
        void _remove_connection( TcpConnection *conn);
        void _process_timeout(TcpConnection *conn);
        int _process_request_msg(TcpConnection * conn, Json::Value root, uint32_t log_id);
        int _process_request_push_msg(TcpConnection * conn,int cmdno, Json::Value root, uint32_t log_id);

    private:
        int work_id_;
        std::unique_ptr<EventLoop> el_;

        IOWatcher *pipe_watcher_ = nullptr;
        std::unique_ptr<std::thread> ev_thread_;
        LockFreeQueue<int> notify_queue_;

        int notify_recv_fd_ = -1;
        int notify_send_fd_ = -1;

         struct rtcServerOptions options_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_WORKER_H_