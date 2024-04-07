#include "server/rtc_server.h"

#include <unistd.h>

#include <yaml-cpp/yaml.h>
#include "rtc_base/logging.h"
#include "rtc_server.h"

namespace lrtc
{
    void rtc_server_recv_notify(EventLoop * /*el*/, IOWatcher * /*w*/,
                                int fd, int events, void *data)
    {
        int msg;
        if (read(fd, &msg, sizeof(int)) != sizeof(int))
        {
            RTC_LOG(LS_ERROR) << "read notify fd error:" << strerror(errno) << ", errno:" << errno;
            return;
        }
        RTC_LOG(LS_INFO) << "recv notify msg:" << msg;

        RtcServer *server = (RtcServer *)data;
        server->_process_notify(msg);
    }

    RtcServer::RtcServer() : event_loop_(std::make_unique<EventLoop>(this))
    {
    }
    RtcServer::~RtcServer()
    {
        RTC_LOG(LS_INFO) << "rtc server destructor";
        if (event_loop_)
        {
             event_loop_.reset();
             event_loop_ = nullptr;
        }
        if (ev_thread_)
        {
            ev_thread_.reset();
             ev_thread_ = nullptr;
        }
        for(auto& work : workers_)
        {
            if (work)
            {
               work.reset();
               work = nullptr;
            }
            
        }
        workers_.clear();
    }
    bool RtcServer::start()
    {
        RTC_LOG(LS_INFO) << "RtcServer::start() begin";
        if (ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "RtcServer::start() is running";
            return false; // 修改返回值为 false，以保持返回类型的一致性
        }

        try
        {
            ev_thread_ = std::make_unique<std::thread>([=]()
                                                       {
            RTC_LOG(LS_INFO) << "RtcServer::start() event loop start >>>  ";
            try {
                event_loop_->start(); // 假设 start 可能抛出异常，进行内部异常处理
                RTC_LOG(LS_INFO) << "RtcServer::start() event loop stop";
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
    int RtcServer::stop()
    {
        RTC_LOG(LS_INFO) << "RtcServer::stop() stop from other event  worker id ";
        return _notify(RtcServer::MSG_QUIT);
    }

    void RtcServer::joined()
    {
        RTC_LOG(LS_INFO) << "RtcServer::joined()";
        if (ev_thread_ && ev_thread_->joinable())
        {
            ev_thread_->join();
        }
    }
    int RtcServer::send_rtc_msg(const std::shared_ptr<LRtcMsg> rtc_msg)
    {
        push_msg(rtc_msg);

        return _notify(MSG_RTC_MSG);
    }
    void RtcServer::push_msg(std::shared_ptr<LRtcMsg> rtc_msg)
    {
        std::unique_lock<std::mutex> lock(q_thread_mutex_);
        q_msgs_.push(rtc_msg);
    }
    const std::shared_ptr<LRtcMsg> RtcServer::pop_msg()
    {
        std::unique_lock<std::mutex> lock(q_thread_mutex_);
        if (q_msgs_.empty())
            return nullptr;
        std::shared_ptr<LRtcMsg> msg = q_msgs_.front();
        q_msgs_.pop();
        return msg;
    }
    int RtcServer::init(const char *conf_file)
    {
        // 空指针判断
        if (!conf_file)
        {
            RTC_LOG(LS_ERROR) << "rtc server conf file is null";
            return -1;
        }
        try
        {
            YAML::Node conf = YAML::LoadFile(conf_file);
            RTC_LOG(LS_INFO) << "rtc server option conf:" << conf;
            options_.worker_num = conf["worker_num"].as<int>();
        }
        catch (const YAML::Exception &e)
        {
            RTC_LOG(LS_ERROR) << "rtc server load conf file failed, " << e.what();
            return -1;
        }

        // 创建管道 用于线程间通讯
        int fds[2];
        if (pipe(fds) == -1)
        {
            RTC_LOG(LS_ERROR) << "create pipe eror :" << strerror(errno) << " ,errorno:" << errno;
            return -1;
        }
        notify_recv_fd_ = fds[0];
        notify_send_fd_ = fds[1];
        pipe_watcher_ = event_loop_->create_io_event(rtc_server_recv_notify, this);
        event_loop_->start_io_event(pipe_watcher_, notify_recv_fd_, EventLoop::READ);

        for (int i = 0; i < options_.worker_num; i++)
        {
            if (_create_worker(i) != 0)
            {
                RTC_LOG(LS_ERROR) << "create worker failed";
                return -1;
            }
        }
        return 0;
    }

    int RtcServer::_notify(int msg)
    {
        RTC_LOG(LS_INFO) << "signaling worker notify msg:" << msg;
        int ret = write(notify_send_fd_, &msg, sizeof(msg));
        return ret == sizeof(msg) ? 0 : -1;
    }

    void RtcServer::_process_notify(int msg)
    {
        switch (msg)
        {
        case MSG_QUIT:
            _stop();
            break;
        case MSG_RTC_MSG:
            _process_rtc_msg();
            break;

        default:
            RTC_LOG(LS_ERROR) << "rtc server recv unknown msg:" << msg;
            break;
        }
    }
    void RtcServer::_stop()
    {
        RTC_LOG(LS_INFO) << "RtcServer::_stop() begin----";
        if (!ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "_stop() rtc server is not running ";
            return;
        }
        event_loop_->delete_io_event(pipe_watcher_);
        event_loop_->stop();

        close(notify_recv_fd_);
        close(notify_send_fd_);
        for (auto &worker : workers_)
        {
            if (worker)
            {
                worker->stop();
                worker->joined();
            }
        }

        RTC_LOG(LS_INFO) << "RtcServer::_stop() end";
    }

    void RtcServer::_process_rtc_msg()
    {
        std::shared_ptr<LRtcMsg> rtc_msg = pop_msg();
        if (!rtc_msg)
        {
            RTC_LOG(LS_ERROR) << "rtc server recv rtc msg is null";
            return;
        }
        RTC_LOG(LS_INFO) << "rtc server recv rtc msg:" << rtc_msg->toString();
    }

    int RtcServer::_create_worker(int work_id)
    {
        RTC_LOG(LS_INFO) << "RtcServer::_create_worker() begin ,worker_id:" << work_id;
        // 验证worker_id的有效性
        if (work_id < 0)
        {
            RTC_LOG(LS_ERROR) << "Invalid worker_id: " << work_id;
            return -1; // 使用具体的错误码可能更合适，这里保持为-1
        }
        std::unique_ptr<RtcWorker> worker = std::make_unique<RtcWorker>(work_id,options_);
        if (worker->init() != 0)
        { // 使用了更明确的判断条件
            RTC_LOG(LS_ERROR) << "init worker error worker_id:" << work_id;
            return -1;
        }
        if (!worker->start())
        {
            RTC_LOG(LS_ERROR) << "start worker error worker_id:" << work_id;
            return -1;
        }

        workers_.push_back(std::move(worker));
        return 0; // 成功时返回0
    }

} // namespace lrtc