load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "parallel_cpp",
    #sha256 = "3601822b4d3c7cc62d891a2d0993b902ad1858e4faf41d895678d3a7749ec503",
    strip_prefix = "parallel-cpp-data-processing-86dc3854804e6a5e2d06fbd9466a360e69911019",
    url = "https://github.com/jvikstrom/parallel-cpp-data-processing/archive/86dc3854804e6a5e2d06fbd9466a360e69911019.zip"
    #url = "https://github.com/abseil/abseil-cpp/archive/ca3f87560a0eef716195cadf66dc6b938a579ec6.zip",
)


local_repository(
    name="org_tensorflow",
    path="local_repositories/tensorflow"
)


#Tensorflow depends on this
http_archive(
    name = "io_bazel_rules_closure",
    sha256 = "5b00383d08dd71f28503736db0500b6fb4dda47489ff5fc6bed42557c07c6ba9",
    strip_prefix = "rules_closure-308b05b2419edb5c8ee0471b67a40403df940149",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",  # 2019-06-13
    ],
)

#load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")
#tf_workspace(path_prefix = "", tf_repo_name = "org_tensorflow")


