#include <query_to_custom_json.h>
#include <stdio.h>

// Same example as query_to_json, but with customized serializer parameters

typedef struct {
    float x, y;
} Position, Velocity;

typedef struct {
    float value;
} Mass;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register component as usual
    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);
    ECS_COMPONENT(ecs, Mass);

    // Add reflection data to components
    ecs_struct(ecs, {
        .entity = ecs_id(Position), // Make sure to use existing id
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_struct(ecs, {
        .entity = ecs_id(Velocity), // Make sure to use existing id
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_struct(ecs, {
        .entity = ecs_id(Mass), // Make sure to use existing id
        .members = {
            { .name = "value", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_entity_t a = ecs_set_name(ecs, 0, "a");
    ecs_entity_t b = ecs_set_name(ecs, 0, "b");
    ecs_entity_t c = ecs_set_name(ecs, 0, "c");
    ecs_entity_t d = ecs_set_name(ecs, 0, "d");

    ecs_set(ecs, a, Position, {10, 20});
    ecs_set(ecs, b, Position, {20, 30});
    ecs_set(ecs, c, Position, {30, 40});
    ecs_set(ecs, d, Position, {40, 50});

    ecs_set(ecs, a, Velocity, {1, 2});
    ecs_set(ecs, b, Velocity, {2, 3});
    ecs_set(ecs, c, Velocity, {3, 4});
    ecs_set(ecs, d, Velocity, {4, 5});
    
    ecs_set(ecs, c, Mass, {10});
    ecs_set(ecs, d, Mass, {20});

    // Query for components
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }
        }
    });

    ecs_iter_t it = ecs_query_iter(ecs, q);

    // Serialize query to JSON. Customize serializer to only serialize entity
    // names and component values.
    ecs_iter_to_json_desc_t desc = {
        .serialize_values = true
    };
    char *json = ecs_iter_to_json(&it, &desc);
    printf("%s\n", json);
    ecs_os_free(json);

    // Iterator returns 2 sets of results, one for each table.

    // {
    //   "results": [
    //     {
    //       "name": "a",
    //       "fields": [
    //         {
    //           "data": {
    //             "x": 10,
    //             "y": 20
    //           }
    //         },
    //         {
    //           "data": {
    //             "x": 1,
    //             "y": 2
    //           }
    //         }
    //       ]
    //     },
    //     {
    //       "name": "b",
    //       "fields": [
    //         {
    //           "data": {
    //             "x": 20,
    //             "y": 30
    //           }
    //         },
    //         {
    //           "data": {
    //             "x": 2,
    //             "y": 3
    //           }
    //         }
    //       ]
    //     },
    //     {
    //       "name": "c",
    //       "fields": [
    //         {
    //           "data": {
    //             "x": 30,
    //             "y": 40
    //           }
    //         },
    //         {
    //           "data": {
    //             "x": 3,
    //             "y": 4
    //           }
    //         }
    //       ]
    //     },
    //     {
    //       "name": "d",
    //       "fields": [
    //         {
    //           "data": {
    //             "x": 30,
    //             "y": 40
    //           }
    //         },
    //         {
    //           "data": {
    //             "x": 4,
    //             "y": 5
    //           }
    //         }
    //       ]
    //     }
    //   ]
    // }

    ecs_fini(ecs);
}
