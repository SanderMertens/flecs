#include <host.h>
#include <stdio.h>

char* module_from_dl(const char *module) {
    (void)module;
#if defined(ECS_TARGET_DARWIN)
    return ecs_os_strdup("libphysics_plugin.dylib");
#elif defined(ECS_TARGET_POSIX)
    return ecs_os_strdup("libphysics_plugin.so");
#elif defined(ECS_TARGET_WINDOWS)
    return ecs_os_strdup("physics_plugin.dll");
#else
    return ecs_os_strdup(module); // best effort
#endif
}

int main(int argc, char *argv[]) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_get_api();
    os_api.module_to_dl_ = module_from_dl;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_entity_t e = ecs_import_from_library(world, "physics.plugin", NULL);
    if (!e) {
        ecs_err("failed to load module");
        return 1;
    }

    if (e != ecs_lookup(world, "physics.plugin")) {
        ecs_err("invalid name for module");
        return 1;
    }

    return ecs_fini(world);
}
