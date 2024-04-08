#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_

// #define USE_SDS

#include <string>
#include <functional>
#include <list>
#include "base/event_loop.h"
#include "base/lheader.h"
#include "rtc_base/byte_buffer.h"
//第三方库 jsoncpp
#include "json/json.h"
#include "base/lrtc_server_def.h"


#ifdef USE_SDS
extern "C"
#include <rtc_base/zalloc.h>
{
#include "rtc_base/sds/sds.h"
#include "rtc_base/sds/slice.h"
}
#endif

namespace lrtc
{
    class TcpConnection
    {
    public:
        enum
        {
            STATE_HEAD = 0,
            STATE_BODY = 1,
            STATE_DONE
        };

        TcpConnection(int fd, const char *ip, int port);
        TcpConnection(int fd);
        ~TcpConnection();

        int read(int fd, std::function<void( Json::Value, uint32_t)> callback);
        int send(const char *buf, int len);
        int close_conn();

        void writerHeaderDataToBuffer(const lheader_t& header,rtc::ByteBufferWriter &writer);

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
         
        lheader_t*  req_header()
         {
            return &req_header_;
         }

        IOWatcher *io_watcher_ = nullptr;
        TimerWatcher *timer_watcher_ = nullptr;

#ifdef USE_SDS
       
        std::list<rtc::Slice> reply_list;
#else
        std::list<std::unique_ptr<rtc::ByteBufferWriter>> reply_list;
#endif
// 记录写入的位置i
       size_t cur_resp_pos = 0;

    private:
        int _recv(char *buf, int len, std::function<void( Json::Value, uint32_t)> callback);

        bool _parseDataIntoLHeader(const char *data, size_t data_size, lheader_t &header);
        bool _parseDataOfBodyJson(const char *data, size_t data_size,size_t  body_len,std::string &body);

    private:
        int fd_;
        char ip_[64];
        int port_;

        size_t bytes_processed_ = 0;
        int current_state_ = STATE_HEAD;
#ifdef USE_SDS
        sds queryBuf_;
#else
       rtc::BufferT<char> queryBuf_;
#endif
        size_t bytes_expected_ = L_HEADER_SIZE;

        uint32_t last_interaction_time_ = 0;

        int recv_len_;
        int send_len_;
        int recv_buf_len_;
        char *recv_buf_;
        char *send_buf_;
        lheader_t  req_header_;
    };

}; // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_TCP_CONNECTION_H_
