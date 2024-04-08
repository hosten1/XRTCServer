#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_RTC_STREAM_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_RTC_STREAM_H_

#include <string>
#include <stdint.h>

#include "base/event_loop.h"
#include <rtc_base/logging.h>

namespace lrtc
{
  class RtcStream
  {

  public:
    RtcStream(EventLoop *el, uint64_t uid,
              const std::string &stream_name, 
              bool audio, bool video, uint32_t log_id);
    virtual ~RtcStream();

  private:
    EventLoop *el_;
    uint64_t uid_;
    std::string stream_name_;
    bool audio_;
    bool video_;
    uint32_t log_id_;
  };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_RTC_STREAM_H_
