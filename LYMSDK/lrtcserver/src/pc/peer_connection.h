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
#include "pc/stream_params.h"

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
    class PeerConnection : public sigslot::has_slots<>
    {

    public:
        PeerConnection(EventLoop *el, PortAllocator *allocator, bool dtls_on);
        ~PeerConnection();
        int init(rtc::RTCCertificate *certificate);
        void destroy();

        std::string create_offer_sdp(const RTCOfferAnswerOptions &options);
        int set_remote_sdp(const std::string &sdp);

        SessionDescription *remote_desc() { return remote_desc_.get(); }
        SessionDescription *local_desc() { return local_session_description_.get(); }

        void add_audio_source(const std::vector<StreamParams> &source)
        {
            audio_source_ = source;
        }

        void add_video_source(const std::vector<StreamParams> &source)
        {
            video_source_ = source;
        }

    private:
        void _on_candidate_allocate_done(TransportController *transport_controller,
                                         const std::string &transport_name,
                                         IceCandidateComponent component,
                                         const std::vector<Candidate> &candidates);

    private:
        EventLoop *el_;
        TimerWatcher *destroy_timer_ = nullptr;
        rtc::RTCCertificate *certificate_ = nullptr;
        std::unique_ptr<SessionDescription> local_session_description_;
        std::shared_ptr<SessionDescription> remote_desc_;

        std::unique_ptr<TransportController> transport_controller_;

        std::vector<StreamParams> audio_source_;
        std::vector<StreamParams> video_source_;

        uint32_t remote_audio_ssrc_ = 0;
        uint32_t remote_video_ssrc_ = 0;
        uint32_t remote_video_rtx_ssrc_ = 0;

        uint8_t video_payload_type_ = 0;
        uint8_t video_rtx_payload_type_ = 0;
        uint8_t audio_payload_type_ = 0;

        webrtc::Clock *clock_;

        RTCOfferAnswerOptions options_;

        bool exist_push_audio_source_ = false;
        bool exist_push_video_source_ = false;
        int h264_codec_id_ = 0;
        int rtx_codec_id_ = 0;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_
