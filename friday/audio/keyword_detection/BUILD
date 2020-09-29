
cc_library(
    name = "keyword_detection_library",
    srcs = ["keyword_detection.cc"],
    hdrs = ["keyword_detection.hpp"],
    deps = [
        "//friday/audio/replay_buffer:replay_buffer_library",
        "//friday/audio/keyword_detection/goldfish:goldfish_library",
        "//friday/shared:logging",
        "//friday/shared:json",
        "//friday/shared:config_library",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
)

cc_test(
    name = "keyword_detection_test",
    srcs = ["keyword_detection_test.cc"],
    data = [
        "//friday/audio/keyword_detection/goldfish/configs:label_maps",
        "//friday/audio/keyword_detection/goldfish/models/saved_models/test_model:test_model",
    ],
    deps = [
        "//friday/testing:testing_library",
        ":keyword_detection_library",
    ],
)
