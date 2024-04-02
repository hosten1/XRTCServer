cmake_minimum_required(VERSION 3.5)
project(libwebrtc)

add_definitions("-g -pipe -W -Wall -fPIC")

set(CMAKE_CXX_STANDARD 14)
# 添加源文件
set(SOURCES
    # C++ source files.
    "libwebrtc/system_wrappers/source/field_trial.cc"
    "libwebrtc/system_wrappers/source/sleep.cc"
    "libwebrtc/system_wrappers/source/clock.cc"
    "libwebrtc/system_wrappers/source/cpu_features.cc"
    "libwebrtc/system_wrappers/source/metrics.cc"
    "libwebrtc/system_wrappers/source/rtp_to_ntp_estimator.cc"

    "libwebrtc/rtc_base/rate_statistics.cc"
    "libwebrtc/rtc_base/experiments/field_trial_parser.cc"
    "libwebrtc/rtc_base/experiments/alr_experiment.cc"
    "libwebrtc/rtc_base/experiments/field_trial_units.cc"
    "libwebrtc/rtc_base/experiments/rate_control_settings.cc"
    "libwebrtc/rtc_base/network/sent_packet.cc"
    "libwebrtc/rtc_base/synchronization/rw_lock_wrapper.cc"
    "libwebrtc/rtc_base/synchronization/rw_lock_posix.cc"
    # "libwebrtc/rtc_base/synchronization/rw_lock_win.cc"
    "libwebrtc/rtc_base/critical_section.cc"
    "libwebrtc/rtc_base/platform_thread_types.cc"
    "libwebrtc/rtc_base/bit_buffer.cc"
    "libwebrtc/rtc_base/byte_buffer.cc"
    "libwebrtc/rtc_base/checks.cc"
    "libwebrtc/rtc_base/logging.cc"
    "libwebrtc/rtc_base/copy_on_write_buffer.cc"
    "libwebrtc/rtc_base/cpu_time.cc"
    "libwebrtc/rtc_base/crc32.cc"
    "libwebrtc/rtc_base/crypt_string.cc"
    "libwebrtc/rtc_base/data_rate_limiter.cc"
    "libwebrtc/rtc_base/flags.cc"
    "libwebrtc/rtc_base/random.cc"
    "libwebrtc/rtc_base/string_encode.cc"
    "libwebrtc/rtc_base/string_to_number.cc"
    "libwebrtc/rtc_base/string_utils.cc"
    "libwebrtc/rtc_base/string_view.cc"
    "libwebrtc/rtc_base/time_utils.cc"
    "libwebrtc/rtc_base/zero_memory.cc"
    "libwebrtc/rtc_base/strings/string_builder.cc"
    "libwebrtc/rtc_base/synchronization/sequence_checker.cc"
    "libwebrtc/rtc_base/strings/audio_format_to_string.cc"


    "libwebrtc/api/transport/bitrate_settings.cc"
    "libwebrtc/api/transport/field_trial_based_config.cc"
    "libwebrtc/api/transport/network_types.cc"
    "libwebrtc/api/units/timestamp.cc"
    "libwebrtc/api/units/time_delta.cc"
    "libwebrtc/api/units/data_rate.cc"
    "libwebrtc/api/units/data_size.cc"
    "libwebrtc/api/units/frequency.cc"
    "libwebrtc/api/network_state_predictor.cc"
    "libwebrtc/api/media_types.cc"
    "libwebrtc/api/rtp_headers.cc"
    "libwebrtc/api/rtp_parameters.cc"
    "libwebrtc/api/audio_codecs/audio_decoder.cc"
    "libwebrtc/api/audio_codecs/audio_format.cc"
    "libwebrtc/api/audio_codecs/audio_codec_pair_id.cc"
    "libwebrtc/api/video/color_space.cc"
    "libwebrtc/api/video/hdr_metadata.cc"
    "libwebrtc/api/video/video_bitrate_allocation.cc"
    "libwebrtc/api/video/video_content_type.cc"
    "libwebrtc/api/video/video_timing.cc"
    "libwebrtc/api/rtp_packet_info.cc"
)

# 添加头文件路径
include_directories(
    "libwebrtc"
    "./deps/abseil-cpp"
    "./json/single_include/nlohmann"
)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

# 添加条件定义
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    MESSAGE(STATUS "current platform: Darwin ")
    add_definitions(-DWEBRTC_MAC)
    add_definitions(-DWEBRTC_POSIX)
    #add_definitions(-DNDEBUG)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
    add_definitions(-DWEBRTC_LINUX)
    add_definitions(-DWEBRTC_POSIX)
    add_definitions(-DUSE_MEDIASOUP_ClASS)
    #add_definitions(-DNDEBUG)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows ")
    add_definitions(-DWEBRTC_WIN)
    #add_definitions(-DNDEBUG)
endif()

include(TestBigEndian)
test_big_endian(WORDS_BIGENDIAN)
if(WORDS_BIGENDIAN)
    add_definitions(-DMS_BIG_ENDIAN)
else()
    add_definitions(-DMS_LITTLE_ENDIAN)
endif()

# 链接库路径
link_directories(
    "./out/deps/abseil-cpp/absl/strings"
    "./out/deps/abseil-cpp/absl/base"
    "./out/deps/abseil-cpp/absl/container"
    # "./out/deps/abseil-cpp/absl/container"
    )

# add_subdirectory("./deps/abseil-cpp")
# add_subdirectory("./deps/abseil-cpp")

# 添加依赖库
add_library(libwebrtc STATIC ${SOURCES})

# 查找 pthread 库
# find_package(Threads REQUIRED)
target_link_libraries(libwebrtc 
                    pthread
                    absl_strings
                    absl_strerror
)
