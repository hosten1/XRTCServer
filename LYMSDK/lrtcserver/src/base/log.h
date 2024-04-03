#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_LOG_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_LOG_H_


#include <fstream>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include "rtc_base/logging.h"

namespace lrtc {
class LrtcLog :public rtc::LogSink
{
private:
    /* data */
public:
    LrtcLog(const std::string& log_dir,
            const std::string& log_name,
            const std::string& log_level);
    ~LrtcLog() override;

    int setUpLogging();

    void set_log_to_stderror(bool on);

    bool start();
    void stop();
    void join();


    void OnLogMessage(const std::string& message,
                            rtc::LoggingSeverity severity) override;
    void OnLogMessage(const std::string& message) override;

private:

    std::string log_dir_;
    std::string log_name_;
    std::string log_level_;
    std::string log_file_;
    std::string log_file_wf_;

    std::ofstream out_file_;
    std::ofstream out_file_wf_;

    std::queue<std::string> log_queue_;
    std::mutex queue_mutex_;

    std::queue<std::string> log_queue_wf_;
    std::mutex queue_mutex_wf_;

    std::thread*  thread_ = nullptr ;
    std::atomic<bool> running_{false};
};

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_BASE_LOG_H_
