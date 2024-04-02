#include "log.h"

namespace lrtc {

    LrtcLog::LrtcLog(const std::string &log_dir, 
    const std::string &log_name, 
    const std::string &log_level):
    log_dir_(log_dir),
    log_name_(log_name),
    log_level_(log_level)
    {

    }

    LrtcLog::~LrtcLog()
    {

    }

    int LrtcLog::setUpLogging()
    {
        rtcbase::LogMessage::add_log_to_stream(this, rtcbase::LS_DEBUG);
        return 0;
    }

    void LrtcLog::on_log_message(const std::string &message, rtcbase::LoggingSeverity severity)
    {
        
    }

}  // namespace lrtc
