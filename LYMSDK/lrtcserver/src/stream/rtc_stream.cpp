#include "stream/rtc_stream.h"

namespace lrtc
{

    RtcStream::RtcStream(EventLoop *el, uint64_t uid, const std::string &stream_name, bool audio, bool video, uint32_t log_id)
        : el_(el),
          uid_(uid),
          stream_name_(stream_name),
          audio_(audio),
          video_(video),
          log_id_(log_id)
    {
        RTC_LOG(LS_INFO) << "RtcStream::RtcStream()";
    }
    RtcStream::~RtcStream()
    {

        RTC_LOG(LS_INFO) << "RtcStream::~RtcStream()";
    }

} // namespace lrtc
