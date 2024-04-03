#include "base/log.h"

#include <iostream>
#include <sys/stat.h>

namespace lrtc {

    LrtcLog::LrtcLog(const std::string &log_dir, 
    const std::string &log_name, 
    const std::string &log_level):
    log_dir_(log_dir),
    log_name_(log_name),
    log_level_(log_level),
    log_file_(log_dir + "/" + log_name + ".log"),
    log_file_wf_(log_dir + "/" + log_name + ".log.wf")
    {

    }

    LrtcLog::~LrtcLog()
    {  
      stop();
      out_file_.close();
      out_file_wf_.close();
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
        int ret = mkdir(log_dir_.c_str(),0755);
        if (ret != 0 && errno != EEXIST)
        {
          fprintf(stderr,"create log dir[%s] failed\n",log_dir_.c_str());
         return -1;
        }
        //打开文件
        out_file_.open(log_file_,std::ios::app);
        if(!out_file_.is_open()){
          fprintf(stderr,"create log file [%s] failed\n",log_file_.c_str());
           return -1;
        }
        //打开文件
        out_file_wf_.open(log_file_,std::ios::app);
        if(!out_file_wf_.is_open()){
          fprintf(stderr,"create log file [%s] failed\n",log_file_wf_.c_str());
           return -1;
        }
        
        return 0;
    }
    void LrtcLog::set_log_to_stderror(bool on){
        rtc::LogMessage::SetLogToStderr(on);
    }

    bool LrtcLog::start()
    {
      if (running_)
      {
        fprintf(stderr,"log thread already running\n");
        return false;
      }
      running_ = true;
      thread_ = new std::thread([=](){
        struct  stat stat_data;
        std::stringstream ss;
        
        while (running_)
        {
          // 检查文件的状态
         if(stat(log_file_.c_str(),&stat_data) < 0) {
              out_file_.close();
              out_file_.open(log_file_,std::ios::app);

         }
          // 检查文件的状态
         if(stat(log_file_wf_.c_str(),&stat_data) < 0) {
              out_file_wf_.close();
              out_file_wf_.open(log_file_wf_,std::ios::app);

         }

         bool write_log = false;
         {
          std::unique_lock<std::mutex> lock(queue_mutex_);
           if (!log_queue_.empty())
           {
             write_log = true;
             while (!log_queue_.empty())
             {
               ss << log_queue_.front();
               log_queue_.pop();
             }
           }
         }
         if (write_log)
         {
           out_file_ << ss.str();
           out_file_.flush();
         }
         ss.str("");

         bool write_log_wf = false;
         {
           std::unique_lock<std::mutex> lock(queue_mutex_wf_);
           if (!log_queue_wf_.empty())
           {
             write_log_wf = true;
             while (!log_queue_wf_.empty())
             {
               ss << log_queue_wf_.front();
               log_queue_wf_.pop();
             }
           }
         }

         if (write_log_wf)
         {
           out_file_wf_ << ss.str();
           out_file_wf_.flush();
         }
         ss.str("");
         std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }
      });
      
      return true;
      
    }
    void LrtcLog::stop()
    {
      running_ = false;
      if(thread_){
        if (thread_->joinable())
        {
          thread_->join();
        }
        delete thread_;
        thread_ = nullptr;
        
      }
    }
    void LrtcLog::join()
    {
      if (thread_ && thread_->joinable())
        {
          thread_->join();
        }
    }
  void LrtcLog::OnLogMessage(const std::string& message,
                            rtc::LoggingSeverity severity)
    {
     if (severity >= rtc::LS_WARNING)
     {
        std::unique_lock<std::mutex> lock(queue_mutex_wf_);
        log_queue_wf_.push(message);
     }else{
       std::unique_lock<std::mutex> lock(queue_mutex_);
        log_queue_.push(message);
     }
     
        
    }
  void LrtcLog::OnLogMessage(const std::string& /*message*/)
  {


  }



}  // namespace lrtc
