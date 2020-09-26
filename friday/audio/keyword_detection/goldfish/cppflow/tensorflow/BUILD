cc_library(
    name = "tensorflow_library_gpu",
    srcs = [
        "lib/gpu/libtensorflow.so", 
        "lib/gpu/libtensorflow.so.1", 
        "lib/gpu/libtensorflow.so.1.14.0", 
        "lib/gpu/libtensorflow_framework.so",
        "lib/gpu/libtensorflow_framework.so.1",
        "lib/gpu/libtensorflow_framework.so.1.14.0"
    ],
    hdrs = [
        "include/tensorflow/c/c_api.h", 
        "include/tensorflow/c/tf_attrtype.h",
        "include/tensorflow/c/c_api_experimental.h",
        "include/tensorflow/c/eager/c_api.h", 
    ],
    visibility = [
        "//friday/audio/keyword_detection/goldfish/cppflow:__pkg__",
    ],
)

cc_library(
    name = "tensorflow_library_cpu",
    srcs = [
        "lib/cpu/libtensorflow.so", 
        "lib/cpu/libtensorflow.so.1", 
        "lib/cpu/libtensorflow.so.1.14.0", 
        "lib/cpu/libtensorflow_framework.so",
        "lib/cpu/libtensorflow_framework.so.1",
        "lib/cpu/libtensorflow_framework.so.1.14.0"
    ],
    hdrs = [
        "include/tensorflow/c/c_api.h", 
        "include/tensorflow/c/tf_attrtype.h",
        "include/tensorflow/c/c_api_experimental.h",
        "include/tensorflow/c/eager/c_api.h", 
    ],
    visibility = [
        "//friday/audio/keyword_detection/goldfish/cppflow:__pkg__",
    ],
)
