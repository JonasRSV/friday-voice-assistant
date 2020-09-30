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

py_binary(name="train_valid_split",
          srcs=["train_valid_split.py"],
          deps=[
              requirement("tensorflow"),
          ],
          python_version="PY3",
)

py_binary(name="record_personal_examples",
          srcs=["record_personal_examples.py"],
          deps=[
              requirement("simpleaudio"),
              requirement("sounddevice"),
              requirement("tensorflow"),
              "//friday/audio/keyword_detection/goldfish/shared:goldfish_utils",
          ],
          python_version="PY3",
)

cc_binary(
          name="data_pipeline",
          srcs=[
            "data_pipeline.cc",
          ],
	  deps=[
            "@parallel_cpp//src:map_reduce",
          ],
          copts = [
            "-std=c++2a",
          ],
)
