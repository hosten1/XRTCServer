#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_

#include <string>
#include "base/event_loop.h"
#include "base/lheader.h"
#include "rtc_base/byte_buffer.h"

// #define USE_SDS
#ifdef USE_SDS
extern "C"
{
#include "rtc_base/sds/sds.h"
}
#endif

namespace lrtc
{
    class TcpConnection
    {
    public:
#ifdef USE_SDS
        enum
        {
            STATE_HEAD = 0,
            STATE_BODY = 1,
            STATE_DONE
        };
#endif // USE_SDS

        TcpConnection(int fd, const char *ip, int port);
        TcpConnection(int fd);
        ~TcpConnection();

        int read(int fd);
        int send(const char *buf, int len);
        int recv(char *buf, int len);
        int close_conn();

        int get_fd() const { return fd_; }
        const char *get_ip() const { return ip_; }
        int get_port() const { return port_; }

        void set_last_interaction_time(uint32_t time)
        {
            last_interaction_time_ = time;
        }
        uint32_t last_interaction_time()
        {
            return last_interaction_time_;
        }

        IOWatcher *io_watcher_ = nullptr;
        TimerWatcher *timer_watcher_ = nullptr;

#ifdef USE_SDS
        int current_state_ = STATE_HEAD;
#endif

    private:
        bool _parseDataIntoLHeader(const char *data, size_t data_size, lheader_t &header, std::string &body);

    private:
        int fd_;
        char ip_[64];
        int port_;

        size_t bytes_processed_ = 0;
#ifdef USE_SDS
        sds queryBuf_;
#endif
        size_t bytes_expected_ = L_HEADER_SIZE;

        uint32_t last_interaction_time_ = 0;

        int recv_len_;
        int send_len_;
        int recv_buf_len_;
        char *recv_buf_;
        char *send_buf_;
    };

}; // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_
