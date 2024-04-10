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

    "libwebrtc/rtc_base/experiments/field_trial_parser.cc"
    "libwebrtc/rtc_base/experiments/alr_experiment.cc"
    "libwebrtc/rtc_base/experiments/field_trial_units.cc"
    "libwebrtc/rtc_base/experiments/rate_control_settings.cc"
    "libwebrtc/rtc_base/network/sent_packet.cc"

    "libwebrtc/rtc_base/async_resolver_interface.cc"
    "libwebrtc/rtc_base/async_socket.cc"
    "libwebrtc/rtc_base/async_packet_socket.cc"
    "libwebrtc/rtc_base/async_tcp_socket.cc"
    "libwebrtc/rtc_base/async_udp_socket.cc"
    "libwebrtc/rtc_base/bit_buffer.cc"
    "libwebrtc/rtc_base/byte_buffer.cc"
    "libwebrtc/rtc_base/checks.cc"
    "libwebrtc/rtc_base/checks1.cc"
    "libwebrtc/rtc_base/copy_on_write_buffer.cc"
    "libwebrtc/rtc_base/cpu_time.cc"
    "libwebrtc/rtc_base/crc32.cc"
    "libwebrtc/rtc_base/crypt_string.cc"
    "libwebrtc/rtc_base/critical_section.cc"
    "libwebrtc/rtc_base/data_rate_limiter.cc"
    "libwebrtc/rtc_base/event.cc"
    "libwebrtc/rtc_base/flags.cc"
    "libwebrtc/rtc_base/http_common.cc"
    "libwebrtc/rtc_base/helpers.cc"
    "libwebrtc/rtc_base/ifaddrs_converter.cc"
    "libwebrtc/rtc_base/ip_address.cc"
    "libwebrtc/rtc_base/location.cc"
    "libwebrtc/rtc_base/logging.cc"
    "libwebrtc/rtc_base/message_digest.cc"
    "libwebrtc/rtc_base/message_handler.cc"
    "libwebrtc/rtc_base/message_queue.cc"
    "libwebrtc/rtc_base/net_helper.cc"
    "libwebrtc/rtc_base/net_helpers.cc"
    "libwebrtc/rtc_base/network.cc"
    "libwebrtc/rtc_base/network_monitor.cc"
    "libwebrtc/rtc_base/null_socket_server.cc"
    "libwebrtc/rtc_base/openssl_digest.cc"
    "libwebrtc/rtc_base/openssl_adapter.cc"
    "libwebrtc/rtc_base/openssl_certificate.cc"
    "libwebrtc/rtc_base/openssl_identity.cc"
    "libwebrtc/rtc_base/openssl_session_cache.cc"
    "libwebrtc/rtc_base/openssl_stream_adapter.cc"
    "libwebrtc/rtc_base/openssl_utility.cc"
    "libwebrtc/rtc_base/openssl_utility.cc"
    "libwebrtc/rtc_base/platform_thread_types.cc"
    "libwebrtc/rtc_base/platform_thread.cc"
    "libwebrtc/rtc_base/proxy_info.cc"
    "libwebrtc/rtc_base/physical_socket_server.cc"
    "libwebrtc/rtc_base/rate_statistics.cc"
    "libwebrtc/rtc_base/rtc_certificate.cc"
    "libwebrtc/rtc_base/rtc_certificate_generator.cc"
    "libwebrtc/rtc_base/random.cc"
    "libwebrtc/rtc_base/stream.cc"
    "libwebrtc/rtc_base/string_encode.cc"
    "libwebrtc/rtc_base/string_to_number.cc"
    "libwebrtc/rtc_base/string_utils.cc"
#   这个是absl下的
    "libwebrtc/rtc_base/signal_thread.cc"
    "libwebrtc/rtc_base/socket.cc"
    "libwebrtc/rtc_base/socket_stream.cc"
    "libwebrtc/rtc_base/socket_address.cc"
    "libwebrtc/rtc_base/socket_adapters.cc"
    "libwebrtc/rtc_base/ssl_adapter.cc"
    "libwebrtc/rtc_base/ssl_certificate.cc"
    "libwebrtc/rtc_base/ssl_fingerprint.cc"
    "libwebrtc/rtc_base/ssl_identity.cc"
    "libwebrtc/rtc_base/ssl_stream_adapter.cc"
    "libwebrtc/rtc_base/socket_address_pair.cc"
    "libwebrtc/rtc_base/socket_address_pair.cc"
    "libwebrtc/rtc_base/time_utils.cc"
    "libwebrtc/rtc_base/zero_memory.cc"
    "libwebrtc/rtc_base/thread.cc"
    "libwebrtc/rtc_base/async_invoker.cc"
    "libwebrtc/rtc_base/strings/string_builder.cc"
    "libwebrtc/rtc_base/synchronization/rw_lock_wrapper.cc"
    "libwebrtc/rtc_base/synchronization/rw_lock_posix.cc"
    # "libwebrtc/rtc_base/synchronization/rw_lock_win.cc"
    "libwebrtc/rtc_base/synchronization/sequence_checker.cc"
    "libwebrtc/rtc_base/synchronization/yield_policy.cc"
    "libwebrtc/rtc_base/strings/audio_format_to_string.cc"
    "libwebrtc/rtc_base/third_party/base64/base64.cc"
    "libwebrtc/rtc_base/third_party/sigslot/sigslot.cc"

    #  这个是redis的库
    "libwebrtc/rtc_base/sds/sds.cc"
    "libwebrtc/rtc_base/sds/zmalloc.cc"    


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
    "libwebrtc/p2p/base/async_stun_tcp_socket.cc"
    "libwebrtc/p2p/base/basic_async_resolver_factory.cc"
    "libwebrtc/p2p/base/basic_packet_socket_factory.cc"
    "libwebrtc/p2p/base/packet_socket_factory.cc"
    "libwebrtc/p2p/base/stun.cc"

)

