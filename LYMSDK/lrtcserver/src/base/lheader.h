/*
 * @brief
 */

#include <stdint.h>
#include <cstring> // 用于std::strlen
#include <string>
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
        lheader_t()
        {
            id = 0;
            version = 0;
            log_id = 0;
            memset(provider, 0, 16);
            magic_num = 0;
            reserved = 0;
            body_len = 0;
        }
        std::string toString()
        {
           std::string magicStr = getMagicNumFormatted();
                return "id: " + std::to_string(id) +
                       ", version: " + std::to_string(version) +
                       ", log_id: " + std::to_string(log_id) +
                       ", provider: " + std::string(provider) +
                       ", magic_num: " + magicStr +
                       ", reserved: " + std::to_string(reserved) +
                       ", body_len: " + std::to_string(body_len);
        }
        std::string getMagicNumFormatted() const
        {
            std::ostringstream oss;
            oss << "0x" << std::hex << magic_num;
            return oss.str();
        }
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_LHEADER_H_
