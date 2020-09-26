cc_library(
    name = "cppflow_library",
    srcs = [
        "cppflow.cc"
    ],
    hdrs = [
        "cppflow.h"
    ],
    deps = [
        "//friday/audio/keyword_detection/goldfish/cppflow/tensorflow:tensorflow_library_gpu",
    ],
    visibility = [
          "//friday/audio/keyword_detection/goldfish:__pkg__",
    ],
    copts = [
        "--std=c++17"
    ]
)
