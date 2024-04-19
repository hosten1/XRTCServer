#ifndef __LYMSDK_LRTCSERVER_SRC_PC_STREAM_PARAMS_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_STREAM_PARAMS_H_

#include <vector>
#include <string>
#include <memory>

namespace lrtc
{

    struct SsrcGroup
    {
        SsrcGroup(){};
        ~SsrcGroup(){};
        SsrcGroup(const std::string &semantics, const std::vector<uint32_t> &ssrcs);

        std::string semantics;
        std::vector<uint32_t> ssrcs;
    };

    struct StreamParams
    {
        StreamParams(){};
        ~StreamParams(){};
        bool has_ssrc(uint32_t ssrc);

        std::string id;
        std::vector<uint32_t> ssrcs;
        std::vector<std::shared_ptr<SsrcGroup>> ssrc_groups;
        std::string cname;
        std::string stream_id;

    public:
        std::string to_string() const
        {
            std::string result = "id:" + id;
            if (!ssrcs.empty())
            {
                result += " ssrcs:" + std::to_string(ssrcs[0]);
            }
            result += " cname:" + cname + " stream_id:" + stream_id;
            return result;
        }
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_PC_STREAM_PARAMS_H_
