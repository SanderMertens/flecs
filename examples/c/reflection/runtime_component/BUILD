cc_binary(
    name = "runtime_component",
    srcs = glob([
        "src/*.c",
        "include/**/*.h",
    ]),
    includes = ["include"],
    deps = [
        "//:flecs",
    ],
)
