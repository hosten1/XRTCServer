#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_SINGNALING_SERVER_RTCTHREAT_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_SINGNALING_SERVER_RTCTHREAT_H_

#include <memory>

#include <p2p/base/basic_packet_socket_factory.h>
#include <rtc_base/async_packet_socket.h>
#include <rtc_base/network/sent_packet.h>
#include <rtc_base/thread.h>
#include <api/scoped_refptr.h>
#include "server/signaling_server_options.h"

namespace lrtc
{
    class SignalingServerRtcThread : public sigslot::has_slots<>
    {

    public:
        SignalingServerRtcThread();
        ~SignalingServerRtcThread();
        int init(const char *conf_file);
        bool start();
        int stop();
        // Accepts incoming TCP connection.
      void OnNewConnection(rtc::AsyncPacketSocket* socket,
                       rtc::AsyncPacketSocket* new_socket);

    protected:
       /// @brief 
       /// @param socket 
       /// @param address 
       void OnAddressReady(rtc::AsyncPacketSocket* socket,
                      const rtc::SocketAddress& address);
        // // Called when a TCP connection is established or fails
        // void OnSocketConnect(rtc::AsyncPacketSocket *socket);
        // void OnSocketClose(rtc::AsyncPacketSocket *socket, int error);

        // // Called when a packet is received on this socket.
        void OnReadPacket(rtc::AsyncPacketSocket *socket,
                          const char *data,
                          size_t size,
                          const rtc::SocketAddress &remote_addr,
                          const int64_t &packet_time_us);

        // void OnSentPacket(rtc::AsyncPacketSocket *socket,
        //                   const rtc::SentPacket &sent_packet);

        // // Called when the socket is currently able to send.
        // void OnReadyToSend(rtc::AsyncPacketSocket *socket);

    private:
        SignalingServerOptions options_;
        std::unique_ptr<rtc::BasicPacketSocketFactory> default_socket_factory_;   
        std::unique_ptr<rtc::AsyncPacketSocket> async_socket_;
        rtc::AsyncPacketSocket* async_socket_client_;
        std::unique_ptr<rtc::Thread> network_thread_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_SINGNALING_SERVER_RTCTHREAT_H_
