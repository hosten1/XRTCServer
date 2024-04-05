/*
 * @brief
 */


#include <stdint.h>

#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_LHEADER_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_LHEADER_H_

namespace lrtc
{
    const int L_HEADER_SIZE = 36;
    const uint32_t L_HEADER_MAGIC_NUMBER  = 0xfb202404;

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
            return "id:" + std::to_string(id) +
                   ", version:" + std::to_string(version) +
                   ", log_id:" + std::to_string(log_id) +
                   ", provider:" + std::string(provider, sizeof(provider)) +
                   ", magic_num:" + std::to_string(magic_num) +
                   ", reserved:" + std::to_string(reserved);
        }
    };

} // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_BASE_LHEADER_H_
