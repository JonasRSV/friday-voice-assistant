# This library is responsible for reading the audio
cc_library(
    name = "recording_library",
    srcs = ["recording.cc"],
    hdrs = ["recording.hpp"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:json",
        "//friday/shared:config_library",
    ],
    linkopts = ["-lasound" ],
    visibility = [
        "//friday:__pkg__",
        "//friday/audio/replay_buffer:__pkg__",
        "//friday/audio/playback:__pkg__",
        "//friday/debug:__pkg__",
    ],
)
