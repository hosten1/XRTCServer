#include "server/signaling_work.h"

#include <unistd.h>

#include <rtc_base/logging.h>
#include "signaling_work.h"
#include "base/socket.h"

namespace lrtc
{
    void signaling_server_recv_notify(EventLoop */*el*/, IOWatcher */*w*/, int fd, int /*events*/, void *data)
    {
        RTC_LOG(LS_VERBOSE) << "signaling server recv notify";
        int msg;
        if (read(fd, &msg, sizeof(msg)) != sizeof(int))
        {
            RTC_LOG(LS_WARNING) << "read from pipe error :"<< strerror(errno) << " ,errorno:"<< errno;;
            return;
        }
        SignalingWork *worker = (SignalingWork*)data;
        worker->on_recv_notify(msg);
    }
    void conn_io_cb(EventLoop * /*el*/, IOWatcher * /*w*/, int fd, int events, void *data)
    {
        SignalingWork *worker = (SignalingWork *)data;
        if (events & EventLoop::READ)
        {
            worker->_read_query(fd);
        }
        
        
    }

    SignalingWork::SignalingWork(int work_id) : work_id_(work_id),
                                                el_(std::make_unique<EventLoop>(this))
    {
    }

    SignalingWork::~SignalingWork()
    {
    }

    int SignalingWork::init()
    {
        RTC_LOG(LS_INFO)<<"signaling worker init worker id :"<<work_id_;
        // 创建管道 用于线程间通讯
        int fds[2];
        if (pipe(fds) == -1)
        {
            RTC_LOG(LS_ERROR) << "create pipe eror :" << strerror(errno) << " ,errorno:" << errno;
            return -1;
        }
        notify_recv_fd_ = fds[0];
        notify_send_fd_ = fds[1];
         //将线程通讯的notifi_recv_fd添加到事件循环里，进行管理
         pipe_watcher_ = el_->create_io_event(signaling_server_recv_notify,this);
         el_->start_io_event(pipe_watcher_,notify_recv_fd_,EventLoop::READ);
        return 0;
    }

    bool SignalingWork::start()
    {
        RTC_LOG(LS_INFO) << "signaling worker start worker id :"<<work_id_;
        if (ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "signaling worker is running";
            return false; // 修改返回值为 false，以保持返回类型的一致性
        }

        try
        {
            ev_thread_ = std::make_unique<std::thread>([=]()
                                                       {
            RTC_LOG(LS_INFO) << "signaling worker event loop run";
            try {
                el_->start(); // 假设 start 可能抛出异常，进行内部异常处理
                RTC_LOG(LS_INFO) << "signaling worker event loop stop";
            } catch (const std::exception& e) {
                // 记录或处理异常
                RTC_LOG(LS_ERROR) << "Event loop start failed: " << e.what();
            } });
        }
        catch (const std::exception &e)
        {
            // 处理线程创建异常
            RTC_LOG(LS_ERROR) << "Thread creation failed: " << e.what();
            return false;
        }

        return true;
    }
    // 实现线程里面的循环停止
    int SignalingWork::stop()
    {
        RTC_LOG(LS_INFO) << "signaling worker stop from other event  worker id :"<<work_id_;
       return _notify(SignalingWork::MSG_QUIT);
    }

    void SignalingWork::joined()
    {
        RTC_LOG(LS_INFO) << "signaling server joined";
        if (ev_thread_ && ev_thread_->joinable())
        {
            ev_thread_->join();
        }
    }

    int SignalingWork::notify_new_conn(int fd)
    {
        //使用队列存储消息的内容
        RTC_LOG(LS_INFO) << "SignalingWork::notify_new_conn() notify new conn fd:" << fd;
        notify_queue_.produce(fd);
       return _notify(SignalingWork::MSG_NEW_CONN);
    }

    int SignalingWork::_notify(int msg)
    {
       RTC_LOG(LS_INFO) << "signaling worker notify msg:" << msg;
       int ret = write(notify_send_fd_, &msg, sizeof(msg));
       return ret == sizeof(msg) ? 0 : -1;
    }
    void SignalingWork::on_recv_notify(int msg)
    {
        RTC_LOG(LS_INFO) << "signaling server notify msg:" << msg;
        switch (msg)
        {
        case SignalingWork::MSG_QUIT:
            _stop();
            break;
        case SignalingWork::MSG_NEW_CONN:
             int fd;
             if (notify_queue_.consumer(&fd))
             {
                _accept_new_connection(fd);

             }
             RTC_LOG(LS_INFO)<<"signaling server _accept_new_connection fd:"<<fd;
            break;

        default:
            RTC_LOG(LS_WARNING) << "signaling server recv unknown msg:" << msg;
            break;
        }
    }
    void SignalingWork::_stop(){
        RTC_LOG(LS_INFO) << "signaling server _stop begin worker id :"<<work_id_;
        if (!ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "_stop() signaling server is not running worker id :"<<work_id_;
            return;
        }
        el_->delete_io_event(pipe_watcher_);
        el_->stop();

        close(notify_recv_fd_);
        close(notify_send_fd_);

        RTC_LOG(LS_INFO) << "signaling server _stop end";
        
    }

    void SignalingWork::_accept_new_connection(int fd)
    {
        RTC_LOG(LS_INFO) << "SignalingWork::_accept_new_connection() [fd:"<< fd 
                            << ", work_id : "<<work_id_  << "]";
        if (fd < 0)
        {
            RTC_LOG(LS_ERROR) << "invalid fd:"<< fd 
                            << ", work_id : "<<work_id_<<" is invalid";
            return;
        }
        // 设置套接字为非阻塞
        sock_setnonblock(fd);
        // 设置为非延时
        sock_setnodelay(fd);
        // fd封装成connection
        // 创建一个指向RTPConection对象的unique_ptr
        std::unique_ptr<TcpConnection> conn = std::make_unique<TcpConnection>(fd);
        // // 创建io_watcher并设置
        conn->io_watcher_ = el_->create_io_event(conn_io_cb, this);
        el_->start_io_event(conn->io_watcher_, fd, EventLoop::READ);
        if((size_t)fd > conn_tcps_.size()){
            conn_tcps_.reserve(fd * 2);
        }
        // 将conn移动到conn_tcps_容器中，使用fd作为键
        conn_tcps_[fd] = std::move(conn);
    }

    void SignalingWork::_read_query(const int fd)
    {
         RTC_LOG(LS_INFO) << "SignalingWork::_read_query() fd:" << fd
                         << ", work_id : " << work_id_;
        if (fd < 0 /*|| (size_t)fd > conn_tcps_.size()*/)
        {
             RTC_LOG(LS_ERROR) << "invalid fd:" << fd
                              << ", work_id : " << work_id_ << " is invalid";
            // Output all keys
              RTC_LOG(LS_INFO) << "All keys in conn_tcps_:"; 
             for (const auto &pair : conn_tcps_)
             {
                  RTC_LOG(LS_INFO) << pair.first;
             }
            return;
        }

        TcpConnection *conn = conn_tcps_[fd].get();
        if (!conn)
        {
            RTC_LOG(LS_ERROR) << "invalid conn fd:" << fd
                              << ", work_id : " << work_id_ << " is invalid";
            return;
        }
        conn->read(fd);

    }

} // namespace lrtc


