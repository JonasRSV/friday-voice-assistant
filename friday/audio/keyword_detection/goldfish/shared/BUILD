py_binary(name="inspect_goldfish_voice",
          srcs=["inspect_goldfish_voice.py"],
          deps=[
              ":goldfish_utils",
          ],
          python_version="PY3",
)

py_library(name="goldfish_utils",
           srcs=["goldfish_utils.py"],
           deps=[],
          visibility = [
            "//friday/audio/keyword_detection/goldfish/preprocessing:__pkg__",
            "//friday/audio/keyword_detection/goldfish/shared:__pkg__",
            "//friday/audio/keyword_detection/goldfish/models/magikarp:__pkg__",
            "//friday/audio/keyword_detection/goldfish/models/toppydat:__pkg__",
        ],
)

