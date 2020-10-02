cc_library(
    name = "philips_hue_library",
    srcs = ["philips_hue.cc"],
    hdrs = ["philips_hue.hpp"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:httplib",
        "//friday/shared:config_library",
        "//friday/shared:json",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
)

cc_test(
    name = "philips_hue_test",
    srcs = ["philips_hue_test.cc"],
    data = ["//configs:configs"],
    deps = [
        "//friday/testing:testing_library",
        ":philips_hue_library",
    ],
)
