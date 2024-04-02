{
  'target_defaults': {
    'dependencies':
    [
      '../jsoncpp/jsoncpp.gyp:jsoncpp',
      'deps/abseil-cpp/abseil-cpp.gyp:abseil',
      '../libuv/uv.gyp:libuv',
      '../openssl/openssl.gyp:openssl'
    ],
    'direct_dependent_settings': {
      'include_dirs':
      [
        '.',
        'libwebrtc'
      ]
    },
    'sources':
    [
      # C++ source files.
      'libwebrtc/system_wrappers/source/field_trial.cc',
      'libwebrtc/rtc_base/rate_statistics.cc',
      'libwebrtc/rtc_base/experiments/field_trial_parser.cc',
      'libwebrtc/rtc_base/experiments/alr_experiment.cc',
      'libwebrtc/rtc_base/experiments/field_trial_units.cc',
      'libwebrtc/rtc_base/experiments/rate_control_settings.cc',
      'libwebrtc/rtc_base/network/sent_packet.cc',
      'libwebrtc/call/rtp_transport_controller_send.cc',
    ],
    'include_dirs':
    [
      'libwebrtc',
      '../../include',
      '../json/single_include/nlohmann'
    ],
    'conditions':
    [
      # Endianness.
      [ 'node_byteorder == "big"', {
          # Define Big Endian.
          'defines': [ 'MS_BIG_ENDIAN' ]
        }, {
          # Define Little Endian.
          'defines': [ 'MS_LITTLE_ENDIAN' ]
      }],
      ['OS != "win"', {
        'defines': [
          'WEBRTC_POSIX',
          # 'NDEBUG',
        ],
      }],
      [ 'OS == "mac"', {
        'defines': [
          'WEBRTC_MAC',
          # 'NDEBUG',
        ],
      }],
      [
        'OS == "linux"', {
            'defines': [
                'WEBRTC_LINUX',
                'USE_MEDIASOUP_ClASS',
                # 'NDEBUG',
            ],
        },
      ],
     ['OS =="win"', {
        'defines': [
          'WEBRTC_WIN',
          # 'NDEBUG',
        ],
      }],
      # Platform-specifics.

      [ 'OS != "win"', {
        'cflags': [ '-std=c++11' ]
      }],

      [ 'OS == "mac"', {
        'xcode_settings':
        {
          'OTHER_CPLUSPLUSFLAGS' : [ '-std=c++11' ]
        }
      }]
    ]
  },
  'targets':
  [
    {
      'target_name': 'libwebrtc',
      'type': 'static_library'
    }
  ]
}
