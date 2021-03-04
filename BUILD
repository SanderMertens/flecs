
cc_library(
    name = "flecs",
    visibility = ["//visibility:public"],

    srcs = glob(["src/**/*.c", "src/**/*.h"]),
    hdrs = glob(["include/**/*.h", "include/**/*.hpp"]),
    includes = ["include"],
)



cc_library(
    name = "os-api",
    deps = [":flecs", "@bake//:util"],

    srcs = glob(["examples/os_api/flecs-os_api-bake/src/**/*.c", "examples/os_api/flecs-os_api-bake/src/**/*.h"]),
    hdrs = glob(["examples/os_api/flecs-os_api-bake/include/**/*.h"]),
    includes = ["examples/os_api/flecs-os_api-bake/include"],
)

cc_test(
    name = "api",
    deps = [":flecs", ":os-api", "@bake//:driver-test"],

    srcs = glob(["test/api/**/*.c", "test/api/**/*.h"]),
    includes = ["test/api/include"],

    timeout = "moderate",
)

cc_test(
    name = "collections",
    deps = [":flecs", "@bake//:driver-test"],
    
    srcs = glob(["test/collections/**/*.c", "test/collections/**/*.h"]),
    includes = ["test/collections/include"],

    timeout = "short",
)

cc_test(
    name = "cpp_api",
    deps = [":flecs", "@bake//:driver-test"],
    
    srcs = glob(["test/cpp_api/**/*.cpp", "test/cpp_api/**/*.h"]),
    includes = ["test/cpp_api/include"],

    timeout = "short",
)
