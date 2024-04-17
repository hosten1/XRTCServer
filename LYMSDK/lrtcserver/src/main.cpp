
#include <iostream>
#include <csignal>
#include <cstdlib>

// #include "base/conf.h"
#include "base/log.h"
#include "server/signaling_server.h"
#include "server/rtc_server.h"
#include <p2p/base/basic_packet_socket_factory.h>
#include "server/settings.h"

// #include "server/singnaling_server_rtcthreat.h"

// lrtc::GeneralConf *g_conf = nullptr;
std::unique_ptr<lrtc::LrtcLog> g_log;
std::unique_ptr<lrtc::SignalingServer> g_sig_nal;
std::unique_ptr<lrtc::RtcServer> g_rtc_server;
// std::unique_ptr<lrtc::SignalingServerRtcThread> g_sig_nal_rtc_{nullptr};

// int init_general_conf(const char *filename)
// {
//     if (!filename)
//     {
//         fprintf(stderr, "filename is nullptr\n");
//         return -1;
//     }
//     g_conf = new lrtc::GeneralConf();
//     int ret = lrtc::load_general_conf(filename, g_conf);
//     if (ret != 0)
//     {
//         fprintf(stderr, "load config file failed filename:%s\n", filename);
//         return -1;
//     }
//     return 0;
// }
int init_log(const lrtc::LogConf &logConf)
{
    g_log = std::make_unique<lrtc::LrtcLog>(logConf.log_dir, logConf.log_name, logConf.log_level);
    int ret = g_log->init();
    if (ret != 0)
    {
        fprintf(stderr, "init log file failed \n");
        return -1;
    }
    g_log->set_log_to_stderror(logConf.log_to_stderr);
    g_log->start();

    return 0;
}

int init_singnaling_server(const lrtc::SignalingServerOptions &options)
{
    g_sig_nal = std::make_unique<lrtc::SignalingServer>();
    int ret = g_sig_nal->init(options);
    if (ret != 0)
    {
        return -1;
    }
    return 0;
}
int init_rtc_server(const lrtc::RtcServerOptions &options)
{
    g_rtc_server = std::make_unique<lrtc::RtcServer>();
    int ret = g_rtc_server->init(options);
    if (ret != 0)
    {
        return -1;
    }
    return 0;
}

// int init_singnaling_server_rtc(const char *filename)
// {
//     if (!filename)
//     {
//         RTC_LOG(LS_ERROR) << "ilename is nullptr\n";
//         return -1;
//     }
//     g_sig_nal_rtc_ = std::make_unique<lrtc::SignalingServerRtcThread>();
//     int ret = g_sig_nal_rtc_->init(filename);
//     if (ret != 0)
//     {
//         return -1;
//     }
//     return 0;
// }
static void process_signal(int sig)
{
    RTC_LOG(LS_INFO) << "process_signal sig=" << sig;
    if (SIGINT == sig || SIGTERM == sig)
    {
        if (g_sig_nal)
        {
            g_sig_nal->stop();
        }
        if (g_log)
        {
            g_log->stop();
        }

        if (g_rtc_server)
        {
            g_rtc_server->stop();
        }
        // if (g_sig_nal_rtc_)
        // {
        //     g_sig_nal_rtc_->stop();
        // }
    }
}
int main(int /*argc*/, const char ** /*argv*/)
{
    // LYMSDK/lrtcserver/conf/general.yaml
    // LYMSDK/lrtcserver/src/main.cpp
    // int ret = init_general_conf("./conf/general.yaml");
    // if (ret != 0)
    // {
    //     return -1;
    // }
    if (!lrtc::Singleton<lrtc::Settings>::Instance()->Init("./conf/general.yaml"))
    {
        fprintf(stderr, "init settings failed \n");
        return -1;
    }
    int ret = init_log(lrtc::Singleton<lrtc::Settings>::Instance()->GetLogConf());
    if (ret != 0)
    {
        return -1;
    }

    ret = init_singnaling_server(lrtc::Singleton<lrtc::Settings>::Instance()->GetSignalingServerOptions());
    if (ret != 0)
    {
        printf("init_singnaling_server failed\n");

        return -1;
    }

    ret = init_rtc_server(lrtc::Singleton<lrtc::Settings>::Instance()->GetRtcServerOptions());
    if (ret != 0)
    {
        printf("init_rtc_server failed\n");
        return -1;
    }

    // ret = init_singnaling_server_rtc("./conf/singnaling_server.yaml");
    // if (ret != 0)
    // {
    //     printf("init_singnaling_server_rtc failed\n");
    //     return -1;
    // }
    // 捕获系统中断事件
    // 设置信号处理程序
    signal(SIGINT, process_signal);
    signal(SIGTERM, process_signal);

    g_sig_nal->start();
    g_rtc_server->start();
    // g_sig_nal_rtc_->start();
    g_rtc_server->joined();
    g_log->join();
    return 0;
}