cc_library(
    name = "tf_library",
    srcs = [
        "tf.cc"
    ],
    hdrs = [
        "tf.h"
    ],
    deps = [
        "//friday/audio/keyword_detection/goldfish/tf/tensorflow:tensorflow_library",
        #"@org_tensorflow//tensorflow/tools/lib_package:libtensorflow"
    ],
    visibility = [
          "//friday/audio/keyword_detection/goldfish:__pkg__",
    ],
    copts = [
        "--std=c++17"
    ],
    linkstatic=True
)
