/*
 * @Author: L yongmeng
 * @Date: 2024-04-08 18:26:23
 * @LastEditTime: 2024-04-16 17:50:36
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */
#include <rtc_base/logging.h>

#include "base/event_loop.h"
#include "stream/push_stream.h"
#include "stream/rtp_stream_manager.h"
#include "ice/port_allocator.h"
#include "server/settings.h"

namespace lrtc
{
    RTPStreamManager::RTPStreamManager(EventLoop *event_loop)
        : el_(event_loop),
          port_allocator_(new PortAllocator)
    {
        port_allocator_->set_port_range(Singleton<Settings>::Instance()->IceMinPort(), Singleton<Settings>::Instance()->IceMaxPort());
    }
    RTPStreamManager::~RTPStreamManager()
    {
    }

    int RTPStreamManager::create_push_stream(const std::shared_ptr<LRtcMsg> &msg, std::string &sdp)
    {
        PushStream *stream = find_push_stream(msg->stream_name);
        if (stream)
        {
            push_stream_map_.erase(msg->stream_name);
            delete stream;
        }

        stream = new PushStream(el_, port_allocator_.get(), msg);
        stream->start((rtc::RTCCertificate *)msg->certificate);
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
