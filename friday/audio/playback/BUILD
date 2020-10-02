# This library is responsible for reading the audio
cc_library(
    name = "playback_library",
    srcs = ["playback.cc"],
    hdrs = ["playback.hpp"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:json",
        "//friday/shared:config_library",
    ],
    linkopts = ["-lasound" ],
    visibility = [
        "//friday:__pkg__",
        "//friday/audio/replay_buffer:__pkg__",
        "//friday/audio/keyword_detection:__pkg__",
    ],
)

cc_test(
    name = "playback_test",
    srcs = ["playback_test.cc"],
    deps = [
        "//friday/audio/recording:recording_library",
        "//friday/testing:testing_library",
        ":playback_library",
    ],
)
