/*
 * @Author: L yongmeng
 * @Date: 2024-04-06 16:47:12
 * @LastEditTime: 2024-04-16 17:05:46
 * @LastEditors: L yongmeng
 * @Description:
 * Software:VSCode,env:
 */
#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_H_

#include <memory>
#include <string>
#include <thread>
#include <queue>
#include <mutex>

#include <rtc_base/rtc_certificate.h>

#include "base/event_loop.h"
#include "base/lrtc_server_def.h"
#include "json/json.h"
#include "server/rtc_worker.h"
#include "server/rtc_server_options.h"

namespace lrtc
{

    class RtcServer
    {
    public:
        enum
        {
            MSG_QUIT = 0,
            MSG_RTC_MSG = 1,
        };

        RtcServer();
        ~RtcServer();
        int init(const char *conf_file);
        bool start();
        int stop();
        void joined();

        int send_rtc_msg(const std::shared_ptr<LRtcMsg> rtc_msg);
        void push_msg(std::shared_ptr<LRtcMsg> rtc_msg);
        const std::shared_ptr<LRtcMsg> pop_msg();

        friend void rtc_server_recv_notify(EventLoop * /*el*/, IOWatcher * /*w*/,
                                           int fd, int events, void *data);

    private:
        int _notify(int msg);
        void _process_notify(int msg);
        void _stop();
        void _process_rtc_msg();
        int _create_worker(int work_id);
        RtcWorker *_get_worker(const std::string stream_name);
        int _generate_and_check_certificate();

    private:
        std::unique_ptr<EventLoop> event_loop_{nullptr};
        rtcServerOptions options_;
        IOWatcher *pipe_watcher_{nullptr};
        std::unique_ptr<std::thread> ev_thread_;

        std::queue<std::shared_ptr<LRtcMsg>> q_msgs_;
        std::mutex q_thread_mutex_;

        int notify_recv_fd_ = -1;
        int notify_send_fd_ = -1;

        std::vector<std::unique_ptr<RtcWorker>> workers_;
        size_t next_works_index_ = 0;

        rtc::scoped_refptr<rtc::RTCCertificate> certificate_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_H_
