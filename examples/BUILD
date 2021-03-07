cc_library(
    name = "os-api",
    visibility = ["//:__subpackages__"],
    deps = ["//:flecs", "@bake//:util"],

    srcs = glob(["os_api/flecs-os_api-bake/src/**/*.c", "os_api/flecs-os_api-bake/src/**/*.h"]),
    hdrs = glob(["os_api/flecs-os_api-bake/include/**/*.h"]),
    includes = ["os_api/flecs-os_api-bake/include"],
)
