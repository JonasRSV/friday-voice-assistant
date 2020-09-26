# This library is responsible for reading the audio
cc_library(
    name = "recording_library",
    srcs = ["recording.cc"],
    hdrs = ["recording.hpp"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:json",
    ],
    linkopts = ["-lasound"],
    visibility = [
        "//friday:__pkg__",
    ],
)
