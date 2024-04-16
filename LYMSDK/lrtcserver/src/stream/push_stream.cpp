#include "stream/push_stream.h"
#include "push_stream.h"

namespace lrtc
{
    PushStream::PushStream(EventLoop *el, uint64_t uid, const std::string &stream_name, bool audio, bool video, uint32_t log_id)
    : RtcStream(el, uid, stream_name, audio, video, log_id)
    {

    }
    PushStream::~PushStream(){
        // LOG_INFO("PushStream::~PushStream()");
    }
    std::string PushStream::create_offer_sdp()
    {
       RTC_LOG(LS_INFO) << "PushStream::create_offer_sdp()"; 
        return pc_->create_offer_sdp();
    }





} //namespace lrtc
