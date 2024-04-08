#ifndef __LYMSDK_LRTCSERVER_SRC_STREAM_PUSH_STREAM_H_
#define __LYMSDK_LRTCSERVER_SRC_STREAM_PUSH_STREAM_H_

#include "stream/rtc_stream.h"


namespace lrtc {
  class PushStream:public RtcStream
  {
  private:
    /* data */
  public:
    PushStream(EventLoop *el, uint64_t uid,
              const std::string &stream_name, 
              bool audio, bool video, uint32_t log_id);
    ~PushStream();
  };
  
  

}  // namespace lrtc


#endif  // __LYMSDK_LRTCSERVER_SRC_STREAM_PUSH_STREAM_H_


