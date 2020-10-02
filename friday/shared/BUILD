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
    name = "config_library",
    srcs = ["config.cc"],
    hdrs = ["config.hpp"],
    deps = [
        "//friday/shared:json",
        "//friday/shared:logging",
    ],
    visibility = [
        "//visibility:public",
    ],
)

# This is where we put external header libraries that are used in controlling things via web.
cc_library(
    name = "httplib",
    hdrs = ["httplib.h"],
    visibility = [
        "//visibility:public",
    ],
)

