#include "server/rtc_worker.h"

#include <unistd.h>

#include "base/event_loop.h"
#include "stream/rtp_stream_manager.h"

#include <rtc_base/logging.h>
#include <rtc_base/rtc_certificate.h>
#include "base/socket.h"
#include "rtc_worker.h"
#include "server/signaling_work.h"
#include "server/tcp_connection.h"

namespace lrtc
{
    void rtc_worker_recv_notify(EventLoop * /*el*/, IOWatcher * /*w*/, int fd, int /*events*/, void *data)
    {
        RTC_LOG(LS_VERBOSE) << "signaling server recv notify";
        int msg;
        if (read(fd, &msg, sizeof(msg)) != sizeof(int))
        {
            RTC_LOG(LS_WARNING) << "read from pipe error :" << strerror(errno)
                                << " ,errorno:" << errno;
            ;
            return;
        }
        RtcWorker *worker = (RtcWorker *)data;
        worker->on_recv_notify(msg);
    }
    void rtc_worker_conn_io_cb(EventLoop * /*el*/, IOWatcher * /*w*/, int fd, int events, void *data)
    {
        RtcWorker *worker = (RtcWorker *)data;
        if (events & EventLoop::READ)
        {
            worker->_read_query(fd);
        }
    }
    void rtc_worker_conn_timer_cb(EventLoop *el, TimerWatcher * /*w*/, void *data)
    {
        RtcWorker *worker = (RtcWorker *)el->get_owner();
        TcpConnection *conn = (TcpConnection *)data;
        worker->_process_timeout(conn);
    }

    RtcWorker::RtcWorker(int work_id, const struct RtcServerOptions &option) : work_id_(work_id),
                                                                               el_(std::make_unique<EventLoop>(this)),
                                                                               options_(option),
                                                                               rtp_stream_manager_(new RTPStreamManager(el_.get()))
    {
    }
    RtcWorker::~RtcWorker()
    {
        // 使用范围循环遍历unordered_map并释放资源
        // for (auto &pair : conn_tcps_)
        // {
        //     if ( pair.second)
        //     {
        //         _close_connection(pair.second.get()); // 如果 _close_connection 需要的是指针，则使用 pair.second.get() 获取指针
        //     }

        //     pair.second.reset(); // 释放 TcpConnection 对象的内存
        // }
        // conn_tcps_.clear(); // 清空unordered_map
        if (el_)
        {
            el_.reset();
            el_ = nullptr;
        }
        if (ev_thread_)
        {
            ev_thread_.reset();
            ev_thread_ = nullptr;
        }
    }

    int RtcWorker::init()
    {
        RTC_LOG(LS_INFO) << "RtcWorker worker init worker id :" << work_id_;
        // 创建管道 用于线程间通讯
        int fds[2];
        if (pipe(fds) == -1)
        {
            RTC_LOG(LS_ERROR) << "create pipe eror :" << strerror(errno) << " ,errorno:" << errno;
            return -1;
        }
        notify_recv_fd_ = fds[0];
        notify_send_fd_ = fds[1];
        // 将线程通讯的notifi_recv_fd添加到事件循环里，进行管理
        pipe_watcher_ = el_->create_io_event(rtc_worker_recv_notify, this);
        el_->start_io_event(pipe_watcher_, notify_recv_fd_, EventLoop::READ);
        return 0;
    }
    bool RtcWorker::start()
    {
        RTC_LOG(LS_INFO) << "RtcWorker worker start worker id :" << work_id_;
        if (ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "RtcWorker worker is running";
            return false; // 修改返回值为 false，以保持返回类型的一致性
        }

        try
        {
            ev_thread_ = std::make_unique<std::thread>([=]()
                                                       {
            RTC_LOG(LS_INFO) << "RtcWorker worker event loop start >>> worker id :"<<work_id_;
            try {
                el_->start(); // 假设 start 可能抛出异常，进行内部异常处理
                RTC_LOG(LS_INFO) << "RtcWorker worker event loop stop worker id :"<<work_id_;
            } catch (const std::exception& e) {
                // 记录或处理异常
                RTC_LOG(LS_ERROR) << "Event loop start failed: " << e.what() << ", worker id :"<<work_id_;
            } });
        }
        catch (const std::exception &e)
        {
            // 处理线程创建异常
            RTC_LOG(LS_ERROR) << "Thread creation failed: " << e.what() << ", worker id :" << work_id_;
            ;
            return false;
        }

        return true;
    }
    int RtcWorker::stop()
    {
        RTC_LOG(LS_INFO) << "RtcWorker worker stop from other event  worker id :" << work_id_;
        return _notify(RtcWorker::MSG_QUIT);
    }

