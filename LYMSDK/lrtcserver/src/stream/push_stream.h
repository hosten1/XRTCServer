#ifndef __LYMSDK_LRTCSERVER_SRC_STREAM_PUSH_STREAM_H_
#define __LYMSDK_LRTCSERVER_SRC_STREAM_PUSH_STREAM_H_

#include <stdint.h>
#include <string>

#include "stream/rtc_stream.h"
#include "pc/stream_params.h"

namespace lrtc
{
  class PushStream : public RtcStream
  {
  private:
    /* data */
  public:
    PushStream(EventLoop *el, PortAllocator *allocator, const std::shared_ptr<LRtcMsg> &msg);
    ~PushStream();
    std::string create_offer_sdp() override;
  };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_STREAM_PUSH_STREAM_H_
