
#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_LRTC_SERVER_DEF_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_LRTC_SERVER_DEF_H_

#define CMDNUM_PUSH        1
#define CMDNUM_PULL        2
#define CMDNUM_ANSWER      3
#define CMDNUM_STOP_PUSH   4
#define CMDNUM_STOP_PULL   5

namespace lrtc {

 struct LRtcMsg
 {
    int cmdno = 0;
    uint64_t uid = 0;
    std::string stream_name = "";
    int audio = 0;
    int video = 0;
    public:
        LRtcMsg() = default;
        ~LRtcMsg()=default;
        std::string toString()
        {
            return "cmdno:" + std::to_string(cmdno) +
                   " uid:" + std::to_string(uid) +
                   " stream_name:" + stream_name +
                   " audio:" + std::to_string(audio) +
                   " video:" + std::to_string(video);
        }
 };
 
}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_BASE_LRTC_SERVER_DEF_H_
