
#include "stream/push_stream.h"
#include "stream/rtp_stream_manager.h"




namespace lrtc {
RTPStreamManager::RTPStreamManager(EventLoop *event_loop)
 :el_(event_loop)
{
}
RTPStreamManager::~RTPStreamManager()
{

}


int RTPStreamManager::create_push_stream(uint64_t uid, const std::string &stream_name, bool audio, bool video, uint32_t log_id, std::string &sdp)
{
    PushStream *stream = new PushStream(el_,uid,stream_name,audio,video,log_id);

    return 0;
}



}  // namespace lrtc


