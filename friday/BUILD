
cc_binary(
    name = "friday",
    srcs = ["friday_main.cc"],
    deps = [
        "//friday/audio/recording:recording_library",
        "//friday/audio/playback:playback_library",
        "//friday/audio/replay_buffer:replay_buffer_library",
        "//friday/audio/replay_buffer:speak_detection_library",
        "//friday/audio/keyword_detection/goldfish:goldfish_library",
        "//friday/audio/keyword_detection:keyword_detection_library",
        "//friday/setup:friday_options_library",
        "//friday/shared:logging",
        "//friday/dispatch:dispatch_library",
        "//friday/dispatch:local_library",
        "//friday/third_party/philips-hue:philips_hue_library",
    ],
    linkopts =["-pthread"]
)


