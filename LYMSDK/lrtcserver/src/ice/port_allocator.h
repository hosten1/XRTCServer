/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file port_allocator.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 18:31:59, 四月 16, 2024
 */
#ifndef __LYMSDK_LRTCSERVER_SRC_ICE_PORT_ALLOCATOR_H_
#define __LYMSDK_LRTCSERVER_SRC_ICE_PORT_ALLOCATOR_H_
#include <memory>

#include "base/network.h"
namespace lrtc
{
    class PortAllocator
    {
    public:
        PortAllocator();
        ~PortAllocator() = default;

        const std::vector<Network *> &get_networks();

        void set_port_range(int min_port, int max_port);

        int min_port() const
        {
            return min_port_;
        }

        int max_port() const
        {
            return max_port_;
        }

    private:
        std::unique_ptr<NetworkManager> network_manager_;
        int min_port_ = 0;
        int max_port_ = 0;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_ICE_PORT_ALLOCATOR_H_
