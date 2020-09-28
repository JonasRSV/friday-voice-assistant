# This library is responsible for returning the frames which to predict on
cc_library(
    name = "replay_buffer_library",
    srcs = ["replay_buffer.cc"],
    hdrs = ["replay_buffer.hpp"],
    deps = [
        "//friday/audio/recording:recording_library",
        "//friday/shared:logging",
        "//friday/shared:json",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
    linkopts =["-pthread"]
)

cc_test(
    name = "replay_buffer_test",
    srcs = ["replay_buffer_test.cc"],
    deps = [
        "//friday/testing:testing_library",
        ":replay_buffer_library",
    ],
)
