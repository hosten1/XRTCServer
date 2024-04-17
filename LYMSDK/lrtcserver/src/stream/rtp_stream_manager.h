#ifndef __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_
#define __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <memory>

#include <rtc_base/rtc_certificate.h>
#include "stream/rtc_stream.h"

namespace lrtc
{
    class EventLoop;
    class PushStream;
    class PortAllocator;
    class PullStream;

    class RTPStreamManager
    {

    public:
        RTPStreamManager(EventLoop *event_loop);
        ~RTPStreamManager();

        int create_push_stream(const std::shared_ptr<LRtcMsg> &msg, std::string &sdp);
        PushStream *find_push_stream(const std::string &stream_name);

    private:
        EventLoop *el_;
        std::unordered_map<std::string, PushStream *> push_stream_map_;
        std::unordered_map<std::string, PullStream *> pull_streams_;
        std::unique_ptr<PortAllocator> port_allocator_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_STREAM_RTP_STREAM_MANAGER_H_
