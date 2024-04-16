/*
 * @Author: L yongmeng
 * @Date: 2024-04-09 09:36:18
 * @LastEditTime: 2024-04-16 16:40:47
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */
#include "pc/peer_connection.h"
#include "peer_connection.h"
#include "ice/ice_credentials.h"

#include <rtc_base/logging.h>

namespace lrtc
{
    static RtpDirection get_direction(bool send, bool recv)
    {
        if (send && recv)
        {
            return RtpDirection::k_send_recv;
        }
        else if (send)
        {
            return RtpDirection::k_send_only;
        }
        else if (recv)
        {
            return RtpDirection::k_recv_only;
        }
        else
        {
            return RtpDirection::k_inactive;
        }
    }

    PeerConnection::PeerConnection(EventLoop *el)
        : el_(el)
    {
    }

    PeerConnection::~PeerConnection()
    {
    }

    std::string PeerConnection::create_offer_sdp(const RTCOfferAnswerOptions &options)
    {
        RTC_LOG(LS_INFO) << "create_offer_sdp";
        local_session_description_ = std::make_unique<SessionDescription>(SdpType::kOffer);

        IceParameters ice_param = IceCredentials::create_random_ice_credentials();

        if (options.recv_audio)
        {
            auto audio = std::make_shared<AudioContentDescription>();
            audio->set_direction(get_direction(options.send_audio, options.recv_audio));
            audio->set_rtcp_mux(options.use_rtcp_mux);
            local_session_description_->add_content(audio);
            local_session_description_->add_transport_info(audio->mid(), ice_param);
        }
        if (options.recv_video)
        {
            auto video = std::make_shared<VideoContentDescription>();
            video->set_direction(get_direction(options.send_video, options.recv_video));
            video->set_rtcp_mux(options.use_rtcp_mux);
            local_session_description_->add_content(video);
            local_session_description_->add_transport_info(video->mid(), ice_param);
        }
        if (options.use_rtp_mux)
        {
            ContentGroup offer_bundle("BUNDLE");
            for (auto &content : local_session_description_->contents())
            {
                offer_bundle.add_content_name(content->mid());
            }
            if (!offer_bundle.content_names().empty())
            {
                local_session_description_->add_group(offer_bundle);
            }
        }

        return local_session_description_->to_string(false);
    }

} // namespace lrtc
