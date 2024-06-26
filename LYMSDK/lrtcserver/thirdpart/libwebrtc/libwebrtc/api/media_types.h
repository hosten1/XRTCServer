/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_MEDIA_TYPES_H_
#define API_MEDIA_TYPES_H_

#include <string>

#include "rtc_base/system/rtc_export.h"


// The cricket and webrtc have separate definitions for what a media type is.
// They're not compatible. Watch out for this.

namespace cricket {

// lym
static const char kAudioKind[] = "audio";
static const char kVideoKind[] = "video";
//lym

enum MediaType { MEDIA_TYPE_AUDIO, MEDIA_TYPE_VIDEO, MEDIA_TYPE_DATA };

RTC_EXPORT std::string MediaTypeToString(MediaType type);
// Aborts on invalid string. Only expected to be used on strings that are
// guaranteed to be valid, such as MediaStreamTrackInterface::kind().
MediaType MediaTypeFromString(const std::string& type_str);

}  // namespace cricket

namespace webrtc {

enum class MediaType { ANY, AUDIO, VIDEO, DATA };

}  // namespace webrtc

#endif  // API_MEDIA_TYPES_H_
