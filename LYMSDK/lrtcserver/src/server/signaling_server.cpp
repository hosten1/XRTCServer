#include "server/signaling_server.h"

#include <unistd.h>
#include "signaling_server.h"
#include <yaml-cpp/yaml.h>
#include <rtc_base/logging.h>
#include "base/socket.h"

namespace lrtc {
    void accep_new_conn(EventLoop *el, IOWatcher *w, int fd, int events, void *data)
    {
        RTC_LOG(LS_VERBOSE) << "signaling server start";
        
        
        // server->accept_cb(fd);
    }
    void signaling_server_recv_notifi_cb(EventLoop *el, IOWatcher *w, int fd, int events, void *data)
    {
        RTC_LOG(LS_VERBOSE) << "signaling server recv notify";
        int msg;
        if (read(fd, &msg, sizeof(msg)) != sizeof(int))
        {
            RTC_LOG(LS_WARNING) << "read from pipe error :"<< strerror(errno) << " ,errorno:"<< errno;;
            return;
        }
        SignalingServer *server = (SignalingServer*)data;
        server->on_recv_notify(msg);
    }
    SignalingServer::SignalingServer():loop_(std::make_unique<EventLoop>(this))
    {
        RTC_LOG(LS_INFO) << "signaling server constructor";
    }

    SignalingServer::~SignalingServer()
    {
        RTC_LOG(LS_INFO) << "signaling server destructor";
    }
    int SignalingServer::init(const char *conf_file)
    {
        RTC_LOG(LS_INFO) << "signaling server init";
        if (!conf_file)
        {
           RTC_LOG(LS_WARNING) << "signaling server conf_file is null";
           return -1;
        }
        try
        {
            YAML::Node config = YAML::LoadFile(conf_file);
            options_.host = config["host"].as<std::string>();
            options_.port = config["port"].as<int>();
            options_.worker_num = config["worker_num"].as<int>();
            options_.connect_timeout = config["connection_timeout"].as<int>();
            RTC_LOG(LS_INFO) << "signaling server conf = " << config
                             << " options_.host = " << options_.host
                             << " options_.port = " << options_.port;

        }
        catch(const YAML::Exception& e)
        {
            RTC_LOG(LS_ERROR) << "catch a YAML::Exeption,err: " << e.what()
                              << " line : " << e.mark.line + 1 << " col:" << e.mark.column + 1
                              << '\n';

            return -1;
         }
         //创建管道 用于线程间通讯
         int fds[2];
         if (pipe(fds) == -1)
         {
             RTC_LOG(LS_ERROR) << "create pipe eror :" << strerror(errno) << " ,errorno:"<< errno;
             return -1;
         }
         notify_recv_fd_ = fds[0];
         notify_send_fd_ = fds[1];

         //将线程通讯的notifi_recv_fd添加到事件循环里，进行管理
         pipe_watcher_ = loop_->create_io_event(signaling_server_recv_notifi_cb,this);
         loop_->start_io_event(pipe_watcher_,notify_recv_fd_,EventLoop::READ);
         // 创建tcp socket
         listen_fd_ = Create_tcp_server(options_.host.c_str(),options_.port);
         io_watcher_ = loop_->create_io_event(accep_new_conn,this);
         loop_->start_io_event(io_watcher_,listen_fd_,EventLoop::READ);
         
         return 0;

    }
    bool SignalingServer::start()
    {
        RTC_LOG(LS_INFO) << "signaling server start";
        if (ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "signaling server is running";
            return false; // 修改返回值为 false，以保持返回类型的一致性
        }

        try
        {
            ev_thread_ = std::make_unique<std::thread>([=]()
                                                       {
            RTC_LOG(LS_INFO) << "signaling server event loop run";
            try {
                loop_->start(); // 假设 start 可能抛出异常，进行内部异常处理
                RTC_LOG(LS_INFO) << "signaling server event loop stop";
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
    int SignalingServer::stop()
    {
        RTC_LOG(LS_INFO) << "signaling server stop from other event";
       return notify(SignalingServer::MSG_QUIT);
    }
    int SignalingServer::notify(int msg)
    {
        RTC_LOG(LS_VERBOSE) << "signaling server notify msg:" << msg;
        int ret = write(notify_send_fd_, &msg, sizeof(msg));
        return ret == sizeof(msg) ? 0 : -1;
    }
    void SignalingServer::on_recv_notify(int msg)
    {
      RTC_LOG(LS_VERBOSE) << "signaling server notify msg:" << msg;
       switch (msg)
       {
       case SignalingServer::MSG_QUIT:
        _stop();
        break;
       
       default:
       RTC_LOG(LS_WARNING) << "signaling server recv unknown msg:" << msg;
        break;
       }
    }
    void SignalingServer::_stop(){
        RTC_LOG(LS_INFO) << "signaling server _stop begin";
        if (!ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "_stop() signaling server is not running";
            return;
        }
        loop_->delete_io_event(pipe_watcher_);
        loop_->delete_io_event(io_watcher_);
        loop_->stop();

        close(notify_recv_fd_);
        close(notify_send_fd_);
        close(listen_fd_);

        RTC_LOG(LS_INFO) << "signaling server _stop end";
        
    }
    void SignalingServer::joined(){
        RTC_LOG(LS_INFO) << "signaling server joined";
        if (ev_thread_)
        {
            ev_thread_->join();
        }
        
    }
} // namespace lrtc
