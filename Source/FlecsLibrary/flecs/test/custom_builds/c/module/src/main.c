#include <module.h>

void MyModuleImport(ecs_world_t *world) {
    ECS_MODULE(world, MyModule);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, MyModule);

    return ecs_fini(world);
}
