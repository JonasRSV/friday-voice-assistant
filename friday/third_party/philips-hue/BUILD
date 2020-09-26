cc_library(
    name = "philips_hue_library",
    srcs = ["philips_hue_main.cc"],
    hdrs = ["philips_hue.hpp"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:config_loader",
        "//friday/control-web/shared:httplib",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
)

cc_test(
    name = "philips_hue_main_test",
    srcs = ["philips_hue_main_test.cc"],
    data = ["//configs/philips-hue:philips_hue_files"],
    deps = ["//friday/testing:testing_library",
            "//friday/philips-hue:philips_hue_library",
    ],
)
