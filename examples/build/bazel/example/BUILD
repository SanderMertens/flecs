cc_binary(
    name = "example",
    srcs = ["main.cpp"],
    deps = ["@flecs"],
    copts = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
)
