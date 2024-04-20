
#include <sstream>

#include <absl/algorithm/container.h>

#include <rtc_base/logging.h>
#include "pc/session_description.h"
#include "ice/ice_def.h"
#include "session_description.h"
namespace lrtc
{
    const char k_media_protocol_dtls_savpf[] = "UDP/TLS/RTP/SAVPF";
    const char k_meida_protocol_savpf[] = "RTP/SAVPF";

    // 从 a=ice-ufrag:w/W3\r\n 里面取出w/W3
    static std::string get_attribute(const std::string &line)
    {
        std::vector<std::string> fields;
        size_t size = rtc::tokenize(line, ':', &fields);
        if (size != 2)
        {
            RTC_LOG(LS_WARNING) << "get attribute error: " << line;
            return "";
        }

        return fields[1];
    }

    static int parse_transport_info(TransportDescription *td, const std::string &line)
    {
        // a=ice-ufrag:
        if (line.find("a=ice-ufrag") != std::string::npos)
        {
            td->ice_ufrag = get_attribute(line);
            if (td->ice_ufrag.empty())
            {
                return -1;
            }

            // a=ice-pwd:
        }
        else if (line.find("a=ice-pwd") != std::string::npos)
        {
            td->ice_pwd = get_attribute(line);
            if (td->ice_pwd.empty())
            {
                return -1;
            }

            // a=fingerprint:
        }
        else if (line.find("a=fingerprint") != std::string::npos)
        {
            std::vector<std::string> items;
            rtc::tokenize(line, ' ', &items);
            if (items.size() != 2)
            {
                RTC_LOG(LS_WARNING) << "parse a=fingerprint error: " << line;
                return -1;
            }

            // 字符串a=fingerprint: 是14字节，还需注意大小写转换
            std::string alg = items[0].substr(14);
            absl::c_transform(alg, alg.begin(), ::tolower);
            std::string content = items[1];

            td->identity_fingerprint = rtc::SSLFingerprint::CreateUniqueFromRfc4572(
                alg, content);
            if (!(td->identity_fingerprint.get()))
            {
                RTC_LOG(LS_WARNING) << "create fingerprint error: " << line;
                return -1;
            }
        }
        return 0;
    }

    static int parse_ssrc_info(std::vector<std::shared_ptr<SsrcInfo>> &ssrc_info, const std::string &line)
    {
        if (line.find("a=ssrc:") == std::string::npos)
        {
            return 0;
        }

        // rfc5576
        // a=ssrc:<ssrc-id> <attribute>
        // a=ssrc:<ssrc-id> <attribute>:<value>
        std::string field1, field2;
        if (!rtc::tokenize_first(line.substr(2), ' ', &field1, &field2))
        {
            RTC_LOG(LS_WARNING) << "parse a=ssrc failed, line: " << line;
            return -1;
        }

        // ssrc:<ssrc-id>
        std::string ssrc_id_s = field1.substr(5);
        uint32_t ssrc_id = 0;
        if (!rtc::FromString(ssrc_id_s, &ssrc_id))
        {
            RTC_LOG(LS_WARNING) << "invalid ssrc_id, line: " << line;
            return -1;
        }

        // <attribute>
        std::string attribute;
        std::string value;
        if (!rtc::tokenize_first(field2, ':', &attribute, &value))
        {
            RTC_LOG(LS_WARNING) << "get ssrc attribute failed, line: " << line;
            return -1;
        }

        // ssrc_info里面找是否存在ssrc_id的一行
        auto iter = ssrc_info.begin();
        for (; iter != ssrc_info.end(); ++iter)
        {
            if ((*iter)->ssrc_id == ssrc_id)
            {
                break;
            }
        }

        // 如果ssrc_info里没有找到ssrc_id的一行，则插入一条ssrc信息
        if (iter == ssrc_info.end())
        {
            std::shared_ptr<SsrcInfo> info = std::make_shared<SsrcInfo>();
            info->ssrc_id = ssrc_id;
            ssrc_info.push_back(info);
            iter = ssrc_info.end() - 1;
        }

        // a=ssrc:3038623782 cname:9UkMttm/AKBk/3gN
        if ("cname" == attribute)
        {
            (*iter)->cname = value;

            // a=ssrc:3038623782 msid:Z0HUtsuZwWwocPvLkt8PANm3axsdHekKKIXA 19a75650-d2ad-45d8-b7f4-53398701f7de
        }
        else if ("msid" == attribute)
        {
            std::vector<std::string> fields;
            rtc::split(value, ' ', &fields);
            if (fields.size() < 1 || fields.size() > 2)
            {
                RTC_LOG(LS_WARNING) << "msid format error, line: " << line;
                return -1;
            }

            (*iter)->stream_id = fields[0];
            if (fields.size() == 2)
            {
                (*iter)->track_id = fields[1];
            }
        }

        return 0;
    }

