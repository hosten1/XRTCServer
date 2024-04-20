/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file ice_connection_info.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 12:14:06, 四月 20, 2024
 */

#ifndef __LYMSDK_LRTCSERVER_SRC_ICE_ICE_CONNECTION_INFO_H_
#define __LYMSDK_LRTCSERVER_SRC_ICE_ICE_CONNECTION_INFO_H_

namespace lrtc
{
    enum class IceCandidatePairState
    {
        WAITING,     // 连通性检查尚未开始
        IN_PROGRESS, // 检查进行中
        SUCCEEDED,   // 检查成功
        FAILED,      // 检查失败
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_ICE_ICE_CONNECTION_INFO_H_
