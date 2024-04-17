/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file candidate.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 18:07:57, 四月 16, 2024
 */
#ifndef __LYMSDK_LRTCSERVER_SRC_ICE_CANDIDATE_H_
#define __LYMSDK_LRTCSERVER_SRC_ICE_CANDIDATE_H_

#include <string>

#include <rtc_base/socket_address.h>

#include "ice/ice_def.h"

namespace lrtc
{
    class Candidate
    {
    public:
        uint32_t get_priority(uint32_t type_preference,
                              int network_adapter_preference,
                              int relay_preference);

        std::string to_string() const;

    public:
        IceCandidateComponent component;
        std::string protocol;
        rtc::SocketAddress address;
        int port = 0;
        uint32_t priority;
        std::string username;
        std::string password;
        std::string type;
        std::string foundation;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_ICE_CANDIDATE_H_
