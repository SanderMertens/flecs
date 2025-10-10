
cc_test(
    name = "core",
    deps = ["//:flecs", "@bake//:driver-test"],
    
    srcs = glob(["core/src/*.c", "core/**/*.h"]),
    includes = ["core/include"],
)

cc_test(
    name = "collections",
    deps = ["//:flecs", "@bake//:driver-test"],
    
    srcs = glob(["collections/src/*.c", "collections/**/*.h"]),
    includes = ["collections/include"],
)

cc_test(
    name = "cpp",
    deps = ["//:flecs", "@bake//:driver-test"],
    
    srcs = glob(["cpp/src/*.cpp", "cpp/**/*.h"]),
    includes = ["cpp/include"],
    cxxopts = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++17"],
        "//conditions:default": ["--std=c++17"],
    }),
)
