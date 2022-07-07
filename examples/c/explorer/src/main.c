#include <explorer.h>

typedef struct {
    double value;
} Mass;

int main(int argc, char *argv[]) {
    // Passing in the command line arguments will allow the explorer to display
    // the application name.
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsUnits);
    ECS_IMPORT(world, FlecsMonitor); // Collect statistics periodically

    // Mass component
    ECS_COMPONENT(world, Mass);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Mass),
        .members = {
            { .name = "value", .type = ecs_id(ecs_f64_t), .unit = EcsKiloGrams}
        }
    });

    // Simple hierarchy
    ecs_entity_t Sun = ecs_new_entity(world, "Sun");
    ecs_set(world, Sun, Mass, {1.988500e31});

    ecs_set_scope(world, Sun);
    ecs_entity_t Earth = ecs_new_entity(world, "Earth");
    ecs_set(world, Earth, Mass, {5.9722e24});

    ecs_set_scope(world, Earth);
    ecs_entity_t Moon = ecs_new_entity(world, "Moon");
    ecs_set(world, Moon, Mass, {7.34767309e22});

    // Restore default hierarchy scope to root
    ecs_set_scope(world, 0);
    
    // Run application with REST interface. When the application is running,
    // navigate to https://flecs.dev/explorer to inspect it!
    //
    // See docs/RestApi.md#explorer for more information.
    return ecs_app_run(world, &(ecs_app_desc_t){
        .enable_rest = true
    });
}