# 添加头文件路径
include_directories(
    "libwebrtc"
    "../abseil-cpp"
    "./json/single_include/nlohmann"
    "./deps/openssl/install/include"
)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")
if(ANDROID)
    MESSAGE(STATUS "current platform: Android")
    add_definitions(-DWEBRTC_ANDROID)
    # 向 SOURCES 变量追加新的源文件
     list(APPEND SOURCES 
            "libwebrtc/rtc_base/strings/ifaddrs_android.cc"
         
          )
else()
    # 添加条件定义
    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        MESSAGE(STATUS "current platform: Darwin ")
        add_definitions(-DWEBRTC_MAC)
        add_definitions(-DWEBRTC_POSIX)
        #add_definitions(-DNDEBUG)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        MESSAGE(STATUS "current platform: Linux ")
        # add_definitions(-DWEBRTC_LINUX)
        # add_definitions(-DWEBRTC_POSIX)
        add_definitions(-DUSE_MEDIASOUP_ClASS)
        add_definitions("-g -pipe -W -Wall -fPIC -std=gnu++14 -DWEBRTC_POSIX
    -DWEBRTC_LINUX -DNDEBUG -Wno-unused-parameter -DWEBRTC_APM_DEBUG_DUMP=0 -mfma -DWEBRTC_EXTERNAL_JSON=0
    ")
        #add_definitions(-DNDEBUG)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
        MESSAGE(STATUS "current platform: Windows ")
        add_definitions(-DUNICODE -D_UNICODE -DWIN32 -D_WIN32 -DWIN32_LEAN_AND_MEAN 
        -DNOMINMAX 
        -D_WINDOWS
        -D_USE_MATH_DEFINES
        -DWEBRTC_WIN
        -DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
        -DWEBRTC_EXTERNAL_JSON
        -DWEBRTC_APM_DEBUG_DUMP)
        #add_definitions(-DNDEBUG)
    endif()

    include(TestBigEndian)
    test_big_endian(WORDS_BIGENDIAN)
    if(WORDS_BIGENDIAN)
        add_definitions(-DMS_BIG_ENDIAN)
    else()
        add_definitions(-DMS_LITTLE_ENDIAN)
    endif()
endif()

# 链接库路径
link_directories(
    "./out/deps/abseil-cpp/absl/strings"
    "./out/deps/abseil-cpp/absl/base"
    "./out/deps/abseil-cpp/absl/container"
    "./out/deps/abseil-cpp/absl/container"
    "./deps/openssl/install/lib"
    )

# add_subdirectory("./deps/abseil-cpp")
# add_subdirectory("./deps/abseil-cpp")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_library(libwebrtc STATIC
        ${SOURCES}
        ${rtc_base_win}
        ${api_win}
        ${audio_device_win}
        ${video_capture_win}

    install(TARGETS libwebrtc ARCHIVE DESTINATION lib)
)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    file(GLOB exclude_src
        "./src/rtc_base/win32.cc"
        "./src/rtc_base/task_queue_win.cc"
        "./src/api/task_queue/default_task_queue_factory_win.cc"
        "./src/rtc_base/system/warn_current_thread_is_deadlocked.cc"
    )
    list(REMOVE_ITEM all_src ${exclude_src})
    add_library(libwebrtc 
        ${SOURCES}
    )
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)  # 设置安装目录
install(TARGETS libwebrtc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/libwebrtc/src/rtc_base 
    ${PROJECT_SOURCE_DIR}/libwebrt/src/api
    ${PROJECT_SOURCE_DIR}/libwebrt/src/p2p/base
    ${PROJECT_SOURCE_DIR}/libwebrt/src/system_wrappers
    ${PROJECT_SOURCE_DIR}/libwebrt/src/modules
    ${PROJECT_SOURCE_DIR}/libwebrt/src/common_audio
    ${PROJECT_SOURCE_DIR}/libwebrt/src/common_video
    ${PROJECT_SOURCE_DIR}/libwebrt/src/media
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
)

# 查找 pthread 库
# find_package(Threads REQUIRED)
target_link_libraries(libwebrtc  pthread
                    libabsl_strings.a 
                    libabsl_throw_delegate.a
                    libabsl_strerror.a
                    libabsl_bad_optional_access.a
                    libssl.a
                    libcrypto.a
)