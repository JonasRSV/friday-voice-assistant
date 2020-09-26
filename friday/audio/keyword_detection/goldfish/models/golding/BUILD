load("@rules_python//python:defs.bzl", "py_binary")
load("@goldfish_deps//:requirements.bzl", "requirement")

py_binary(name="golding",
          srcs=["golding.py"],
          deps=[
              requirement("tensorflow"),
              "//friday/audio/keyword_detection/goldfish/shared:goldfish_utils",
              "//friday/audio/keyword_detection/goldfish/models/shared:feature_extraction",
          ],
          python_version="PY3",
)

