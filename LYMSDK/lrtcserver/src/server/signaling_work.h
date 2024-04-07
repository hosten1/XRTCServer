#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_WORK_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_WORK_H_
#include <memory>
#include <thread>
#include <vector>
// #include <unordered_map>
#include <mutex>
#include <atomic>
#include <queue>

#include "base/event_loop.h"
#include "base/lock_free_queue.h"
#include "server/tcp_connection.h"
#include "server/signaling_server_options.h"

namespace lrtc
{
    class SignalingWork
    {

    public:
        enum
        {
            MSG_QUIT = 0,
            MSG_NEW_CONN = 1,
            MSG_RTC_MSG = 2,
        };

        SignalingWork(int work_id,const struct SignalingServerOptions option);

        ~SignalingWork();
        int init();
        bool start();
        int stop();
        void joined();
        int notify_new_conn(int fd);

        int send_rtc_msg(const std::shared_ptr<LRtcMsg> rtc_msg);
        void push_msg(std::shared_ptr<LRtcMsg> rtc_msg);
        std::shared_ptr<LRtcMsg> pop_msg();

        friend void signaling_server_recv_notify(EventLoop *el, IOWatcher *w, int fd,
                                                 int events, void *data);
        friend void conn_io_cb(EventLoop * /*el*/, IOWatcher * /*w*/, 
                                                 int fd, int events, void *data);
        friend void  conn_timer_cb(EventLoop *el, TimerWatcher * /*w*/, void *data);

    private:
        int _notify(int msg);
        void on_recv_notify(int msg);
        void _stop();
        void _accept_new_connection(const int fd);
        void _read_query(int fd);
        #ifdef USE_SDS
        int _process_queue_buffer(const TcpConnection *conn);
        int _process_request(const TcpConnection *conn,const rtc::Slice* header,const rtc::Slice* body);
        #endif // USE_SDS
        void _close_connection( TcpConnection *conn);
        void _remove_connection( TcpConnection *conn);
        void _process_timeout(TcpConnection *conn);
        int _process_request_msg(TcpConnection * conn, Json::Value root, uint32_t log_id);
        int _process_request_push_msg(TcpConnection * conn,int cmdno, Json::Value root, uint32_t log_id);
        void _process_rtc_msg();
        void _response_server_offer(std::shared_ptr<LRtcMsg> rtc_msg);
    #ifdef USE_SDS
         void _add_reply_conn(TcpConnection* conn,const rtc::Slice& repaly);
    #else
         void _add_reply_conn(TcpConnection* conn,const rtc::ByteBufferWriter &writer);
    #endif
    void _write_repaly(int fd);
    private:
        int work_id_;
        std::unique_ptr<EventLoop> el_;

        IOWatcher *pipe_watcher_ = nullptr;
        std::unique_ptr<std::thread> ev_thread_;
        LockFreeQueue<int> notify_queue_;

        int notify_recv_fd_ = -1;
        int notify_send_fd_ = -1;

        std::vector<TcpConnection*> conn_tcps_;

        struct SignalingServerOptions options_;

        // std::vector<TcpConnection*> conns_;
        std::queue<std::shared_ptr<LRtcMsg>> q_msg_;
        std::mutex q_mtx_;

    };

} // namespace signaling_work

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_WORK_H_
