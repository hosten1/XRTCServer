/*
 * @Author: L yongmeng
 * @Date: 2024-04-08 18:26:23
 * @LastEditTime: 2024-04-16 17:50:36
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */

#include "stream/push_stream.h"
#include "stream/rtp_stream_manager.h"

namespace lrtc
{
    RTPStreamManager::RTPStreamManager(EventLoop *event_loop)
        : el_(event_loop)
    {
    }
    RTPStreamManager::~RTPStreamManager()
    {
    }

    int RTPStreamManager::create_push_stream(uint64_t uid, const std::string &stream_name, bool audio, bool video, uint32_t log_id, rtc::RTCCertificate *certificate, std::string &sdp)
    {
        PushStream *stream = find_push_stream(stream_name);
        if (stream)
        {
            push_stream_map_.erase(stream_name);
            delete stream;
        }

        stream = new PushStream(el_, uid, stream_name, audio, video, log_id);
        stream->start(certificate);
        sdp = stream->create_offer_sdp();

        return 0;
    }

    PushStream *RTPStreamManager::find_push_stream(const std::string &stream_name)
    {
        auto ite = push_stream_map_.find(stream_name);
        if (ite != push_stream_map_.end())
        {
            return ite->second;
        }

        return nullptr;
    }

} // namespace lrtc
