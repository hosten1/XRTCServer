
#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_LRTC_SERVER_DEF_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_LRTC_SERVER_DEF_H_

#define CMDNUM_PUSH        1
#define CMDNUM_PULL        2
#define CMDNUM_ANSWER      3
#define CMDNUM_STOP_PUSH   4
#define CMDNUM_STOP_PULL   5

#define MAX_RESP_BUFEER_SIZE 4096

namespace lrtc {

 struct LRtcMsg
 {
    int cmdno = 0;
    uint64_t uid = 0;
    std::string stream_name = "";
    int audio = 0;
    int video = 0;
    std::string log_id = "";
    void *signalingWorker = nullptr;
    int signalingWorkerId = 0;
    void *signalingConn = nullptr;
    std::string sdp;
    int err_no = 0;
    int fd = 0;

    public:
        LRtcMsg() = default;
        ~LRtcMsg()=default;
        std::string toString()
        {
            return "cmdno:" + std::to_string(cmdno) +
                   " uid:" + std::to_string(uid) +
                   " stream_name:" + stream_name +
                   " audio:" + std::to_string(audio) +
                   " video:" + std::to_string(video) +
                   " log_id:" + log_id;
        }
 };
 
}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_BASE_LRTC_SERVER_DEF_H_
