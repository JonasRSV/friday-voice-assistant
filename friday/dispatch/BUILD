

cc_library(
    name = "dispatch_library",
    srcs = ["dispatch.cc"],
    hdrs = ["dispatch.hpp"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:json",
        "//friday/third_party/philips-hue:philips_hue_library",
        ":local_library",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
    linkopts =["-pthread"]
)


cc_library(
    name = "local_library",
    srcs = ["local.cc"],
    hdrs = ["local.hpp"],
    deps = [
        "//friday/shared:json",
        "//friday/shared:logging",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
)