    // 解析 a=ssrc-group:FID
    static int parse_ssrc_group_info(std::vector<std::shared_ptr<SsrcGroup>> &ssrc_groups, const std::string &line)
    {
        if (line.find("a=ssrc-group:") == std::string::npos)
        {
            return 0;
        }

        // rfc5576
        // a=ssrc-group:<semantics> <ssrc-id> ...
        std::vector<std::string> fields;
        rtc::split(line.substr(2), ' ', &fields);
        if (fields.size() < 2)
        {
            RTC_LOG(LS_WARNING) << "ssrc-group field size < 2, line: " << line;
            return -1;
        }

        std::string semantics = get_attribute(fields[0]);
        if (semantics.empty())
        {
            return -1;
        }

        std::vector<uint32_t> ssrcs;
        for (size_t i = 1; i < fields.size(); ++i)
        {
            uint32_t ssrc_id = 0;
            if (!rtc::FromString(fields[i], &ssrc_id))
            {
                return -1;
            }
            ssrcs.push_back(ssrc_id);
        }
        std::shared_ptr<SsrcGroup> ssrc_group = std::make_shared<SsrcGroup>(semantics, ssrcs);
        ssrc_groups.push_back(ssrc_group);

        return 0;
    }

    // 暂时采用比较粗暴的做法
    // a=fmtp:100
    static int parse_fmtp_info(int &h264_codec_id, const std::string &line)
    {
        if (h264_codec_id != 0)
        {
            return 0;
        }

        if (line.find("42e01f") == std::string::npos)
        {
            return 0;
        }

        if (line.find("level-asymmetry-allowed=1") == std::string::npos)
        {
            return 0;
        }

        if (line.find("packetization-mode=1") == std::string::npos)
        {
            return 0;
        }

        std::vector<std::string> fields;
        rtc::split(line.substr(2), ' ', &fields);
        if (fields.size() < 2)
        {
            RTC_LOG(LS_WARNING) << "rtpmap field size < 2, line: " << line;
            return -1;
        }

        std::string code_id = get_attribute(fields[0]);
        if (code_id.empty())
        {
            return -1;
        }

        h264_codec_id = atoi(code_id.c_str());
        RTC_LOG(LS_INFO) << "fmtp h264 code id: " << h264_codec_id;

        return 0;
    }

    // a=rtpmap:101 rtx/90000
    static int parse_rtpmap_info(int &rtx_codec_id, const std::string &line)
    {
        if (rtx_codec_id != 0)
        {
            return 0;
        }

        if (line.find("rtx/90000") == std::string::npos)
        {
            return 0;
        }

        std::vector<std::string> fields;
        rtc::split(line.substr(2), ' ', &fields);
        if (fields.size() < 2)
        {
            RTC_LOG(LS_WARNING) << "rtpmap field size < 2, line: " << line;
            return -1;
        }

        std::string code_id = get_attribute(fields[0]);
        if (code_id.empty())
        {
            return -1;
        }

        rtx_codec_id = atoi(code_id.c_str());
        RTC_LOG(LS_INFO) << "rtpmap rtx code id: " << rtx_codec_id;

        return 0;
    }

