cc_binary(
    name = "ser_std_optional",
    srcs = glob([
        "src/*.cpp",
        "include/**/*.h",
    ]),
    includes = ["include"],
    deps = [
        "//:flecs",
    ],
    cxxopts = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++17"],
        "//conditions:default": ["--std=c++17"],
    }),
)
