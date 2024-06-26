#ifndef __LYMSDK_LRTCSERVER_SRC_PC_STREAM_PARAMS_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_STREAM_PARAMS_H_

#include <vector>
#include <string>

namespace lrtc
{

    struct SsrcGroup
    {
        SsrcGroup(const std::string &semantics, const std::vector<uint32_t> &ssrcs);

        std::string semantics;
        std::vector<uint32_t> ssrcs;
    };

    struct StreamParams
    {
        bool has_ssrc(uint32_t ssrc);

        std::string id;
        std::vector<uint32_t> ssrcs;
        std::vector<SsrcGroup> ssrc_groups;
        std::string cname;
        std::string stream_id;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_PC_STREAM_PARAMS_H_