    static void create_track_from_ssrc_info(const std::vector<std::shared_ptr<SsrcInfo>> &ssrc_infos,
                                            std::vector<std::shared_ptr<StreamParams>> &tracks)
    {
        for (auto &ssrc_info : ssrc_infos)
        {
            std::string track_id = ssrc_info->track_id;

            auto iter = tracks.begin();
            for (; iter != tracks.end(); ++iter)
            {
                if ((*iter)->id == track_id)
                {
                    break;
                }
            }

            if (iter == tracks.end())
            {
                std::shared_ptr<StreamParams> track = std::make_shared<StreamParams>();
                track->id = track_id;
                tracks.push_back(track);
                iter = tracks.end() - 1;
            }

            (*iter)->cname = ssrc_info->cname;
            (*iter)->stream_id = ssrc_info->stream_id;
            (*iter)->ssrcs.push_back(ssrc_info->ssrc_id);
        }
    }

    AudioContentDescription::AudioContentDescription()
    {
        auto codec = std::make_shared<AudioCodecInfo>();
        codec->id = 111;
        codec->name = "opus";
        codec->samplerate = 48000;
        codec->channels = 2;

        // add feedback param
        codec->feedback_param.push_back(FeedbackParam("transport-cc"));

        // add codec param
        codec->codec_param["minptime"] = "10";
        codec->codec_param["useinbandfec"] = "1";

        codecs_.push_back(codec);
    }

    AudioContentDescription::~AudioContentDescription()
    {
    }

    VideoContentDescription::~VideoContentDescription()
    {
    }

    VideoContentDescription::VideoContentDescription(int h264_codec_id, int rtx_codec_id)
    {
        auto codec = std::make_shared<VideoCodecInfo>();
        codec->id = h264_codec_id; // 107
        codec->name = "H264";
        codec->samplerate = 90000;
        codecs_.push_back(codec);

        // add feedback param
        codec->feedback_param.push_back(FeedbackParam("goog-remb"));
        codec->feedback_param.push_back(FeedbackParam("transport-cc"));
        codec->feedback_param.push_back(FeedbackParam("ccm", "fir"));
        codec->feedback_param.push_back(FeedbackParam("nack"));
        codec->feedback_param.push_back(FeedbackParam("nack", "pli"));

        // add codec param
        codec->codec_param["level-asymmetry-allowed"] = "1";
        codec->codec_param["packetization-mode"] = "1";
        codec->codec_param["profile-level-id"] = "42e01f";

        auto rtx_codec = std::make_shared<VideoCodecInfo>();
        rtx_codec->id = rtx_codec_id; // 99;
        rtx_codec->name = "rtx";
        rtx_codec->samplerate = 90000;

        // add codec param
        rtx_codec->codec_param["apt"] = std::to_string(codec->id);
        codecs_.push_back(rtx_codec);
    }

    bool ContentGroup::has_content_name(const std::string &content_name)
    {
        for (auto name : content_names_)
        {
            if (name == content_name)
            {
                return true;
            }
        }

        return false;
    }

    std::shared_ptr<MediaContentDescription> SessionDescription::get_content(
        const std::string &mid)
    {
        for (auto content : contents_)
        {
            if (mid == content->mid())
            {
                return content;
            }
        }

        return nullptr;
    }

    void ContentGroup::add_content_name(const std::string &content_name)
    {
        if (!has_content_name(content_name))
        {
            content_names_.push_back(content_name);
        }
    }

    SessionDescription::SessionDescription(SdpType type) : sdp_type_(type)
    {
    }

    SessionDescription::~SessionDescription()
    {
    }

    static void add_fmtp_line(std::shared_ptr<CodecInfo> codec,
                              std::stringstream &ss)
    {
        if (!codec->codec_param.empty())
        {
            ss << "a=fmtp:" << codec->id << " ";
            std::string data;
            for (auto param : codec->codec_param)
            {
                data += (";" + param.first + "=" + param.second);
            }
            // data = ";key1=value1;key2=value2"
            data = data.substr(1);
            ss << data << "\r\n";
        }
    }

    static void add_rtcp_fb_line(std::shared_ptr<CodecInfo> codec,
                                 std::stringstream &ss)
    {
        for (auto param : codec->feedback_param)
        {
            ss << "a=rtcp-fb:" << codec->id << " " << param.id();
            if (!param.param().empty())
            {
                ss << " " << param.param();
            }
            ss << "\r\n";
        }
    }

