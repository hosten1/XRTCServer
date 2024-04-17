/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file async_udp_socket.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 10:37:47, 四月 17, 2024
 */

#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_ASYNC_UDP_SOCKET_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_ASYNC_UDP_SOCKET_H_

#include <list>

#include <rtc_base/third_party/sigslot/sigslot.h>
#include <rtc_base/socket_address.h>

#include "base/event_loop.h"

namespace lrtc
{
    class UdpPacketData
    {
    public:
        UdpPacketData(const char *data, size_t size, const rtc::SocketAddress &addr) : data_(new char[size]),
                                                                                       size_(size),
                                                                                       addr_(addr)
        {
            memcpy(data_, data, size);
        }

        ~UdpPacketData()
        {
            if (data_)
            {
                delete[] data_;
                data_ = nullptr;
            }
        }

        char *data() { return data_; }
        size_t size() { return size_; }
        rtc::SocketAddress addr() { return addr_; }

    private:
        char *data_;
        size_t size_;
        rtc::SocketAddress addr_;
    };

    class AsyncUdpSocket
    {
    public:
        AsyncUdpSocket(EventLoop *el, int socket);
        ~AsyncUdpSocket();

        void send_data();
        void recv_data();

        int send_to(const char *data, size_t size, const rtc::SocketAddress &addr);

        sigslot::signal5<AsyncUdpSocket *, char *, size_t, const rtc::SocketAddress &, int64_t>
            signal_read_packet;

    private:
        int _add_udp_packet(const char *data, size_t size, const rtc::SocketAddress &addr);

    private:
        EventLoop *el_ = nullptr;
        int socket_ = 0;
        IOWatcher *socket_watcher_ = nullptr;
        char *buf_ = nullptr;
        size_t size_ = 0;

        std::list<UdpPacketData *> udp_packet_list_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_ASYNC_UDP_SOCKET_H_
