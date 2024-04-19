/*
 * @Author: L yongmeng
 * @Date: 2024-04-09 09:36:18
 * @LastEditTime: 2024-04-16 17:46:46
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */
#include "pc/peer_connection.h"

#include <rtc_base/logging.h>
#include <rtc_base/helpers.h>
#include <api/task_queue/default_task_queue_factory.h>

#include "peer_connection.h"
#include "ice/ice_credentials.h"

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

    PeerConnection::PeerConnection(EventLoop *el, PortAllocator *allocator, bool dtls_on) : el_(el),
                                                                                            transport_controller_(new TransportController(el, allocator, dtls_on)),
                                                                                            clock_(webrtc::Clock::GetRealTimeClock())
    {
        transport_controller_->signal_candidate_allocate_done.connect(this,
                                                                      &PeerConnection::_on_candidate_allocate_done);
        // transport_controller_->signal_connection_state.connect(this,
        //                                                        &PeerConnection::_on_connection_state);
        // transport_controller_->signal_rtp_packet_received.connect(this,
        //                                                           &PeerConnection::_on_rtp_packet_received);
        // transport_controller_->signal_rtcp_packet_received.connect(this,
        //                                                            &PeerConnection::_on_rtcp_packet_received);
    }

    PeerConnection::~PeerConnection()
    {
        if (destroy_timer_)
        {
            // el_->delete_timer(destroy_timer_);
            // destroy_timer_ = nullptr;
        }

        // if (video_recv_stream_)
        // {
        //     delete video_recv_stream_;
        //     video_recv_stream_ = nullptr;
        // }

        // if (audio_recv_stream_)
        // {
        //     delete audio_recv_stream_;
        //     audio_recv_stream_ = nullptr;
        // }

        RTC_LOG(LS_INFO) << "PeerConnection destroy";
    }

    int PeerConnection::init(rtc::RTCCertificate *certificate)
    {
        certificate_ = certificate;
        transport_controller_->set_local_certificate(certificate);
        return 0;
    }

    void PeerConnection::destroy()
    {
        if (destroy_timer_)
        {
            el_->delete_timer_event(destroy_timer_);
            destroy_timer_ = nullptr;
        }

        // destroy_timer_ = el_->create_timer_event(destroy_timer_cb, this, false);
        // el_->start_timer_event(destroy_timer_, 10000); // 10ms
    }

    std::string PeerConnection::create_offer_sdp(const RTCOfferAnswerOptions &options)
    {
        options_ = options;
        RTC_LOG(LS_INFO) << "create_offer_sdp certificate_:" << certificate_;
        if (options.dtls_on && !certificate_)
        {
            RTC_LOG(LS_ERROR) << "create_offer_sdp: certificate is null";
            return "";
        }

        local_session_description_ = std::make_unique<SessionDescription>(SdpType::kOffer);

        IceParameters ice_param = IceCredentials::create_random_ice_credentials();

        if (options.recv_audio)
        {
            auto audio = std::make_shared<AudioContentDescription>();
            audio->set_direction(get_direction(options.send_audio, options.recv_audio));
            audio->set_rtcp_mux(options.use_rtcp_mux);
            local_session_description_->add_content(audio);
            local_session_description_->add_transport_info(audio->mid(), ice_param, certificate_);
        }
        if (options.recv_video)
        {
            auto video = std::make_shared<VideoContentDescription>();
            video->set_direction(get_direction(options.send_video, options.recv_video));
            video->set_rtcp_mux(options.use_rtcp_mux);
            local_session_description_->add_content(video);
            local_session_description_->add_transport_info(video->mid(), ice_param, certificate_);
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

        transport_controller_->set_local_description(local_session_description_.get());

        return local_session_description_->to_string(false);
    }

    int PeerConnection::set_remote_sdp(const std::string &sdp)
    {
        RTC_LOG(LS_INFO) << __FUNCTION__;
        remote_desc_ = SessionDescription::parse_session_description(sdp, kOffer, h264_codec_id_, rtx_codec_id_);
        std::shared_ptr<AudioContentDescription> audio_content = remote_desc_->get_audio_content_description();
        std::shared_ptr<VideoContentDescription> video_content = remote_desc_->get_video_content_description();
        RTC_LOG(LS_INFO) << "set_remote_sdp audio_content:" << audio_content->to_string() << " video_content:" << video_content->to_string();
        if (audio_content)
        {
            exist_push_audio_source_ = true;
            auto audio_codecs = audio_content->get_codecs();
            if (!audio_codecs.empty())
            {
                audio_payload_type_ = audio_codecs[0]->id;
            }

            if (options_.recv_audio)
            {
                // _create_audio_receive_stream(audio_content.get());
            }
        }

        if (video_content)
        {
            exist_push_video_source_ = true;

            auto video_codecs = video_content->get_codecs();
            if (!video_codecs.empty())
            {
                video_payload_type_ = video_codecs[0]->id;
            }

            if (video_codecs.size() > 1)
            {
                video_rtx_payload_type_ = video_codecs[1]->id;
            }

            if (options_.recv_video)
            {
                // _create_video_receive_stream(video_content.get());
            }
        }

        // transport_controller_->set_remote_description(remote_desc_.get());

        return 0;
    }

    void PeerConnection::_on_candidate_allocate_done(TransportController *transport_controller, const std::string &transport_name, IceCandidateComponent component, const std::vector<Candidate> &candidates)
    {

        if (!local_session_description_)
        {
            return;
        }
        auto content = local_session_description_->get_content(transport_name);

        std::vector<std::shared_ptr<lrtc::Candidate>> result;
        for (auto &candidate : candidates)
        {
            result.push_back(std::make_shared<lrtc::Candidate>(candidate));
            RTC_LOG(LS_INFO) << "candidate: " << candidate.to_string();
        }
        content->add_candidates(result);
    }

} // namespace lrtc
