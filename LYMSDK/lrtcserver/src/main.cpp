
#include <iostream>
#include <csignal>
#include <cstdlib>

#include "base/conf.h"
#include "base/log.h"
#include "server/signaling_server.h"
#include "server/rtc_server.h"

lrtc::GeneralConf *g_conf = nullptr;
lrtc::LrtcLog *g_log = nullptr;
lrtc::SignalingServer *g_sig_nal = nullptr;
std::unique_ptr<lrtc::RtcServer> g_rtc_server{nullptr};

int init_general_conf(const char *filename)
{
    if (!filename)
    {
        fprintf(stderr, "filename is nullptr\n");
        return -1;
    }
    g_conf = new lrtc::GeneralConf();
    int ret = lrtc::load_general_conf(filename, g_conf);
    if (ret != 0)
    {
        fprintf(stderr, "load config file failed filename:%s\n", filename);
        return -1;
    }
    return 0;
}
int init_log(const std::string &log_dir, const std::string &log_name, std::string &log_level)
{
    g_log = new lrtc::LrtcLog(log_dir, log_name, log_level);
    int ret = g_log->setUpLogging();
    if (ret != 0)
    {
        fprintf(stderr, "init log file failed \n");
        return -1;
    }

    return 0;
}

int init_singnaling_server(const char *filename)
{
    if (!filename)
    {
        RTC_LOG(LS_ERROR) << "ilename is nullptr\n";
        return -1;
    }
    g_sig_nal = new lrtc::SignalingServer();
    int ret = g_sig_nal->init(filename);
    if (ret != 0)
    {
        return -1;
    }
    return 0;
}
int init_rtc_server(const char *filename)
{
    if (!filename)
    {
        RTC_LOG(LS_ERROR) << "ilename is nullptr\n";
        return -1;
    }
    g_rtc_server = std::make_unique<lrtc::RtcServer>();
    int ret = g_rtc_server->init(filename);
    if (ret != 0)
    {
        return -1;
    }
    return 0;
}
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
        
    }
}
int main(int /*argc*/, const char ** /*argv*/)
{
    // LYMSDK/lrtcserver/conf/general.yaml
    // LYMSDK/lrtcserver/src/main.cpp
    int ret = init_general_conf("./conf/general.yaml");
    if (ret != 0)
    {
        return -1;
    }
    ret = init_log(g_conf->log_dir, g_conf->log_name, g_conf->log_level);
    if (ret != 0)
    {
        return -1;
    }
    g_log->set_log_to_stderror(g_conf->log_to_stderr);

    g_log->start();

    RTC_LOG(LS_DEBUG) << "hello world log_to_stderr=" << g_conf->log_to_stderr
                      << " log_level=" << g_conf->log_level
                      << " log_dir=" << g_conf->log_dir
                      << " log_name=" << g_conf->log_name;

    ret = init_singnaling_server("./conf/singnaling_server.yaml");
    if (ret != 0)
    {
        return -1;
    }

    ret = init_rtc_server("./conf/rtc_server.yaml");
    if (ret != 0)
    {
        return -1;
    }
    // 捕获系统中断事件
    // 设置信号处理程序
    signal(SIGINT, process_signal);
    signal(SIGTERM, process_signal);

    g_sig_nal->start();
    g_rtc_server->start();
    g_sig_nal->joined();
    g_log->join();
    g_rtc_server->joined();
    return 0;
}