#include "stream/push_stream.h"
#include "push_stream.h"

namespace lrtc
{
    PushStream::PushStream(EventLoop *el, PortAllocator *allocator, const std::shared_ptr<LRtcMsg> &msg)
        : RtcStream(el, allocator, msg)
    {
    }
    PushStream::~PushStream()
    {
        // LOG_INFO("PushStream::~PushStream()");
    }
    std::string PushStream::create_offer_sdp()
    {
        RTC_LOG(LS_INFO) << "PushStream::create_offer_sdp()";
        RTCOfferAnswerOptions options;
        options.recv_audio = audio_;
        options.recv_video = video_;
        options.send_audio = false;
        options.send_video = false;
        options.use_rtcp_mux = true;
        options.use_rtp_mux = true;
        return pc_->create_offer_sdp(options);
    }

} // namespace lrtc
