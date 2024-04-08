#include "server/signaling_work.h"

#include <unistd.h>

#include <rtc_base/logging.h>
#include "base/socket.h"
#include "server/rtc_server.h"
#include "signaling_work.h"

extern lrtc::RtcServer *g_rtc_server;

namespace lrtc
{
    void signaling_server_recv_notify(EventLoop * /*el*/, IOWatcher * /*w*/, int fd, int /*events*/, void *data)
    {
        RTC_LOG(LS_VERBOSE) << "signaling server recv notify";
        int msg;
        if (read(fd, &msg, sizeof(msg)) != sizeof(int))
        {
            RTC_LOG(LS_WARNING) << "read from pipe error :" << strerror(errno) << " ,errorno:" << errno;
            ;
            return;
        }
        SignalingWork *worker = (SignalingWork *)data;
        worker->on_recv_notify(msg);
    }
    void conn_io_cb(EventLoop * /*el*/, IOWatcher * /*w*/, int fd, int events, void *data)
    {
        SignalingWork *worker = (SignalingWork *)data;
        if (events & EventLoop::READ)
        {
            worker->_read_query(fd);
        }
        if (events & EventLoop::WRITE)
        {
            worker->_write_repaly(fd);
        }
    }
    void conn_timer_cb(EventLoop *el, TimerWatcher * /*w*/, void *data)
    {
        SignalingWork *worker = (SignalingWork *)el->get_owner();
        TcpConnection *conn = (TcpConnection *)data;
        worker->_process_timeout(conn);
    }

    SignalingWork::SignalingWork(int work_id, const struct SignalingServerOptions option) : work_id_(work_id),
                                                                                            el_(std::make_unique<EventLoop>(this)), options_(option)
    {
    }

