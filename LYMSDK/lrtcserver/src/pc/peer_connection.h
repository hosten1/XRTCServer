#ifndef __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_

#include <string>
#include <stdint.h>

#include "base/event_loop.h"

namespace lrtc
{
    class PeerConnection
    {

    public:
        PeerConnection(EventLoop *ev);
        ~PeerConnection();
    private:
        EventLoop *ev_;
    };
    

    
    
} // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_H_
