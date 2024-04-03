#include "base/log.h"

#include <iostream>

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
   static rtc::LoggingSeverity get_log_severity(const std::string& level){
    if ("verbose" == level)
    {
      return rtc::LS_VERBOSE;
    }else if ("debug" == level)
    {
      return rtc::LS_DEBUG;
    }else if ("info" == level)
    {
      return rtc::LS_INFO;
    }else if ("warning" == level)
    {
     return rtc::LS_WARNING;
    }else if ("error" == level)
    {
     return rtc::LS_ERROR;
    }else if ("none" == level)
    {
     return rtc::LS_NONE;
    }
    return rtc::LS_NONE;
    
   }

    int LrtcLog::setUpLogging()
    {
        rtc::LogMessage::ConfigureLogging("thread tstamp");
        rtc::LogMessage::AddLogToStream(this, get_log_severity(log_level_));
        return 0;
    }
    void LrtcLog::set_log_to_stderror(bool on){
        rtc::LogMessage::SetLogToStderr(on);
    }
  void LrtcLog::OnLogMessage(const std::string& message,
                            rtc::LoggingSeverity severity)
    {
     std::cout << " <<<<<<<<<< msg:"<< message << std::endl;
        
    }
  void LrtcLog::OnLogMessage(const std::string& /*message*/)
  {


  }



}  // namespace lrtc
