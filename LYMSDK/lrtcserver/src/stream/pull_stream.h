#ifndef __LYMSDK_LRTCSERVER_SRC_STREAM_PULL_STEAM_H_
#define __LYMSDK_LRTCSERVER_SRC_STREAM_PULL_STEAM_H_

#include <stdint.h>
#include <string>

#include "stream/rtc_stream.h"
#include "pc/stream_params.h"

namespace lrtc
{
    // class PullStream : public RtcStream
    // {
    // public:
    //     PullStream(EventLoop *el, PortAllocator *allocator, const std::shared_ptr<LRtcMsg> &msg);
    //     ~PullStream();

    // public:
    //     std::string create_answer() override;
    //     RtcStreamType stream_type() override { return RtcStreamType::k_pull; }

    //     void add_audio_source(const std::vector<StreamParams> &source);
    //     void add_video_source(const std::vector<StreamParams> &source);
    // };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_STREAM_PULL_STEAM_H_
