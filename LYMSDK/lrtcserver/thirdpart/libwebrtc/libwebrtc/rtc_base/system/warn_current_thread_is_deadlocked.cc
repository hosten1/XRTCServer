/*
 *  Copyright 2019 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "rtc_base/system/warn_current_thread_is_deadlocked.h"

#include "rtc_base/logging.h"
#if defined(WEBRTC_ANDROID) && !defined(WEBRTC_CHROMIUM_BUILD)
#include "sdk/android/native_api/stacktrace/stacktrace.h"
#else

#endif

namespace webrtc {
  
#if defined(WEBRTC_ANDROID) && !defined(WEBRTC_CHROMIUM_BUILD)
void WarnThatTheCurrentThreadIsProbablyDeadlocked() {
  RTC_LOG(LS_WARNING) << "Probable deadlock:";
  RTC_LOG(LS_WARNING) << StackTraceToString(GetStackTrace());
}
#endif

}  // namespace webrtc
