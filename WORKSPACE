load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

new_git_repository(
    name = "bake",
    remote = "git@github.com:SanderMertens/bake.git",
    commit = "95aaf38de363be4ca693b867d9334d0967475d5e",
    shallow_since = "1610936730 -0800",

    build_file_content = """
cc_library(
    name = "driver-test",
    visibility = ["//visibility:public"],
    deps = [":util", ":bake"],

    srcs = glob(["drivers/test/src/**/*.c", "drivers/test/src/**/*.h"]),
    hdrs = glob(["drivers/test/include/**/*.h"]),
    includes = ["drivers/test/include"],
)

cc_library(
    name = "bake",
    visibility = ["//visibility:public"],
    deps = [":util"],

    srcs = glob(["src/*.c", "src/*.h"]),
    hdrs = glob(["include/*.h", "include/bake/*.h"]),
    includes = ["include"],
)

cc_library(
    name = "util",
    visibility = ["//visibility:public"],
    defines = ["__BAKE__", "_XOPEN_SOURCE=600"],

    linkopts = select({
        "@bazel_tools//src/conditions:windows": [],
        "//conditions:default": ["-lrt -lpthread -ldl"],
    }),

    srcs = glob(["util/src/*.c"]) + select({
        "@bazel_tools//src/conditions:windows": glob(["util/src/win/*.c"]),
        "//conditions:default": glob(["util/src/posix/*.c"]),
    }),
    hdrs = glob(["util/include/*.h", "util/include/bake-util/*.h"]) + select({
        "@bazel_tools//src/conditions:windows": glob(["util/include/bake-util/win/*.h"]),
        "//conditions:default": glob(["util/include/bake-util/posix/*.h"]),
    }),
    includes = ["util/include"],
)
"""
)