    static void build_rtp_map(std::shared_ptr<MediaContentDescription> content,
                              std::stringstream &ss)
    {
        for (auto codec : content->get_codecs())
        {
            ss << "a=rtpmap:" << codec->id << " " << codec->name << "/" << codec->samplerate;
            if (MediaType::MEDIA_TYPE_AUDIO == content->type())
            {
                auto audio_codec = codec->as_audio();
                ss << "/" << audio_codec->channels;
            }
            ss << "\r\n";

            add_rtcp_fb_line(codec, ss);
            add_fmtp_line(codec, ss);
        }
    }

    static void build_rtp_direction(std::shared_ptr<MediaContentDescription> content, std::stringstream &ss)
    {
        switch (content->direction())
        {
        case RtpDirection::k_send_recv:
            ss << "a=sendrecv\r\n";
            break;
        case RtpDirection::k_send_only:
            ss << "a=sendonly\r\n";
            break;
        case RtpDirection::k_recv_only:
            ss << "a=recvonly\r\n";
            break;
        default:
            ss << "a=inactive\r\n";
            break;
        }
    }

    void SessionDescription::add_content(std::shared_ptr<MediaContentDescription> content)
    {
        contents_.push_back(content);
    }

    void SessionDescription::add_group(const ContentGroup &group)
    {
        content_groups_.push_back(group);
    }

    std::vector<const ContentGroup *> SessionDescription::get_group_by_name(const std::string &name) const
    {
        std::vector<const ContentGroup *> content_groups;
        for (const ContentGroup &group : content_groups_)
        {
            if (group.semantics() == name)
            {
                content_groups.push_back(&group);
            }
        }

        return content_groups;
    }

    static std::string connection_role_to_string(ConnectionRole role)
    {
        switch (role)
        {
        case ConnectionRole::ACTIVE:
            return "active";
        case ConnectionRole::PASSIVE:
            return "passive";
        case ConnectionRole::ACTPASS:
            return "actpass";
        case ConnectionRole::HOLDCONN:
            return "holdconn";
        default:
            return "none";
        }
    }

    bool SessionDescription::add_transport_info(const std::string &mid,
                                                const IceParameters &ice_param,
                                                rtc::RTCCertificate *certificate)
    {
        auto desc = std::make_shared<TransportDescription>();
        desc->mid = mid;
        desc->ice_ufrag = ice_param.ice_ufrag;
        desc->ice_pwd = ice_param.ice_pwd;

        if (certificate)
        {
            desc->identity_fingerprint = rtc::SSLFingerprint::CreateFromCertificate(*certificate);
            if (!desc->identity_fingerprint)
            {
                RTC_LOG(LS_WARNING) << "get fingerprint failed";
                return false;
            }
        }

        if (SdpType::kOffer == sdp_type_)
        {
            desc->connection_role = ConnectionRole::ACTPASS;
        }
        else
        {
            desc->connection_role = ConnectionRole::PASSIVE;
        }

        transport_infos_.emplace_back(std::move(desc));

        return true;
    }

    bool SessionDescription::add_transport_info(std::shared_ptr<TransportDescription> td)
    {
        transport_infos_.emplace_back(td);
        return true;
    }

    std::shared_ptr<TransportDescription> SessionDescription::get_transport_info(const std::string &mid)
    {
        for (auto info : transport_infos_)
        {
            if (info->mid == mid)
            {
                return info;
            }
        }

        return nullptr;
    }

    bool SessionDescription::is_bundle(const std::string &mid)
    {
        auto content_group = get_group_by_name("BUNDLE");
        if (content_group.empty())
        {
            return false;
        }

        for (auto group : content_group)
        {
            for (auto name : group->content_names())
            {
                if (name == mid)
                {
                    return true;
                }
            }
        }

        return false;
    }

    std::string SessionDescription::get_first_bundle_mid()
    {
        auto content_group = get_group_by_name("BUNDLE");
        if (content_group.empty())
        {
            return "";
        }

        return content_group[0]->content_names()[0];
    }

