#ifndef __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_

#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include <rtc_base/ssl_fingerprint.h>

#include "pc/codec_info.h"
#include "ice/ice_credentials.h"
#include "ice/candidate.h"
#include "pc/stream_params.h"

namespace lrtc
{
    struct SsrcInfo
    {
        SsrcInfo()
        {
            ssrc_id = 0;
            cname = "";
            stream_id = "";
        }
        ~SsrcInfo(){};
        uint32_t ssrc_id;
        std::string cname;
        std::string stream_id;
        std::string track_id;
    };

    enum SdpType
    {
        kOffer = 0,
        kAnswer = 1,
        kPranswer,
        kRollback
    };
    enum class MediaType
    {
        MEDIA_TYPE_AUDIO = 0,
        MEDIA_TYPE_VIDEO,
    };

    enum class RtpDirection
    {
        k_send_recv,
        k_send_only,
        k_recv_only,
        k_inactive
    };

    class MediaContentDescription
    {
    public:
        virtual ~MediaContentDescription() {}
        virtual MediaType type() = 0;
        virtual std::string mid() = 0;

        const std::vector<std::shared_ptr<CodecInfo>> &get_codecs() const
        {
            return codecs_;
        }

        RtpDirection direction() { return direction_; }
        void set_direction(RtpDirection direction) { direction_ = direction; }

        bool rtcp_mux() { return use_rtcp_mux_; }
        void set_rtcp_mux(bool mux) { use_rtcp_mux_ = mux; }

        const std::vector<std::shared_ptr<Candidate>> &candidates() { return candidates_; }
        void add_candidates(const std::vector<std::shared_ptr<Candidate>> &candidates)
        {
            candidates_ = candidates;
        }

        const std::vector<std::shared_ptr<StreamParams>> &streams() { return send_streams_; }
        void add_stream(std::shared_ptr<StreamParams> stream)
        {
            send_streams_.push_back(stream);
        }
        std::string rtpDirectionToString(const RtpDirection &direction) const;

        std::string to_string() const;

    protected:
        std::vector<std::shared_ptr<CodecInfo>> codecs_;
        RtpDirection direction_;
        bool use_rtcp_mux_ = true;
        std::vector<std::shared_ptr<Candidate>> candidates_;
        std::vector<std::shared_ptr<StreamParams>> send_streams_;
    };

    class AudioContentDescription : public MediaContentDescription
    {
    public:
        AudioContentDescription();
        ~AudioContentDescription() override;
        MediaType type() override { return MediaType::MEDIA_TYPE_AUDIO; }
        std::string mid() override { return "audio"; }
    };

    class VideoContentDescription : public MediaContentDescription
    {
    public:
        ~VideoContentDescription() override;
        VideoContentDescription(int h264_codec_id = 107, int rtx_codec_id = 99);
        MediaType type() override { return MediaType::MEDIA_TYPE_VIDEO; }
        std::string mid() override { return "video"; }
    };

    class ContentGroup
    {
    public:
        ContentGroup(const std::string &semantics) : semantics_(semantics) {}
        ~ContentGroup() {}

    public:
        std::string semantics() const { return semantics_; }
        const std::vector<std::string> &content_names() const { return content_names_; }
        bool has_content_name(const std::string &content_name);
        void add_content_name(const std::string &content_name);

    private:
        std::string semantics_;
        std::vector<std::string> content_names_;
    };

    enum ConnectionRole
    {
        NONE = 0,
        ACTIVE,
        PASSIVE,
        ACTPASS,
        HOLDCONN
    };

    class TransportDescription
    {
    public:
        TransportDescription()
        {
        }
        ~TransportDescription()
        {
        }
        std::string mid;
        std::string ice_ufrag;
        std::string ice_pwd;
        std::unique_ptr<rtc::SSLFingerprint> identity_fingerprint;
        ConnectionRole connection_role = ConnectionRole::NONE;
    };
    class SessionDescription
    {

    public:
        SessionDescription(SdpType type);
        ~SessionDescription();

        SdpType type() const { return sdp_type_; }
        std::string to_string(bool dtls_on);

        std::shared_ptr<MediaContentDescription> get_content(const std::string &mid);
        void add_content(std::shared_ptr<MediaContentDescription> content);
        const std::vector<std::shared_ptr<MediaContentDescription>> &contents() const
        {
            return contents_;
        }
        // bundle 行
        void add_group(const ContentGroup &group);
        std::vector<const ContentGroup *> get_group_by_name(const std::string &name) const;

        bool add_transport_info(const std::string &mid, const IceParameters &ice_param,
                                rtc::RTCCertificate *certificate);
        bool add_transport_info(std::shared_ptr<TransportDescription> td);
        std::shared_ptr<TransportDescription> get_transport_info(const std::string &mid);

        bool is_bundle(const std::string &mid);
        std::string get_first_bundle_mid();

        std::shared_ptr<AudioContentDescription> get_audio_content_description();
        std::shared_ptr<VideoContentDescription> get_video_content_description();

        static std::shared_ptr<SessionDescription> parse_session_description(const std::string &sdp, const SdpType type, int &h264_codec_id, int &rtx_codec_id);

    private:
        SdpType sdp_type_;
        std::vector<std::shared_ptr<MediaContentDescription>> contents_;
        std::vector<ContentGroup> content_groups_;
        std::vector<std::shared_ptr<TransportDescription>> transport_infos_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_
