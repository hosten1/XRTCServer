
#include <iostream>
#include "base/conf.h"
#include "base/log.h"


lrtc::GeneralConf* g_conf = nullptr;
lrtc::LrtcLog* g_log  = nullptr;

int init_general_conf(const char* filename) {
    if (!filename)
    {
        fprintf(stderr,"filename is nullptr\n");
        return -1;
    }
    g_conf = new lrtc::GeneralConf();
    int ret = lrtc::load_general_conf(filename,g_conf);
    if (ret != 0)
    {
        fprintf(stderr,"load config file failed \n",filename);
       return -1;
    }  
    
}
int  init_log(const std::string& log_dir, const
 std::string& log_name, std::string& log_level){
    g_log = new lrtc::LrtcLog(log_dir,log_name,log_level);
    int ret = g_log->setUpLogging();
    if (ret != 0)
    {
        fprintf(stderr,"init log file failed \n");
       return -1;
    }


}

int  main(int argc, const char** argv) {
    // LYMSDK/lrtcserver/conf/general.yaml
    // LYMSDK/lrtcserver/src/main.cpp
   int ret = init_general_conf("./conf/general.yaml");
   if (ret != 0)
   {
    return -1;
   }
   ret = init_log(g_conf->log_dir,g_conf->log_name,g_conf->log_level);
   if (ret != 0)
   {
    return -1;
   }
   g_log->set_log_to_stderror(g_conf->log_to_stderr);

   g_log->start();


   RTC_LOG(LS_DEBUG) << "hello world log_to_stderr=" << g_conf->log_to_stderr 
                            <<" log_level=" << g_conf->log_level
                            <<" log_dir=" << g_conf->log_dir
                             <<" log_name=" << g_conf->log_name;
   g_log->join();  
    return 0;
}