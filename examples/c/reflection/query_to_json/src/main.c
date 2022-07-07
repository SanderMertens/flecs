#include <query_to_json.h>
#include <stdio.h>

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
    ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position), // Make sure to use existing id
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Velocity), // Make sure to use existing id
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Mass), // Make sure to use existing id
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
    ecs_query_t *q = ecs_query_init(ecs, &(ecs_query_desc_t){
        .filter.terms = {
            { .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }
        }
    });

    // Serialize query to JSON. Note that this works for iterators from any
    // source, including filters & rules.
    ecs_iter_t it = ecs_query_iter(ecs, q);
    char *json = ecs_iter_to_json(ecs, &it, NULL);
    printf("%s\n", json);
    ecs_os_free(json);

    // Iterator returns 2 sets of results, one for each table.

    // {
    //   "ids": ["Position", "Velocity"],
    //   "results": [{
    //     "ids": ["Position", "Velocity"],
    //     "subjects": [0, 0],
    //     "is_set": [true, true],
    //     "entities": ["a", "b"],
    //     "values": [
    //       [{
    //         "x": 10.00,
    //         "y": 20.00
    //       }, {
    //         "x": 20.00,
    //         "y": 30.00
    //       }],
    //       [{
    //         "x": 1.00,
    //         "y": 2.00
    //       }, {
    //         "x": 2.00,
    //         "y": 3.00
    //       }]
    //     ]
    //   }, {
    //     "ids": ["Position", "Velocity"],
    //     "subjects": [0, 0],
    //     "is_set": [true, true],
    //     "entities": ["c", "d"],
    //     "values": [
    //       [{
    //         "x": 30.00,
    //         "y": 40.00
    //       }, {
    //         "x": 40.00,
    //         "y": 50.00
    //       }],
    //       [{
    //         "x": 3.00,
    //         "y": 4.00
    //       }, {
    //         "x": 4.00,
    //         "y": 5.00
    //       }]
    //     ]
    //   }]
    // }

    ecs_fini(ecs);
}
