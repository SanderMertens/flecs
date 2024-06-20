#include <world_ser_deser.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position, Velocity;

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    const Velocity *v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        printf("%s: {%f, %f}\n", ecs_get_name(it->world, it->entities[i]), 
            p[i].x, p[i].y);
    }
}

// Register components and systems in a module. This excludes them by default
// from the serialized data, and makes it easier to import across worlds.
void MoveModuleImport(ecs_world_t *world) {
    ECS_MODULE(world, MoveModule);

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { .name = "x", .type = ecs_id(ecs_f64_t) },
            { .name = "y", .type = ecs_id(ecs_f64_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            { .name = "x", .type = ecs_id(ecs_f64_t) },
            { .name = "y", .type = ecs_id(ecs_f64_t) },
        }
    });

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world_a = ecs_init_w_args(argc, argv); {
        ECS_IMPORT(world_a, MoveModule); // put in a scope so variable doesn't overlap
    }

    ecs_entity_t ent_1 = ecs_entity(world_a, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world_a, { .name = "ent_2" });
    ecs_set(world_a, ent_1, Position, {10, 20});
    ecs_set(world_a, ent_2, Position, {30, 40});
    ecs_set(world_a, ent_1, Velocity, {1, -1});
    ecs_set(world_a, ent_2, Velocity, {-1, 1});

    // Serialize world to JSON
    char *json = ecs_world_to_json(world_a, NULL);
    printf("%s\n\n", json);

    // Output:
    // {
    //     "results": [{
    //         "ids": [
    //             ["my_module.Position"],
    //             ["my_module.Velocity"],
    //             ["flecs.core.Identifier", "flecs.core.Name"]
    //         ],
    //         "entities": ["ent_1", "ent_2"],
    //         "values": [
    //             [{
    //                 "x": 10,
    //                 "y": 20
    //             }, {
    //                 "x": 30,
    //                 "y": 40
    //             }],
    //             [{
    //                 "x": 1,
    //                 "y": -1
    //             }, {
    //                 "x": -1,
    //                 "y": 1
    //             }], 0
    //         ]
    //     }]
    // }

    // Create second world, import same module
    ecs_world_t *world_b = ecs_init(); {
        ECS_IMPORT(world_b, MoveModule);
    }

    // Deserialize JSON into second world
    ecs_world_from_json(world_b, json, NULL);

    // Run system once for both worlds
    ecs_progress(world_a, 0);
    printf("\n");
    ecs_progress(world_b, 0);

    // Output
    //   ent_1: {11.000000, 19.000000}
    //   ent_2: {29.000000, 41.000000}
    //   
    //   ent_1: {11.000000, 19.000000}
    //   ent_2: {29.000000, 41.000000}

    ecs_fini(world_a);
    ecs_fini(world_b);

    ecs_os_free(json);

    return 0;
}