    SignalingWork::~SignalingWork()
    {
        // 释放所有元素
        for (auto &conn : conn_tcps_)
        {
            if (conn)
            {
                _close_connection(conn);
                delete conn;
            }
        }
        conn_tcps_.clear(); // 清空容器
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

    int SignalingWork::init()
    {
        RTC_LOG(LS_INFO) << "signaling worker init worker id :" << work_id_;
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
        pipe_watcher_ = el_->create_io_event(signaling_server_recv_notify, this);
        el_->start_io_event(pipe_watcher_, notify_recv_fd_, EventLoop::READ);
        return 0;
    }

    bool SignalingWork::start()
    {
        RTC_LOG(LS_INFO) << "signaling worker start worker id :" << work_id_;
        if (ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "signaling worker is running , worker id :" << work_id_;
            ;
            return false; // 修改返回值为 false，以保持返回类型的一致性
        }

        try
        {
            ev_thread_ = std::make_unique<std::thread>([=]()
                                                       {
            RTC_LOG(LS_INFO) << "signaling worker event loop start >>> , worker id :"<<work_id_;
            try {
                el_->start(); // 假设 start 可能抛出异常，进行内部异常处理
                RTC_LOG(LS_INFO) << "signaling worker event loop stop , worker id :"<<work_id_;
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
        RTC_LOG(LS_INFO) << "signaling worker stop from other event  worker id :" << work_id_;
        return _notify(SignalingWork::MSG_QUIT);
    }

    void SignalingWork::joined()
    {
        RTC_LOG(LS_INFO) << "SignalingWork::joined()"
                         << ", worker id :" << work_id_;
        ;
        if (ev_thread_ && ev_thread_->joinable())
        {
            ev_thread_->join();
        }
    }

    int SignalingWork::notify_new_conn(int fd)
    {
        // 使用队列存储消息的内容
        RTC_LOG(LS_INFO) << "Signaling Work::notify_new_conn() notify new conn fd:" << fd << ", worker id :" << work_id_;
        ;
        notify_queue_.produce(fd);
        return _notify(SignalingWork::MSG_NEW_CONN);
    }

    int SignalingWork::send_rtc_msg(const std::shared_ptr<LRtcMsg> rtc_msg)
    {
        RTC_LOG(LS_INFO) << "Signaling Work::send_rtc_msg() send rtc msg:"<<rtc_msg->toString()
                            <<", worker id :" << work_id_;
        push_msg(rtc_msg);

        return _notify(SignalingWork::MSG_RTC_MSG);
    }
    void SignalingWork::push_msg(std::shared_ptr<LRtcMsg> rtc_msg)
    {
        std::unique_lock<std::mutex> lock(q_mtx_);
        q_msg_.push(rtc_msg);
    }
    std::shared_ptr<LRtcMsg> SignalingWork::pop_msg()
    {
        std::unique_lock<std::mutex> lock(q_mtx_);
        if (q_msg_.empty())
        {
            return nullptr;
        }
        std::shared_ptr<LRtcMsg> rtc_msg = q_msg_.front();
        q_msg_.pop();

        return rtc_msg;
    }
    int SignalingWork::_notify(int msg)
    {
        RTC_LOG(LS_INFO) << "signaling worker notify msg:" << msg << ", worker id :" << work_id_;;
        int ret = write(notify_send_fd_, &msg, sizeof(msg));
        return ret == sizeof(msg) ? 0 : -1;
    }
    void SignalingWork::on_recv_notify(int msg)
    {
        RTC_LOG(LS_INFO) << "SignalingWork notify msg:" << msg << ", worker id :" << work_id_;
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
            RTC_LOG(LS_INFO) << "signaling server on_recv_notify fd:" << fd << ", worker id :" << work_id_;;
            break;
        case MSG_RTC_MSG:
            _process_rtc_msg();
            break;
        default:
            RTC_LOG(LS_WARNING) << "signaling server recv unknown msg:" << msg << ", worker id :" << work_id_;
            ;
            break;
        }
    }
    void SignalingWork::_stop()
    {
        RTC_LOG(LS_INFO) << "signaling server _stop begin worker id :" << work_id_;
        if (!ev_thread_)
        {
            RTC_LOG(LS_WARNING) << "_stop() signaling server is not running worker id :" << work_id_;
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
        RTC_LOG(LS_INFO) << "SignalingWork::_accept_new_connection() [fd:" << fd
                         << ", work_id : " << work_id_ << "]";
        if (fd < 0)
        {
            RTC_LOG(LS_ERROR) << "invalid fd:" << fd
                              << ", work_id : " << work_id_ << " is invalid";
            return;
        }
        // 设置套接字为非阻塞
        sock_setnonblock(fd);
        // 设置为非延时
        sock_setnodelay(fd);
        // fd封装成connection
        // 创建一个指向RTPConection对象的unique_ptr
        TcpConnection* conn =  new TcpConnection(fd);
        // // 创建io_watcher并设置
        conn->io_watcher_ = el_->create_io_event(conn_io_cb, this);
        el_->start_io_event(conn->io_watcher_, fd, EventLoop::READ);
        // 创建计时器
        conn->timer_watcher_ = el_->create_timer_event(conn_timer_cb, conn, true);
        el_->start_timer_event(conn->timer_watcher_, 100000); // 100ms执行一次
        // 首次连接创建的时间
        conn->set_last_interaction_time(el_->now_time_usec());

        if ((size_t)fd > conn_tcps_.size())
        {
            conn_tcps_.reserve(fd * 2);
        }
        // 将conn移动到conn_tcps_容器中，使用fd作为键
        if (fd >= conn_tcps_.size())
        {
            conn_tcps_.resize(fd + 1, nullptr); // 将多余的元素初始化为 nullptr
        }
        conn_tcps_[fd] = conn;
    }

    void SignalingWork::_read_query(const int fd)
    {
        RTC_LOG(LS_INFO) << "SignalingWork::_read_query() fd:" << fd
                         << ", work_id : " << work_id_;
        if (fd < 0 || (size_t)fd > conn_tcps_.size())
        {
            RTC_LOG(LS_ERROR) << "invalid fd:" << fd
                               << ",conn_tcps_.size()"<< conn_tcps_.size()
                              << ", work_id : " << work_id_ << " is invalid";
            // Output all keys
            RTC_LOG(LS_INFO) << "All keys in conn_tcps_:";
            for (const auto &pair : conn_tcps_)
            {
                RTC_LOG(LS_INFO) << pair;
            }
            return;
        }

        TcpConnection *conn = conn_tcps_[fd];
#ifndef USE_SDS

        if (!conn)
        {
            RTC_LOG(LS_ERROR) << "invalid conn fd:" << fd
                              << ", work_id : " << work_id_ << " is invalid";
            return;
        }
        int ret = conn->read(fd, [=](Json::Value root, uint32_t log_id)
                             { _process_request_msg(conn, root, log_id); });
        if (-1 == ret)
        {
            _close_connection(conn);
            return;
        }
#else
        int nread = 0;
        const int read_len = conn->bytes_expected_;

        int qb_len = sdslen(conn->queryBuf_);
        conn->queryBuf_ = sdsMakeRoomFor(conn->queryBuf_, read_len);
        nread = sock_read_data(fd, conn->queryBuf_ + qb_len, read_len);
        RTC_LOG(LS_INFO) << "SignalingWork::_read_query() fd:" << fd
                         << ", work_id : " << work_id_ << " nread:" << nread;

        if (-1 == nread)
        {
            // 读取失败
            RTC_LOG(LS_ERROR) << "read query failed fd:" << fd
                              << ", work_id : " << work_id_ << " nread:" << nread;
            // _close_connection(fd);
            return;
        }
        else if (nread > 0)
        {
            sdsIncrLen(conn->queryBuf_, nread);
        }
        int ret = _process_queue_buffer(conn);
           // 接收到数据的时间
        conn->set_last_interaction_time(el_->now_time_usec());

#endif // !        #ifdef USE_SDS

     
    }
#ifdef USE_SDS
    int SignalingWork::_process_queue_buffer(const TcpConnection *conn)
    {
        while (sdslen(conn->queryBuf_) >= conn->bytes_processed_)
        {
            rhead_t *head = (lheader_t *)(conn->queryBuf_);
            if (TcpConnection::STATE_HEAD == conn->current_state_)
            {
                if (L_HEADER_MAGIC_NUMBER != head->magic_num)
                {
                    RTC_LOG(LS_WARNING) << "invalid magic number:" << head->magic_num;
                    return -1;
                }
                conn->current_state_ = TcpConnection::STATE_BODY;
                conn->bytes_expected_ = L_HEADER_SIZE;
                conn->bytes_processed_ = head->body_len;
            }
            else
            {
                rtc::Slice header(conn->queryBuf_, L_HEADER_SIZE);
                rtc::Slice body(conn->queryBuf_ + L_HEADER_SIZE, head->body_len);
                int ret = _process_request(conn, header, body);
                if (-1 == ret)
                {
                    RTC_LOG(LS_ERROR) << "process request failed";
                    return -1;
                }
                // 假定是一个短连接处理 忽略其他数据
                conn->bytes_expected_ = 65535;
            }
        }

        return 0;
    }
    int SignalingWork::_process_request(const TcpConnection *conn,
                                        const rtc::Slice *header,
                                        const rtc::Slice *body)
    {
        RTC_LOG(LS_INFO) << "SignalingWork::_process_request() fd:" << conn->fd_
                         << ", work_id : " << work_id_ << " header:" << header->data()
                         << ", body:" << body->data();
        return 0;
    }
#endif

    void SignalingWork::_close_connection(TcpConnection *conn)
    {
        RTC_LOG(LS_INFO) << "SignalingWork::_close_connection() fd:" << conn->get_fd()
                         << ", work_id : " << work_id_;
        if (conn)
        {
            conn->close_conn();
        }
        _remove_connection(conn);
    }

    void SignalingWork::_remove_connection(TcpConnection *conn)
    {
        if (conn)
        {
            el_->delete_timer_event(conn->timer_watcher_);
            el_->delete_io_event(conn->io_watcher_);
            // conn_tcps_.erase(conn->get_fd());
            int index = conn->get_fd();
            if (index < conn_tcps_.size())
            {
                delete conn_tcps_[index]; // 如果需要，释放资源
                conn_tcps_.erase(conn_tcps_.begin() + index);
            }
            if (conn)
            {
                conn = nullptr;
            }
            
            
        }
    }

    void SignalingWork::_process_timeout(TcpConnection *conn)
    {

        uint32_t timer_sub = el_->now_time_usec() - conn->last_interaction_time();

        if (timer_sub > (uint32_t)options_.connect_timeout * 1000)
        {
            RTC_LOG(LS_INFO) << "SignalingWork::_process_timeout() fd:" << conn->get_fd()
                             << ", work_id : " << work_id_
                             << " timer cout sub = " << timer_sub
                             << "  connect_timeout setting:" << (uint32_t)options_.connect_timeout * 1000;
            _close_connection(conn);
        }
    }

    int SignalingWork::_process_request_msg(TcpConnection *conn, Json::Value root, uint32_t log_id)
    {
           // 接收到数据的时间
        conn->set_last_interaction_time(el_->now_time_usec());
        RTC_LOG(LS_INFO)<<"SignalingWork::_process request_msg log_id:"<<log_id;
        // 解析body {"cmdno":1,"uid":1234321,"stream_name":"lymRTest","audio":1,"video":1}
        int cmdNo = 0;
        try
        {
            cmdNo = root["cmdno"].asInt();
        }
        catch (const Json::Exception &e)
        {
            RTC_LOG(LS_ERROR) << "no cmdno field in body err: " << e.what() << ",log_id:" << log_id;
            return -1;
        }
        switch (cmdNo)
        {
        case CMDNUM_PUSH:
            _process_request_push_msg(conn, cmdNo, root, log_id);

            break;
        case CMDNUM_PULL:

            break;
        case CMDNUM_ANSWER:

            break;
        case CMDNUM_STOP_PUSH:

            break;
        case CMDNUM_STOP_PULL:

            break;
        default:
            break;
        }

        return 0;
    }

    int SignalingWork::_process_request_push_msg(TcpConnection *conn, int cmdno, Json::Value root, uint32_t log_id)
    {  
        // 暂时停止读事件
        el_->stop_io_event(conn->io_watcher_,conn->get_fd(),EventLoop::READ);
        uint64_t uid = 0;
        std::string stream_name;
        int audio;
        int video;
        try
        {
            uid = root["uid"].asUInt64();
            stream_name = root["stream_name"].asString();
            audio = root["audio"].asInt();
            video = root["video"].asInt();
        }
        catch (const Json::Exception &e)
        {
            RTC_LOG(LS_ERROR) << "parse json body  err: " << e.what() << ",log_id:" << log_id;
            return -1;
        }

        RTC_LOG(LS_INFO) << "SignalingWork::_process_request_push_msg() fd:" << conn->get_fd()
                         << ", work_id : " << work_id_ << " cmdno = " << cmdno << " [uid:" << uid
                         << ", stream_name:" << stream_name << ", audio:" << audio
                         << ", video:" << video << "]";
        std::shared_ptr<lrtc::LRtcMsg> msg = std::make_shared<lrtc::LRtcMsg>();
        msg->cmdno = cmdno;
        msg->uid = uid;
        msg->stream_name = stream_name;
        msg->audio = audio;
        msg->video = video;
        msg->log_id = log_id;
        msg->signalingWorker = this;
        msg->signalingConn = conn;
        msg->fd = conn->get_fd();

        return g_rtc_server->send_rtc_msg(msg);
    }

    void SignalingWork::_process_rtc_msg()
    {
        RTC_LOG(LS_INFO)<< "SignalingWork::_process_rtc_msg";
        std::shared_ptr<LRtcMsg> msg = pop_msg();
        if (!msg)
        {
            return;
        }
        RTC_LOG(LS_INFO) << "SignalingWork::_process_rtc_msg cmdno:" << msg->cmdno;
        
        switch (msg->cmdno)
        {
        case CMDNUM_PUSH:
            _response_server_offer(msg);
            break;

        default:
            RTC_LOG(LS_WARNING) << "SignalingWork::_process_rtc_msg unknown cmdno"
                                << msg->cmdno << ", log_id = " << msg->log_id;
            break;
        }
    }

    void SignalingWork::_response_server_offer(std::shared_ptr<LRtcMsg> rtc_msg)
    {
        RTC_LOG(LS_INFO) << "SignalingWork::_response _server_offer offer:" << rtc_msg->sdp;
        TcpConnection *conn = (TcpConnection *)(rtc_msg->signalingConn);
        if (!conn)
        {
            RTC_LOG(LS_ERROR) << "SignalingWork::_response _server_offer conn is null";
            return;
        }
        int fd = rtc_msg->fd;
        RTC_LOG(LS_INFO) << "SignalingWork::_response _server_offer msg_fd:" << fd 
                          << ", peer fd:"<<conn->get_fd() 
                          << ", conn:"<<conn 
                          << ", conn_tcps_:"<<conn_tcps_[fd];

        if (fd <= 0 || sizeof(fd) >= conn_tcps_.size())
        {
            RTC_LOG(LS_ERROR) << "SignalingWork::_response server_offer fd == -1";
            return;
        }
        if (conn_tcps_[fd] != conn)
        {
             RTC_LOG(LS_ERROR) << "SignalingWork::_response_ server_offer() peer and conn_tcps_[fd] not eque";

            return;
        }

       
#ifdef USE_SDS
        lheader_t *h = (lheader_t *)(c->querybuf);
        rtc::Slice header(c->querybuf, L_HEADER_SIZE);
        char *buf = (char *)zmalloc(L_HEADER_SIZE + MAX_RESP_BUFEER_SIZE);
        if (!buf)
        {
            RTC_LOG(LS_ERROR) << "SignalingWork::_response_ server_offer zmalloc failed log_id:= " << h->log_id;
            ;
            return;
        }
        memccpy(buf, header.data(), header.size());
         lheader_t *res_xh  = (lheader_t *)buf
#else
        lheader_t *res_xh  = conn->req_header();
        rtc::ByteBufferWriter writer(rtc::ByteBuffer::ORDER_HOST);
#endif
        Json::Value resp_root;
        resp_root["err_no"] = rtc_msg->err_no;
        if (rtc_msg->err_no != 0)
        {
            resp_root["err_msg"] = "process error";
            resp_root["offer"] = "";
        }
        else
        {
            resp_root["err_msg"] = "success";
            resp_root["offer"] = rtc_msg->sdp;
        }
        Json::StreamWriterBuilder write_builder;
        write_builder.settings_["indentation"] = "";
        std::string json_data = Json::writeString(write_builder, resp_root);
        RTC_LOG(INFO) << "send_rtc_msg json_data:" << json_data << ", res_xh:"<< res_xh;
        if (res_xh == nullptr)
        {
            RTC_LOG(INFO) << "res_xh is nullptr";
            return;
        }
        
        res_xh->body_len = json_data.size();
        RTC_LOG(INFO) << "send_rtc_msg lheader_t:" << res_xh->toString();
#ifdef USE_SDS
        snprintf(buf + L_HEADER_SIZE, MAX_RESP_BUFEER_SIZE, "%s", json_data.c_str());
        rtc::Slice reply(buf, L_HEADER_SIZE + res_xh->body_len);
        _add_reply_conn(conn,reply);
#else
        conn->writerHeaderDataToBuffer(*res_xh, writer);
        writer.WriteString(json_data);
        _add_reply_conn(conn,writer);
#endif

    }
#ifdef USE_SDS
    void SignalingWork::_add_reply_conn(TcpConnection *conn, const rtc::Slice &repaly)
    {
        conn->reply_list.push_back(repaly);
#else
    void SignalingWork::_add_reply_conn(TcpConnection *conn, const rtc::ByteBufferWriter &writer)
    {
        conn->reply_list.push_back(std::make_unique<rtc::ByteBufferWriter>(writer.Data(), writer.Length(),rtc::ByteBuffer::ORDER_HOST));
#endif
   
       RTC_LOG(LS_INFO)<<"add reply conn fd:"<<conn->get_fd()
                                    <<", worker_id:"<<work_id_;
        if (!conn->io_watcher_ )
        {
           RTC_LOG(LS_ERROR)<<"add reply conn fd:"<<conn->get_fd()
                                    <<", worker_id:"<<work_id_
                                    <<", io_watcher_:"<<conn->io_watcher_;
           return;
        }
        
      el_->start_io_event(conn->io_watcher_,conn->get_fd(),EventLoop::WRITE);

    }

    void SignalingWork::_write_repaly(int fd)
    {
        RTC_LOG(LS_INFO) << "SignalingWork::_write_repaly fd:" << fd;
        if (fd <= 0 || (size_t)(fd) >= conn_tcps_.size())
        {
            RTC_LOG(LS_ERROR) << "SignalingWork::_write_repaly fd:" << fd << " is invalid";
            return;
        }

        TcpConnection *conn = conn_tcps_[fd];
        while (!conn->reply_list.empty())
        {
            int nwritten = 0;
#ifdef USE_SDS
            rtc::Slice reply = conn->reply_list.front();
            nwritten = sock_write_data(conn->get_fd(), reply->data() + conn->cur_resp_pos, reply->size() - conn->cur_resp_pos);

#else
            rtc::ByteBufferWriter *replyWiter = conn->reply_list.front().get();
            const size_t data_len = replyWiter->Length();
            char *dataBuff = static_cast<char *>(malloc(data_len));
            if (dataBuff == nullptr)
            {
                // 内存分配失败，处理错误
                // 可以抛出异常或者返回错误码
                // 例如：
                throw std::runtime_error("Failed to allocate memory");
            }
            memset(dataBuff, 0, data_len);
            memcpy(dataBuff, replyWiter->Data(), data_len);
            nwritten = sock_write_data(conn->get_fd(), dataBuff + conn->cur_resp_pos, data_len - conn->cur_resp_pos);

#endif
            if (-1 == nwritten)
            {
                _close_connection(conn);
            }
            else if (0 == nwritten)
            {
                RTC_LOG(LS_WARNING) << "Write zero bytes,fd: " << conn->get_fd()
                                    << ", worker_id:" << work_id_;
#ifdef USE_SDS
            }
            else if ((nwritten + conn->cur_resp_pos) >= reply.size())
            {
                conn->reply_list.pop_front();

                zfree((void *)read.data());
                conn->cur_resp_pos = 0;
                RTC_LOG(LS_WARNING) << "Write all bytes,fd: " << conn->get_fd()
                                    << ", worker_id:" << work_id_;
            }
#else
            }
            else if ((nwritten + conn->cur_resp_pos) >= data_len)
            {
                conn->reply_list.pop_front();

                conn->cur_resp_pos = 0;
                RTC_LOG(INFO) << "send_rtc_msg: nwritten=" << nwritten << ", cur_resp_pos= " << conn->cur_resp_pos << ", data_len:" << data_len;
                //  replyWiter->Clear();
                //  replyWiter = nullptr;
            }
#endif
            else
            {
                conn->cur_resp_pos += nwritten;
            }
            RTC_LOG(INFO) << "send_rtc_msg end: nwritten=" << nwritten
            << ", cur_resp_pos= " << conn->cur_resp_pos 
            << ", data_len:" << data_len 
            << ", conn->reply_list.empty()="<<conn->reply_list.empty();
            conn->set_last_interaction_time(el_->now_time_usec());
            if (conn->reply_list.empty())
            {

                el_->stop_io_event(conn->io_watcher_, conn->get_fd(), EventLoop::WRITE);
                el_->stop_timer_event(conn->timer_watcher_);
            }
        }
    }

} // namespace lrtc
