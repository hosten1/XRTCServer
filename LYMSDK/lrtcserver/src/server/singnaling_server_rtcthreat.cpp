#include "server/singnaling_server_rtcthreat.h"
#include <rtc_base/logging.h>
#include <unistd.h>
#include <exception> // 引入异常处理库
#include <yaml-cpp/yaml.h>
#include "singnaling_server_rtcthreat.h"

namespace lrtc
{

    SignalingServerRtcThread::SignalingServerRtcThread()
    {
    }
    SignalingServerRtcThread::~SignalingServerRtcThread()
    {
        stop();
    }
    int SignalingServerRtcThread::init(const char *conf_file)
    {
        RTC_LOG(LS_INFO) << "SignalingServerRtcThread server init";
        if (!conf_file)
        {
            RTC_LOG(LS_WARNING) << "SignalingServerRtcThread server conf_file is null";
            return -1;
        }
        try
        {
            YAML::Node config = YAML::LoadFile(conf_file);
            options_.host = config["host"].as<std::string>();
            options_.port = config["port"].as<int>();
            options_.worker_num = config["worker_num"].as<int>();
            options_.connect_timeout = config["connection_timeout"].as<int>();
            RTC_LOG(LS_INFO) << "SignalingServerRtcThread server conf = " << config
                             << " options_.host = " << options_.host
                             << " options_.port = " << options_.port;
        }
        catch (const YAML::Exception &e)
        {
            RTC_LOG(LS_ERROR) << "SignalingServerRtcThread catch a YAML::Exeption,err: " << e.what()
                              << " line : " << e.mark.line + 1 << " col:" << e.mark.column + 1
                              << '\n';

            return -1;
        }
        network_thread_ = rtc::Thread::CreateWithSocketServer();
        network_thread_->Start();
        network_thread_->SetName("network_thread", network_thread_.get());
        default_socket_factory_.reset(
            new rtc::BasicPacketSocketFactory(network_thread_.get()));
        if (!default_socket_factory_)
        {
            return false;
        }
        int opts = 0;
        rtc::ProxyInfo info;
        info.type = rtc::ProxyType::PROXY_NONE;
        std::string user_agent_s = "";
        async_socket_.reset(default_socket_factory_->CreateServerTcpSocket(
            rtc::SocketAddress(options_.host, options_.port), 0, 0, opts));
       // async_socket_.reset(default_socket_factory_->CreateServerTcpSocket(
        //     rtc::SocketAddress("127.0.0.1", 0),
        //     rtc::SocketAddress(options_.host, options_.port), info, user_agent_s,opts));
        // if (async_socket_)
        if (!async_socket_)
        {
            RTC_LOG(LS_ERROR) << "SignalingServerRtcThread server create socket error";

            return false;
        }
        RTC_LOG(LS_INFO) << "luoyoongmegn : Connecting from "
                                << async_socket_->GetLocalAddress().ToSensitiveString()
                                << " to "
                                << rtc::SocketAddress(options_.host, options_.port).ToSensitiveString();
        async_socket_->SignalNewConnection.connect(this, &SignalingServerRtcThread::OnNewConnection);
        async_socket_->SignalAddressReady.connect(this, &SignalingServerRtcThread::OnAddressReady);
        
        // async_socket_->SignalConnect.connect(this, &SignalingServerRtcThread::OnSocketConnect);

        async_socket_->SignalReadPacket.connect(this, &SignalingServerRtcThread::OnReadPacket);
        // async_socket_->SignalReadyToSend.connect(this, &SignalingServerRtcThread::OnReadyToSend);
        // async_socket_->SignalClose.connect(this, &SignalingServerRtcThread::OnSocketClose);

        return true;
    }
    bool SignalingServerRtcThread::start()
    {
        RTC_LOG(LS_INFO) << "SignalingServerRtcThread server start";
        if (network_thread_->RunningForTest())
        {
            RTC_LOG(LS_WARNING) << "SignalingServerRtcThread server is running";
            return false; // 修改返回值为 false，以保持返回类型的一致性
        }
        network_thread_->Start();
        return true;
    }
    int SignalingServerRtcThread::stop()
    {
        RTC_LOG(LS_INFO) << "SignalingServerRtcThread server stop from other event";
       if(async_socket_) async_socket_->Close();
        if(network_thread_)network_thread_->Stop();
        return 0;
    }

    void SignalingServerRtcThread::OnNewConnection(rtc::AsyncPacketSocket *socket, rtc::AsyncPacketSocket *new_socket)
    {
        async_socket_client_ = new_socket;
        async_socket_client_->setNotProcessInput(true);
        async_socket_client_->SignalReadPacket.connect(this, &SignalingServerRtcThread::OnReadPacket);
        RTC_LOG(LS_INFO) << "SignalingServerRtcThread server OnNewConnection "<<new_socket->GetRemoteAddress().ToString();
    }
    void SignalingServerRtcThread::OnAddressReady(rtc::AsyncPacketSocket *socket, const rtc::SocketAddress &address)
    {
        RTC_LOG(LS_INFO) << "SignalingServerRtcThread server OnAddressReady "<<address.ToString();
    }

    // Called when a TCP connection is established or fails
    // void SignalingServerRtcThread::OnSocketConnect(rtc::AsyncPacketSocket *socket)
    // {
    //     RTC_LOG(LS_INFO) << "SignalingServerRtcThread server OnSocketConnect" << socket->GetRemoteAddress().ToString();
    // }
    // void SignalingServerRtcThread::OnSocketClose(rtc::AsyncPacketSocket *socket, int error)
    // {
    // }

    // // Called when a packet is received on this socket.
    void SignalingServerRtcThread::OnReadPacket(rtc::AsyncPacketSocket *socket,
                                                const char *data,
                                                size_t size,
                                                const rtc::SocketAddress &remote_addr,
                                                const int64_t &packet_time_us)
    {
        RTC_LOG(LS_INFO) << "SignalingServerRtcThread server OnReadPacket addr: " << socket->GetRemoteAddress().ToString()
                         << " ,size:" << size;
    }
    // void SignalingServerRtcThread::OnSentPacket(rtc::AsyncPacketSocket *socket,
    //                                             const rtc::SentPacket &sent_packet)
    // {
    // }
    // // Called when the socket is currently able to send.
    // void SignalingServerRtcThread::OnReadyToSend(rtc::AsyncPacketSocket *socket)
    // {
    // }

} // namespace lrtc
