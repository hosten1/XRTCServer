/*
 * @brief
 */

#include <stdint.h>
#include <sstream>

#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_LHEADER_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_LHEADER_H_

namespace lrtc
{
    const int L_HEADER_SIZE = 36;
    const uint32_t L_HEADER_MAGIC_NUMBER = 0xfb202404;

    struct lheader_t
    {
        uint16_t id;
        uint16_t version;
        uint32_t log_id;
        char provider[16];
        uint32_t magic_num;
        uint32_t reserved;
        uint32_t body_len;

    public:
        std::string toString()
        {
            std::stringstream ss;
            ss << "id: " << id
               << ", version: " << version
               << ", log_id: " << log_id
               << ", provider: " << std::string(provider, sizeof(provider))
               << ", magic_num: 0x" << std::hex << magic_num // Output in hexadecimal format
               << ", reserved: " << reserved;
            return ss.str();
        }
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_LHEADER_H_
