# This library is responsible for parsing the launch options of all the components in the assistant.
cc_library(
    name = "friday_options_library",
    srcs = ["friday_options.cc"],
    hdrs = ["friday_options.hpp", "shared/argparse.h"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:json",
        ":setup_configs_library",
        ":setup_logging_library",
    ],
    visibility = [
        "//friday:__pkg__",
        "//friday/debug:__pkg__",
        "//friday/shared:__pkg__",
    ],
)

cc_library(
    name = "setup_configs_library",
    srcs = ["setup_configs.cc"],
    hdrs = ["setup_configs.hpp"],
    deps = [
        "//friday/shared:logging",
        "//friday/shared:json",
        "//friday/shared:json_utils_library",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
)

cc_test(
    name = "setup_configs_test",
    srcs = ["setup_configs_test.cc"],
    data = ["//configs:configs"],
    deps = [
        "//friday/testing:testing_library",
        ":setup_configs_library",
    ],
)

cc_library(
    name = "setup_logging_library",
    srcs = ["setup_logging.cc"],
    hdrs = ["setup_logging.hpp"],
    deps = [
        "//friday/shared:logging",
    ],
    visibility = [
        "//friday:__pkg__",
    ],
)
