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
#include "rtp_stream_manager.h"

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
        push_stream_map_[msg->stream_name] = stream;

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

    int RTPStreamManager::set_answer(const std::shared_ptr<LRtcMsg> &msg)
    {
        if ("push" == msg->stream_type)
        {
            PushStream *push_stream = find_push_stream(msg->stream_name);
            if (!push_stream)
            {
                RTC_LOG(LS_ERROR) << "set_answer: push stream not found, stream_name=" << msg->stream_name
                                  << ", uid=" << msg->uid
                                  << ", logid =" << msg->log_id;
                return -1;
            }
            return push_stream->set_remote_sdp(msg->sdp);
        }
        else if ("pull" == msg->stream_type)
        {
        }
        else
        {
        }

        return 0;
    }

} // namespace lrtc
