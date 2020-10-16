config_setting(
    name="k8-tf-gpu",
    values = {
        "cpu": "k8",
        "define": "platform=linux-x86-gpu"
        }
)

config_setting(
    name="k8-tf-cpu",
    values = {
        "cpu": "k8",
        "define": "platform=linux-x86-cpu"
    }
)

config_setting(
    name="arm-pi",
    values = {
        "define": "platform=linux-arm-cpu"
    }
)


cc_library(
    name = "tensorflow_library",
    #srcs = select({
      #":k8-tf-gpu": [
         #"lib/linux-x86-64-gpu/libtensorflow.so.1.14.0", 
         #"lib/linux-x86-64-gpu/libtensorflow_framework.so.1.14.0"
      #],
      #":k8-tf-cpu": [
         #"lib/linux-x86-64-cpu/libtensorflow.so.1", 
         #"lib/linux-x86-64-cpu/libtensorflow_framework.so.1"
      #],
      #":arm-pi": [
         #"lib/raspberryPi32/libtensorflow.so", 
         #"lib/raspberryPi32/libtensorflow_framework.so"
      #]},
      #no_match_error = "\n\n(Tensorflow): Please define platform with one of (--define platform=linux-x86-cpu | --define platform=linux-x86-gpu | --define platform=linux-arm-cpu)\nYou probably want --define platform=linux-x86-cpu if you're testing on your laptop\n\n"
    #),
    deps = [
        "@org_tensorflow//tensorflow/core:lib"
    ],
    hdrs = [
        "include/tensorflow/c/c_api.h", 
        "include/tensorflow/c/tf_attrtype.h",
        "include/tensorflow/c/c_api_experimental.h",
        "include/tensorflow/c/eager/c_api.h", 
    ],
    visibility = [
        "//friday/audio/keyword_detection/goldfish/tf:__pkg__",
    ],
    linkstatic=True,
)

