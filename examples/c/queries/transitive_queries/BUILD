cc_binary(
    name = "transitive_queries",
    srcs = glob([
        "src/*.c",
        "include/**/*.h",
    ]),
    includes = ["include"],
    deps = [
        "//:flecs",
    ],
)
