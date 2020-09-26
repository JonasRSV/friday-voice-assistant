load("@rules_python//python:defs.bzl", "py_binary")
load("@rules_python//python:defs.bzl", "py_library")
load("@goldfish_deps//:requirements.bzl", "requirement")



py_binary(name="inspect_goldfish_voice",
          srcs=["inspect_goldfish_voice.py"],
          deps=[
              requirement("tensorflow"),
              requirement("simpleaudio"),
              ":goldfish_utils",
          ],
          python_version="PY3",
)

py_library(name="goldfish_utils",
           srcs=["goldfish_utils.py"],
           deps=[
              requirement("tensorflow"),
          ],
          visibility = [
            "//friday/audio/keyword_detection/goldfish/preprocessing:__pkg__",
            "//friday/audio/keyword_detection/goldfish/shared:__pkg__",
            "//friday/audio/keyword_detection/goldfish/models/golding:__pkg__",
        ],
)

