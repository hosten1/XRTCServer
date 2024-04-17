/*
 * @Author: L yongmeng
 * @Date: 2024-04-08 18:07:54
 * @LastEditTime: 2024-04-16 17:28:35
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */
#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_RTC_STREAM_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_RTC_STREAM_H_

#include <stdint.h>
#include <string>
#include <memory>

#include <rtc_base/rtc_certificate.h>
#include <rtc_base/third_party/sigslot/sigslot.h>

#include <rtc_base/logging.h>
#include "base/event_loop.h"
#include "pc/peer_connection.h"
#include "base/lrtc_server_def.h"

namespace lrtc
{
  class EventLoop;
  class PortAllocator;
  class RtcStream;

  enum class RtcStreamType
  {
    k_push,
    k_pull
  };

  class RtcStreamListener
  {
  public:
    // virtual void on_connection_state(RtcStream *stream, PeerConnectionState state) = 0;
    // virtual void on_rtp_packet_received(RtcStream *stream, const char *data, size_t len) = 0;
    // virtual void on_rtcp_packet_received(RtcStream *stream, const char *data, size_t len) = 0;
    // virtual void on_stream_exception(RtcStream *stream) = 0;
  };
  class RtcStream : public sigslot::has_slots<>
  {

  public:
    RtcStream(EventLoop *el, PortAllocator *allocator, const std::shared_ptr<LRtcMsg> &msg);
    virtual ~RtcStream();
    virtual std::string create_offer_sdp() = 0;

  public:
    int start(rtc::RTCCertificate *certificate);

  protected:
    EventLoop *el_;
    uint64_t uid_;
    std::string stream_name_;
    bool audio_;
    bool video_;
    bool dtls_on_;
    uint32_t log_id_;
    std::unique_ptr<PeerConnection> pc_;
    TimerWatcher *ice_timeout_watcher_ = nullptr;

    friend class RtcStreamManager;
    friend void ice_timeout_cb(EventLoop *el, TimerWatcher *w, void *data);
  };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_RTC_STREAM_H_
