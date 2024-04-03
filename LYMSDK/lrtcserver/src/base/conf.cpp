
#include <stdio.h>
#include <iostream>
#include "base/conf.h"
#include <yaml-cpp/yaml.h>

namespace lrtc {

int load_general_conf(const char *filename, GeneralConf *conf)
{
    if (!filename || !conf)
    {
        fprintf(stderr,"filename or conf is nullptr \n");
        return -1;
    }
    conf->log_dir = "./log";
    conf->log_name = "undefind";
    conf->log_level = "info";
    conf->log_to_stderr = false;
    try
    {
        YAML::Node config = YAML::LoadFile(filename);
        conf->log_dir = config["log"]["log_dir"].as<std::string>();
        conf->log_name = config["log"]["log_name"].as<std::string>();
        conf->log_level = config["log"]["log_level"].as<std::string>();
        conf->log_to_stderr = config["log"]["log_to_stderr"].as<bool>();
    }
    catch(const YAML::Exception& e)
    {
        std::cerr << "catch a YAML::Exeption,err: "<<e.what() 
                    << " line : "<<e.mark.line+1 <<" col:"<<e.mark.column+1
                     <<'\n';
    }
    
  
    return 0;
}

}  // namespace conf
