load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
    name = "rules_python",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.0.2/rules_python-0.0.2.tar.gz",
    strip_prefix = "rules_python-0.0.2",
    sha256 = "b5668cde8bb6e3515057ef465a35ad712214962f0b3a314e551204266c7be90c",
)

load("@rules_python//python:pip.bzl", "pip_repositories")
pip_repositories()

load("@rules_python//python:pip.bzl", "pip3_import")


pip3_import(   # or pip3_import
   name = "goldfish_deps",
   requirements = "//friday/audio/keyword_detection/goldfish:requirements.txt",
)

load("@goldfish_deps//:requirements.bzl", _goldfish_install = "pip_install")
_goldfish_install()


http_archive(
    name = "parallel_cpp",
    #sha256 = "3601822b4d3c7cc62d891a2d0993b902ad1858e4faf41d895678d3a7749ec503",
    strip_prefix = "parallel-cpp-data-processing-86dc3854804e6a5e2d06fbd9466a360e69911019",
    url = "https://github.com/jvikstrom/parallel-cpp-data-processing/archive/86dc3854804e6a5e2d06fbd9466a360e69911019.zip"
    #url = "https://github.com/abseil/abseil-cpp/archive/ca3f87560a0eef716195cadf66dc6b938a579ec6.zip",
)


