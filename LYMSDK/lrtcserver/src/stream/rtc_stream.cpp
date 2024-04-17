/*
 * @Author: L yongmeng
 * @Date: 2024-04-08 18:12:05
 * @LastEditTime: 2024-04-16 17:35:47
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */
#include "stream/rtc_stream.h"
#include "rtc_stream.h"

namespace lrtc
{
    const size_t k_ice_timeout = 30000; // 30s
    void ice_timeout_cb(EventLoop * /*el*/, TimerWatcher * /*w*/, void *data)
    {
        RtcStream *stream = (RtcStream *)data;
        // if (stream->state_ != PeerConnectionState::k_connected)
        // {
        //     if (stream->listener_)
        //     {
        //         stream->listener_->on_stream_exception(stream);
        //     }
        // }
    }

    RtcStream::RtcStream(EventLoop *el, PortAllocator *allocator, const std::shared_ptr<LRtcMsg> &msg)
        : el_(el),
          uid_(msg->uid),
          stream_name_(msg->stream_name),
          audio_(msg->audio),
          video_(msg->video),
          dtls_on_(msg->dtls_on),
          log_id_(msg->log_id),
          pc_(new PeerConnection(el, allocator, dtls_on_))
    {
        RTC_LOG(LS_INFO) << "RtcStream::RtcStream()";
    }
    RtcStream::~RtcStream()
    {

        RTC_LOG(LS_INFO) << "RtcStream::~RtcStream()";
    }

    int RtcStream::start(rtc::RTCCertificate *certificate)
    {
        ice_timeout_watcher_ = el_->create_timer_event(ice_timeout_cb, this, false);
        el_->start_timer_event(ice_timeout_watcher_, k_ice_timeout * 1000);

        return pc_->init(certificate);
    }

} // namespace lrtc
