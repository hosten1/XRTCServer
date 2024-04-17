/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file utils.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 10:55:37, 四月 17, 2024
 */
#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_UTILS_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_UTILS_H_

namespace lrtc
{
    template <typename T>
    class Singleton
    {
    public:
        static T *Instance()
        {
            static T instance;
            return &instance;
        }
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_UTILS_H_
