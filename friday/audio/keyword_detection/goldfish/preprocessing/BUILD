load("@rules_python//python:defs.bzl", "py_binary")
load("@goldfish_deps//:requirements.bzl", "requirement")

py_binary(name="common_voice_to_goldfish_voice",
          srcs=["common_voice_to_goldfish_voice.py"],
          deps=[
              requirement("numpy"),
              requirement("tensorflow"),
              "//friday/audio/keyword_detection/goldfish/shared:goldfish_utils",
          ],
          python_version="PY3",
)

py_binary(name="google_speech_commands_to_goldfish_voice",
          srcs=["google_speech_commands_to_goldfish_voice.py"],
          deps=[
              requirement("numpy"),
              requirement("tensorflow"),
              "//friday/audio/keyword_detection/goldfish/shared:goldfish_utils",
          ],
          python_version="PY3",
)

py_binary(name="preprocess_goldfish",
          srcs=["preprocess_goldfish.py"],
          deps=[
              requirement("tensorflow"),
              "//friday/audio/keyword_detection/goldfish/shared:goldfish_utils",
          ],
          python_version="PY3",
)

