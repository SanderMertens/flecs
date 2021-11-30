load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

new_git_repository(
    name = "bake",
    remote = "git@github.com:SanderMertens/bake.git",
    commit = "e2b3d03b61ef36e17a41aa76cb9395b4260468aa",
    shallow_since = "1637826538 -0800",

    build_file_content = """
cc_library(
    name = "driver-test",
    visibility = ["//visibility:public"],
    deps = [":util", ":bake"],

    defines = ["bake_test_STATIC"],
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
    defines = ["UT_IMPL", "__BAKE__", "_XOPEN_SOURCE=600"],

    linkopts = select({
        "@bazel_tools//src/conditions:windows": ["-DEFAULTLIB:dbghelp -DEFAULTLIB:shell32 -DEFAULTLIB:shlwapi"],
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

