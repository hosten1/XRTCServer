/*
 * @Author: L yongmeng
 * @Date: 2024-04-09 09:34:44
 * @LastEditTime: 2024-04-16 17:32:50
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */
#ifndef __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_

#include <string>
#include <stdint.h>
#include <memory>
#include <rtc_base/rtc_certificate.h>
#include <system_wrappers/include/clock.h>
#include <rtc_base/third_party/sigslot/sigslot.h>
#include <rtc_base/copy_on_write_buffer.h>
#include "ice/ice_def.h"

#include "base/event_loop.h"
#include "pc/session_description.h"
#include "pc/transport_controller.h"

namespace lrtc
{
    struct RTCOfferAnswerOptions
    {
        bool send_audio = true;
        bool send_video = true;
        bool recv_audio = true;
        bool recv_video = true;
        bool use_rtp_mux = true;
        bool use_rtcp_mux = true;
        bool dtls_on = true;
    };
    class PeerConnection
    {

    public:
        PeerConnection(EventLoop *el, PortAllocator *allocator, bool dtls_on);
        ~PeerConnection();
        int init(rtc::RTCCertificate *certificate);
        void destroy();

        std::string create_offer_sdp(const RTCOfferAnswerOptions &options);

    private:
        EventLoop *el_;
        TimerWatcher *destroy_timer_ = nullptr;
        rtc::RTCCertificate *certificate_ = nullptr;
        std::unique_ptr<SessionDescription> local_session_description_;

        std::unique_ptr<TransportController> transport_controller_;

        webrtc::Clock *clock_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_
