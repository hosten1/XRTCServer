/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file peer_connection_def.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 08:35:35, 四月 17, 2024
 */
#ifndef __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_DEF_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_DEF_H_

namespace lrtc
{
    enum class PeerConnectionState
    {
        k_new = 0,
        k_connecting,
        k_connected,
        k_disconnected,
        k_failed,
        k_closed,
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_PC_PEER_CONNECTION_DEF_H_
