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
        rtc::LogMessage::AddLogToStream(this, rtc::LS_VERBOSE);
        rtc::LogMessage::SetLogToStderr(true);
        return 0;
    }
  void LrtcLog::OnLogMessage(const std::string& message,
                            rtc::LoggingSeverity severity)
    {
        
    }
  void LrtcLog::OnLogMessage(const std::string& message)
  {


  }



}  // namespace lrtc
