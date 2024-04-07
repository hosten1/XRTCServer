#ifndef __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_OPTIONS_H_
#define __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_OPTIONS_H_

namespace lrtc
{
    struct rtcServerOptions
    {
        int worker_num;

    public:
        rtcServerOptions() : worker_num(1) {}
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_SERVER_RTC_SERVER_OPTIONS_H_
