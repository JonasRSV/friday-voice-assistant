# This is where we put external header libraries that are used in multiple places across the project.
cc_library(
    name = "logging",
    hdrs = ["aixlog.hpp"],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "json",
    hdrs = ["json.hpp"],
    visibility = [
        "//visibility:public",
    ],
)


cc_library(
    name = "config_loader",
    srcs = ["config_loader.cc"],
    hdrs = ["config_loader.hpp"],
    deps = [
        "//friday/shared:json",
        "//friday/shared:logging",
    ],
    visibility = [
        "//visibility:public",
    ],
)
