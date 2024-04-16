#ifndef __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_

#include <string>
#include <stdint.h>
#include <memory>

#include "base/event_loop.h"
#include "pc/session_description.h"

namespace lrtc
{
    class PeerConnection
    {

    public:
        PeerConnection(EventLoop *el);
        ~PeerConnection();

        std::string create_offer_sdp();
    private:
        EventLoop *el_;
        std::unique_ptr<SessionDescription> local_session_description_;
    };
    

    
    
} // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_
