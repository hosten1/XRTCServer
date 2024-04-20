/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file ice_credentials.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 16:11:01, 四月 16, 2024
 */

#ifndef __LYMSDK_LRTCSERVER_SRC_ICE_ICE_CREDENTIALS_H_
#define __LYMSDK_LRTCSERVER_SRC_ICE_ICE_CREDENTIALS_H_

#include <string>

namespace lrtc
{
    struct IceParameters
    {
        IceParameters() = default;
        IceParameters(const std::string &ufrag, const std::string &pwd) : ice_ufrag(ufrag), ice_pwd(pwd) {}

        std::string ice_ufrag;
        std::string ice_pwd;

    public:
        std::string to_string() const
        {
            return "ice_ufrag:" + ice_ufrag + "ice_pwd:" + ice_pwd;
        };
    };

    class IceCredentials
    {
    public:
        static IceParameters create_random_ice_credentials();
    };
} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_ICE_ICE_CREDENTIALS_H_