    std::shared_ptr<AudioContentDescription> SessionDescription::get_audio_content_description()
    {
        auto it = std::find_if(contents_.begin(), contents_.end(), [](const std::shared_ptr<MediaContentDescription> &content)
                               { return content->type() == MediaType::MEDIA_TYPE_AUDIO; });
        if (it != contents_.end())
        {
            return std::static_pointer_cast<AudioContentDescription>(*it);
        }
        else
        {
            return nullptr; // 或者根据需要返回其他值
        }
    }

    std::shared_ptr<VideoContentDescription> SessionDescription::get_video_content_description()
    {
        auto it = std::find_if(contents_.begin(), contents_.end(), [](const std::shared_ptr<MediaContentDescription> &content)
                               { return content->type() == MediaType::MEDIA_TYPE_VIDEO; });
        if (it != contents_.end())
        {
            return std::static_pointer_cast<VideoContentDescription>(*it);
        }
        else
        {
            return nullptr; // 或者根据需要返回其他值
        }
    }

    static void build_candidates(std::shared_ptr<MediaContentDescription> content,
                                 std::stringstream &ss)
    {
        for (auto c : content->candidates())
        {
            ss << "a=candidate:" << c->foundation
               << " " << c->component
               << " " << c->protocol
               << " " << c->priority
               << " " << c->address.HostAsURIString()
               << " " << c->port
               << " typ " << c->type
               << "\r\n";
        }
    }

    static void add_ssrc_line(uint32_t ssrc, const std::string &attribute,
                              const std::string &value, std::stringstream &ss)
    {
        ss << "a=ssrc:" << ssrc << " " << attribute << ":" << value << "\r\n";
    }

    static void build_ssrc(std::shared_ptr<MediaContentDescription> content, std::stringstream &ss)
    {
        for (auto track : content->streams())
        {
            for (auto ssrc_group : track->ssrc_groups)
            {
                if (ssrc_group->ssrcs.empty())
                {
                    continue;
                }

                ss << "a=ssrc-group:" << ssrc_group->semantics;
                for (auto ssrc : ssrc_group->ssrcs)
                {
                    ss << " " << ssrc;
                }
                ss << "\r\n";
            }

            std::string msid = track->stream_id + " " + track->id;
            for (auto ssrc : track->ssrcs)
            {
                add_ssrc_line(ssrc, "cname", track->cname, ss);
                add_ssrc_line(ssrc, "msid", msid, ss);
                add_ssrc_line(ssrc, "mslabel", track->stream_id, ss);
                add_ssrc_line(ssrc, "lable", track->id, ss);
            }
        }
    }

