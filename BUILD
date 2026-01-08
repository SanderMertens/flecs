cc_library(
    name = "flecs",
    visibility = ["//visibility:public"],
    defines = ["flecs_EXPORTS"],

    srcs = glob(["src/**/*.c", "src/**/*.h", "src/**/*.inl"]),
    hdrs = glob(["include/**/*.h", "include/**/*.hpp", "include/**/*.inl"]),
    includes = ["include"],

    copts = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
)
