load("@rules_python//python:defs.bzl", "py_binary")
load("@goldfish_deps//:requirements.bzl", "requirement")

py_binary(name="toppydat",
          srcs=["toppydat.py"],
          deps=[
              requirement("tensorflow"),
              "//friday/audio/keyword_detection/goldfish/shared:goldfish_utils",
          ],
          python_version="PY3",
)