    std::string SessionDescription::to_string(bool dtls_on)
    {
        std::stringstream ss;
        //  version
        ss << "v=0\r\n";
        // session orifin
        // RFC 4566
        // o= <username> <sessionid> <sessionversion> <nettype> <addrtype> <unicast-address>
        ss << "o=lrtc/1.0 0 2 IN IP4 0.0.0.0\r\n";
        // session name
        ss << "s=LymSDK \r\n";
        // time description 也就是会话的存在时间
        ss << "t=0 0\r\n";
        // 改为ice-lite方式，一起是ice-full方式
        //  ss << "a=ice-lite\r\n";
        //  BUDDLE
        std::vector<const ContentGroup *> content_group = get_group_by_name("BUNDLE");
        if (!content_group.empty())
        {
            ss << "a=group:BUNDLE";
            for (auto group : content_group)
            {
                for (auto content_name : group->content_names())
                {
                    if (content_name == "audio")
                    {
                        ss << " " << 0;
                    }
                    else if (content_name == "video")
                    {
                        ss << " " << 1;
                    }
                }
            }
            ss << "\r\n";
        }

        ss << "a=msid-semantic: WMS  live/lrtc \r\n";

        for (auto content : contents_)
        {
            // RFC 4566
            // m=<media> <port> <proto> <fmt>
            std::string fmt;
            for (auto codec : content->get_codecs())
            {
                fmt.append(" ");
                fmt.append(std::to_string(codec->id));
            }

            std::string meida_protocol;
            if (dtls_on)
            {
                meida_protocol = k_media_protocol_dtls_savpf;
            }
            else
            {
                meida_protocol = k_meida_protocol_savpf;
            }
            ss << "m=" << content->mid() << " 9 " << meida_protocol << fmt << "\r\n";

            ss << "c=IN IP4 0.0.0.0\r\n";
            // ss << "a=rtcp:9 IN IP4 0.0.0.0\r\n";

            auto transport_info = get_transport_info(content->mid());
            if (transport_info)
            {
                ss << "a=ice-ufrag:" << transport_info->ice_ufrag << "\r\n";
                ss << "a=ice-pwd:" << transport_info->ice_pwd << "\r\n";

                auto fp = transport_info->identity_fingerprint.get();
                if (fp)
                {
                    ss << "a=fingerprint:" << fp->algorithm << " " << fp->GetRfc4572Fingerprint()
                       << "\r\n";
                    ss << "a=setup:" << connection_role_to_string(transport_info->connection_role) << "\r\n";
                }
            }

            if (content->mid() == "audio")
            {
                ss << "a=mid:0"
                   << "\r\n";
            }
            else if (content->mid() == "video")
            {
                ss << "a=mid:1"
                   << "\r\n";
            }

            ss << "a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01"
               << "\r\n";

            build_rtp_direction(content, ss);

            if (content->rtcp_mux())
            {
                ss << "a=rtcp-mux\r\n";
            }

            ss << "a=rtcp-rsize\r\n";

            build_rtp_map(content, ss);
            build_ssrc(content, ss);

            build_candidates(content, ss);
        }

        return ss.str();
    }

