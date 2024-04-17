/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file network.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 18:34:40, 四月 16, 2024
 */

#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_NETWORK_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_NETWORK_H_

#include <string>
#include <vector>

#include <rtc_base/ip_address.h>
namespace lrtc
{
    class Network
    {
    public:
        Network(const std::string &name, const rtc::IPAddress &ip) : name_(name), ip_(ip) {}
        ~Network() = default;

    public:
        const std::string &name()
        {
            return name_;
        }

        const rtc::IPAddress &ip()
        {
            return ip_;
        }

        std::string to_string()
        {
            return name_ + ":" + ip_.ToString();
        }

    private:
        std::string name_;
        rtc::IPAddress ip_;
    };

    class NetworkManager
    {
    public:
        NetworkManager() = default;
        ~NetworkManager();

        const std::vector<Network *> &get_networks() { return network_list_; }
        int create_networks();

    private:
        std::vector<Network *> network_list_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_NETWORK_H_
