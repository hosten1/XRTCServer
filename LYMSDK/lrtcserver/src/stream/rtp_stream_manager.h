#ifndef __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_
#define __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_

#include <string>
#include <stdint.h>

#include "base/event_loop.h"
#include <rtc_base/logging.h>

namespace lrtc {
    class RTPStreamManager
    {
  
    public:
        RTPStreamManager(EventLoop *event_loop);
        ~RTPStreamManager();

        int create_push_stream(uint64_t uid,const std::string& stream_name, 
                            bool audio,bool video,uint32_t log_id,std::string& sdp);
     private:
        EventLoop *el_;
    };
    
  

}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_
