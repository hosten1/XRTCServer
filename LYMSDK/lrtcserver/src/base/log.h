#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_LOG_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_LOG_H_

#include "logging.h"

namespace lrtc {
class LrtcLog :public rtcbase::LogSink
{
private:
    /* data */
public:
    LrtcLog(const std::string& log_dir,
            const std::string& log_name,
            const std::string& log_level);
    ~LrtcLog() override;

    int setUpLogging();
    
    void on_log_message(const std::string& message, rtcbase::LoggingSeverity severity) override;

private:

    std::string log_dir_;
    std::string log_name_;
    std::string log_level_;


};

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_BASE_LOG_H_
