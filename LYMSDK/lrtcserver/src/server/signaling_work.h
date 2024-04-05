#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_WORK_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_WORK_H_
#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>

#include "base/event_loop.h"
#include "base/lock_free_queue.h"
#include "server/tcp_connection.h"


namespace lrtc
{
    class SignalingWork
    {

    public:
        enum
        {
            MSG_QUIT = 0,
            MSG_NEW_CONN = 1,
        };
        SignalingWork(int work_id);
        ~SignalingWork();
        int init();
        bool start();
        int stop();
        void joined();
        int notify_new_conn(int fd);

        friend void signaling_server_recv_notify(EventLoop *el, IOWatcher *w, int fd,
                                                 int events, void *data);
        friend void conn_io_cb(EventLoop * /*el*/, IOWatcher * /*w*/, 
                                                 int fd, int events, void *data);

    private:
        int _notify(int msg);
        void on_recv_notify(int msg);
        void _stop();
        void _accept_new_connection(const int fd);
        void _read_query(int fd);

    private:
        int work_id_;
        std::unique_ptr<EventLoop> el_;

        IOWatcher *pipe_watcher_ = nullptr;
        std::unique_ptr<std::thread> ev_thread_;
        LockFreeQueue<int> notify_queue_;

        int notify_recv_fd_ = -1;
        int notify_send_fd_ = -1;

        std::unordered_map<int, std::unique_ptr<TcpConnection>> conn_tcps_;
    };

} // namespace signaling_work

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_SIGNALING_WORK_H_
