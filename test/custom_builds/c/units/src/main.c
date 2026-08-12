#include <units.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsUnits);

    assert(ecs_lookup(world, "flecs.units") != 0);
    assert(EcsUnitPrefixes != 0);

    return ecs_fini(world);
}
