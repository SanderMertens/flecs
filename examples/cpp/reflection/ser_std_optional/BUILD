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
    copts = ["-std=c++17"],
)
