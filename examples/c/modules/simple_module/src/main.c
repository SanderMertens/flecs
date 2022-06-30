#include <simple_module.h>
#include <stdio.h>

void PrintPosition(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("p = {%f, %f} (system)\n", p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Import module which calls the SimpleModuleImport function
    ECS_IMPORT(world, SimpleModule);

    // Create system that uses component from module. Note how the component
    // identifier is prefixed with the module.
    ECS_SYSTEM(world, PrintPosition, EcsOnUpdate, simple.module.Position);

    // Create entity with components imported from module
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
                     ecs_set(world, e, Velocity, {1, 2});

    // Call progress which runs imported Move system
    ecs_progress(world, 0);

    // Use component from module in operation
    const Position *p = ecs_get(world, e, Position);
    printf("p = {%f, %f} (get)\n", p->x, p->y);

    return ecs_fini(world);

    // Output:
    //   p = {11.000000, 22.000000} (system)
    //   p = {11.000000, 22.000000} (get)
}