    std::shared_ptr<SessionDescription> SessionDescription::parse_session_description(const std::string &sdp, const SdpType type, int &h264_codec_id, int &rtx_codec_id)
    {

        bool exist_push_audio_source = false;

        bool exist_push_video_source = false;

        std::vector<std::string> fields;
        size_t size = rtc::tokenize(sdp, '\n', &fields);
        if (size <= 0)
        {
            RTC_LOG(LS_WARNING) << "remote sdp invalid";
            return nullptr;
        }

        bool is_rn = false;
        if (sdp.find("\r\n") != std::string::npos)
        {
            is_rn = true;
        }

        std::shared_ptr<SessionDescription> remote_desc = std::make_shared<SessionDescription>(type);

        std::string media_type;
        std::shared_ptr<AudioContentDescription> audio_content;
        std::shared_ptr<VideoContentDescription> video_content;
        auto audio_td = std::make_shared<TransportDescription>();
        auto video_td = std::make_shared<TransportDescription>();

        std::vector<std::shared_ptr<SsrcInfo>> audio_ssrc_info;
        std::vector<std::shared_ptr<SsrcInfo>> video_ssrc_info;
        std::vector<std::shared_ptr<SsrcGroup>> video_ssrc_groups;
        std::vector<std::shared_ptr<StreamParams>> audio_tracks;
        std::vector<std::shared_ptr<StreamParams>> video_tracks;

        for (auto field : fields)
        {
            if (is_rn)
            {
                field = field.substr(0, field.length() - 1);
            }

            if (field.find("m=group:BUNDLE") != std::string::npos)
            {
                std::vector<std::string> items;
                rtc::split(field, ' ', &items);
                if (items.size() > 1)
                {
                    ContentGroup answer_bundle("BUNDLE");
                    for (size_t i = 1; i < items.size(); ++i)
                    {
                        answer_bundle.add_content_name(items[i]);
                    }
                    remote_desc->add_group(answer_bundle);
                }
            }
            else if (field.find("m=") != std::string::npos)
            {
                std::vector<std::string> items;
                rtc::split(field, ' ', &items);
                if (items.size() <= 2)
                {
                    RTC_LOG(LS_WARNING) << "parse m= error: " << field;
                    return remote_desc;
                }

                // m=audio/video
                media_type = items[0].substr(2);
                if ("audio" == media_type)
                {
                    audio_td->mid = "audio";
                    exist_push_audio_source = true;
                }
                else if ("video" == media_type)
                {
                    video_td->mid = "video";
                    exist_push_video_source = true;
                }
            }
            if ("audio" == media_type)
            {
                if (parse_transport_info(audio_td.get(), field) != 0)
                {
                    return remote_desc;
                }

                if (parse_ssrc_info(audio_ssrc_info, field) != 0)
                {
                    return remote_desc;
                }
            }
            else if ("video" == media_type)
            {
                if (parse_transport_info(video_td.get(), field) != 0)
                {
                    return remote_desc;
                }

                if (parse_ssrc_info(video_ssrc_info, field) != 0)
                {
                    return remote_desc;
                }

                if (parse_ssrc_group_info(video_ssrc_groups, field) != 0)
                {
                    return remote_desc;
                }

                if (parse_fmtp_info(h264_codec_id, field) != 0)
                {
                    return remote_desc;
                }
                if (h264_codec_id != 0)
                {
                    if (parse_rtpmap_info(rtx_codec_id, field) != 0)
                    {
                        return remote_desc;
                    }
                }
            }
        }

        if (exist_push_audio_source)
        {
            audio_content = std::make_shared<AudioContentDescription>();
            remote_desc->add_content(audio_content);
        }

        if (exist_push_video_source)
        {
            video_content = std::make_shared<VideoContentDescription>(h264_codec_id, rtx_codec_id);
            remote_desc->add_content(video_content);
        }

        if (exist_push_audio_source && !audio_ssrc_info.empty())
        {
            create_track_from_ssrc_info(audio_ssrc_info, audio_tracks);

            for (auto &track : audio_tracks)
            {
                audio_content->add_stream(track);
            }
        }
        else
        {
            RTC_LOG(LS_WARNING) << "no audio track exist_push_audio_source = " << exist_push_audio_source
                                << " audio_ssrc_info.empty() = " << audio_ssrc_info.empty();
        }

        if (exist_push_video_source && !video_ssrc_info.empty())
        {
            create_track_from_ssrc_info(video_ssrc_info, video_tracks);

            for (auto &ssrc_group : video_ssrc_groups)
            {
                if (ssrc_group->ssrcs.empty())
                {
                    continue;
                }

                uint32_t ssrc = ssrc_group->ssrcs.front();
                for (auto &track : video_tracks)
                {
                    if (track->has_ssrc(ssrc))
                    {
                        track->ssrc_groups.push_back(ssrc_group);
                    }
                }
            }

            for (auto &track : video_tracks)
            {
                video_content->add_stream(track);
            }
        }
        else
        {
            RTC_LOG(LS_WARNING) << "no video track exist_push_audio_source = " << exist_push_video_source
                                << " audio_ssrc_info.empty() = " << video_ssrc_info.empty();
        }

        remote_desc->add_transport_info(audio_td);
        remote_desc->add_transport_info(video_td);

        return remote_desc;
    }
    std::string MediaContentDescription::rtpDirectionToString(const RtpDirection &direction) const
    {
        switch (direction)
        {
        case RtpDirection::k_send_recv:
            return "a=sendrecv";
            break;
        case RtpDirection::k_send_only:
            return "a=sendonly\r\n";
            break;
        case RtpDirection::k_recv_only:
            return "a=recvonly\r\n";
            break;
        default:
            return "a=inactive\r\n";
            break;
        }
    }
    std::string MediaContentDescription::to_string() const
    {
        {
            std::ostringstream oss;
            oss << "codecs_: ";
            for (const auto &codec : codecs_)
            {
                oss << codec->to_string() << ", ";
            }
            // 添加其他成员变量的输出
            // 这里是一个示例，您可以按照需要添加其他成员变量的输出
            oss << "direction_: " << rtpDirectionToString(direction_) << ", ";
            oss << "use_rtcp_mux_: " << (use_rtcp_mux_ ? "true" : "false") << ", ";
            oss << "candidates_: ";
            for (const auto &candidate : candidates_)
            {
                oss << candidate->to_string() << ", ";
            }
            oss << "send_streams_: ";
            for (const auto &stream : send_streams_)
            {
                oss << stream->to_string() << ", ";
            }
            return oss.str();
        }
    }

} // namespace lrtc
