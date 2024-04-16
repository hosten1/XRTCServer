#ifndef __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_
#define __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <memory>

#include <rtc_base/rtc_certificate.h>

#include "base/lrtc_server_def.h"

#include "base/event_loop.h"
#include <rtc_base/logging.h>
#include "stream/push_stream.h"


namespace lrtc {
    class EventLoop;
    class PushStream;
    class PortAllocator;
    class PullStream;

    class RTPStreamManager
    {
  
    public:
        RTPStreamManager(EventLoop *event_loop);
        ~RTPStreamManager();

        int create_push_stream(uint64_t uid,const std::string& stream_name, 
                            bool audio,bool video,uint32_t log_id,std::string& sdp);
        PushStream* find_push_stream(const std::string& stream_name);
     private:
        EventLoop *el_;
        std::unordered_map<std::string,PushStream*> push_stream_map_;
    };
    
  

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_
