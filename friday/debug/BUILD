
cc_test(
    name = "debug_friday_audio_test",
    srcs = ["debug_friday_audio_test.cc"],
    data = [
        "//configs:configs",
    ],
    deps = [
        "//friday/testing:testing_library",
        "//friday/audio/recording:recording_library",
        "//friday/audio/replay_buffer:replay_buffer_library",
        "//friday/audio/replay_buffer:speak_detection_library",
        "//friday/audio/keyword_detection/goldfish:goldfish_library",
        "//friday/setup:friday_options_library",
        "//friday/shared:logging",
    ],
    linkopts =["-pthread"]
)
