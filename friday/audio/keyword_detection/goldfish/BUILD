cc_library(
    name = "goldfish_library",
    srcs = [
        "goldfish.cc"
    ],
    hdrs = [
        "goldfish.hpp"
    ],
    deps = [
        "//friday/audio/keyword_detection/goldfish/cppflow:cppflow_library",
        "//friday/shared:logging",
        "//friday/shared:json",
    ],
    visibility = [
          "//visibility:public",
    ],
)

cc_test(
    name = "goldfish_test",
    srcs = ["goldfish_test.cc"],
    data = ["//friday/audio/keyword_detection/goldfish/models/saved_models/test_model:test_model"],
    deps = [
        "//friday/testing:testing_library",
        ":goldfish_library",
    ],
)