    void RtcWorker::joined()
    {
        RTC_LOG(LS_INFO) << "RtcWorker::joined() , worker id :" << work_id_;
        if (ev_thread_ && ev_thread_->joinable())
        {
            ev_thread_->join();
        }
    }

    void RtcWorker::push_msg(std::shared_ptr<LRtcMsg> rtc_msg)
    {
        q_msg_.produce(rtc_msg);
    }

    bool RtcWorker::pop_msg(std::shared_ptr<LRtcMsg> *rtc_msg)
    {
        if (q_msg_.consumer(rtc_msg))
        {
            return true;
        }
        return false;
    }

    int RtcWorker::notify_new_conn(int fd)
    {
        // 使用队列存储消息的内容
        RTC_LOG(LS_INFO) << "RtcWorker Work::notify_new_conn() notify new conn fd:" << fd;
        //     notify_queue_.produce(fd);
        //    return _notify(RtcWorker::MSG_NEW_CONN);
        return 0;
    }

    int RtcWorker::send_rtc_msg(const std::shared_ptr<LRtcMsg> rtc_msg)
    {
        // 将消息 放到 worker队列
        push_msg(rtc_msg);
        return _notify(RtcWorker::MSG_RTC_MSG);

        return 0;
    }
    int RtcWorker::_notify(int msg)
    {
        RTC_LOG(LS_INFO) << "RtcWorker worker notify msg:" << msg;
        int ret = write(notify_send_fd_, &msg, sizeof(msg));
        return ret == sizeof(msg) ? 0 : -1;
    }

    void RtcWorker::on_recv_notify(int msg)
    {
        RTC_LOG(LS_INFO) << "RtcWorker notify msg:" << msg << ", worker id :" << work_id_;
        ;
        switch (msg)
        {
        case RtcWorker::MSG_QUIT:
            _stop();
            break;
        case RtcWorker::MSG_RTC_MSG:
            _process_rtc_msg();
            break;

        default:
            RTC_LOG(LS_WARNING) << "RtcWorker server recv unknown msg:" << msg;
            break;
        }
    }

    void RtcWorker::_stop()
    {
        RTC_LOG(LS_INFO) << "RtcWorker server _stop begin worker id :" << work_id_;
        if (!ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "_stop() RtcWorker server is not running worker id :" << work_id_;
            return;
        }
        el_->delete_io_event(pipe_watcher_);
        el_->stop();

        close(notify_recv_fd_);
        close(notify_send_fd_);

        RTC_LOG(LS_INFO) << "RtcWorker server _stop end , worker id :" << work_id_;
    }
    void RtcWorker::_process_push_rtcmsg(std::shared_ptr<LRtcMsg> rtcmsg)
    {
        std::string offer = "offer";
        int ret = rtp_stream_manager_->create_push_stream(rtcmsg, offer);
        RTC_LOG(LS_INFO) << "RtcWorker server _process_push_rtcmsg create_push_stream ret:" << ret << ",offer:" << offer;
        rtcmsg->sdp = offer;

        SignalingWork *signaling_work = (SignalingWork *)rtcmsg->signalingWorker;
        RTC_LOG(LS_INFO) << "RtcWorker::_process_push_rtcmsg,signaling_work.id:" << signaling_work->get_work_id()
                         << " rtcmsg.signalingWorkerid:" << rtcmsg->signalingWorkerId;
        if (signaling_work)
        {
            signaling_work->send_rtc_msg(rtcmsg);
        }

        // 将结果返回给client
    }
    void RtcWorker::_process_rtc_msg()
    {
        std::shared_ptr<LRtcMsg> rtcmsg;
        if (!pop_msg(&rtcmsg))
        {
            return;
        }
        RTC_LOG(LS_INFO) << "RtcWorker server _process_rtc_msg begin worker id :" << work_id_
                         << " msg:" << rtcmsg->toString();
        switch (rtcmsg->cmdno)
        {
        case CMDNUM_PUSH:
            _process_push_rtcmsg(rtcmsg);
            break;

        default:
            RTC_LOG(LS_WARNING) << "RtcWorker server _process_rtc_msg unknown msg:" << rtcmsg->toString();
            break;
        }
    }

    void RtcWorker::_read_query(int /*fd*/)
    {
    }

    void RtcWorker::_close_connection(TcpConnection * /*conn*/)
    {
    }

    void RtcWorker::_remove_connection(TcpConnection * /*conn*/)
    {
    }

    void RtcWorker::_process_timeout(TcpConnection * /*conn*/)
    {
    }

    int RtcWorker::_process_request_msg(TcpConnection * /*conn*/, Json::Value /*oot*/, uint32_t /*log_id*/)
    {
        return 0;
    }

} // namespace lrtc