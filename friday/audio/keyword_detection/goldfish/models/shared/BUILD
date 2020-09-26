load("@rules_python//python:defs.bzl", "py_library")
load("@goldfish_deps//:requirements.bzl", "requirement")

py_library(name="feature_extraction",
           srcs=["audio.py"],
           deps=[
              requirement("tensorflow"),
          ],
          visibility = [
            "//friday/audio/keyword_detection/goldfish/models/golding:__pkg__",
        ],
)

