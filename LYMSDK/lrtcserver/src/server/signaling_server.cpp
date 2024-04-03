#include "server/signaling_server.h"
#include "signaling_server.h"
#include <yaml-cpp/yaml.h>
#include <rtc_base/logging.h>

namespace lrtc {
    SignalingServer::SignalingServer()
    {
    }

    SignalingServer::~SignalingServer()
    {
    }
    int SignalingServer::init(const char *conf_file)
    {
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
         return 0;

    }
}  // namespace lrtc
