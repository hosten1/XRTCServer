/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file settings.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 10:53:18, 四月 17, 2024
 */

#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SETTINGS_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SETTINGS_H_

#include <string>

#include "base/utils.h"

namespace lrtc
{

    typedef struct LogConf
    {
        std::string log_dir = "./log";
        std::string log_name = "undefined";
        std::string log_level = "info";
        bool log_to_stderr = false;
    } LogConf;

    struct IceConf
    {
        int ice_min_port = 0;
        int ice_max_port = 0;
    };

    struct RtcServerOptions
    {
        std::string candidate_ip;
        int worker_num = 2;
    };

    struct SignalingServerOptions
    {
        std::string host;
        int port = 9000;
        int worker_num = 2;
        int connect_timeout = 5000; // 单位毫秒
    };

    class Settings
    {
    public:
        Settings() = default;
        ~Settings() = default;

        bool Init(const char *confPath);

        LogConf GetLogConf()
        {
            return log_conf_;
        }

        IceConf GetIceConf()
        {
            return ice_conf_;
        }

        int IceMinPort() const
        {
            return ice_conf_.ice_min_port;
        }

        int IceMaxPort() const
        {
            return ice_conf_.ice_max_port;
        }

        SignalingServerOptions GetSignalingServerOptions()
        {
            return signaling_server_options_;
        }

        const std::string &CandidateIp()
        {
            return rtc_server_options_.candidate_ip;
        }

        RtcServerOptions GetRtcServerOptions()
        {
            return rtc_server_options_;
        }

    private:
        std::string conf_path_;

        LogConf log_conf_;
        IceConf ice_conf_;
        RtcServerOptions rtc_server_options_;
        SignalingServerOptions signaling_server_options_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_SETTINGS_H_
