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
    cxxopts = ["-std=c++17"],
)
