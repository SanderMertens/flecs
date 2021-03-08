load(":bake_tests.bzl", "persuite_bake_tests")

cc_library(
    name = "test-api",
    deps = ["//:flecs", "//examples:os-api", "@bake//:driver-test"],

    srcs = glob(["api/src/*.c", "api/**/*.h"]),
    includes = ["api/include"],
)

persuite_bake_tests("api", [":test-api"], glob(["api/src/*.c"], exclude=["api/src/main.c", "api/src/util.c"]))

cc_test(
    name = "collections",
    deps = ["//:flecs", "@bake//:driver-test"],
    
    srcs = glob(["collections/src/*.c", "collections/**/*.h"]),
    includes = ["collections/include"],

    timeout = "short",
)

cc_test(
    name = "cpp_api",
    deps = ["//:flecs", "@bake//:driver-test"],
    
    srcs = glob(["cpp_api/src/*.cpp", "cpp_api/**/*.h"]),
    includes = ["cpp_api/include"],

    timeout = "short",
)
