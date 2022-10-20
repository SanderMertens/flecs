#include <custom_runner.h>
#include <stdio.h>

// Systems can be created with a custom run function that takes control over the
// entire iteration. By default, a system is invoked once per matched table, 
// which means the function can be called multiple times per frame. In some
// cases that's inconvenient, like when a system has things it needs to do only
// once per frame. For these use cases, the run callback can be used which is
// called once per frame per system.

typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    printf("Move begin\n");

    // In a run function we have to manually iterate through the query results.
    // Note that this code looks the same as iterating a query.
    while (ecs_query_next(it)) {
        // Inside the loop the code looks the same as for a regular system
        Position *p = ecs_field(it, Position, 1);
        const Velocity *v = ecs_field(it, Velocity, 2);

        for (int i = 0; i < it->count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            printf("%s: {%f, %f}\n", ecs_get_name(it->world, it->entities[i]), 
                p[i].x, p[i].y);
        }
    }

    printf("Move end\n");
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    // Create a system for Position, Velocity
    ecs_entity_t move = ecs_system(ecs, {
        .query.filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .inout = EcsIn }
        },
        .run = Move // use .run instead of .callback
    });

    // C applications can also use the ECS_SYSTEM convenience macro:
    // ECS_SYSTEM(ecs, Move, 0, Position, [in] Velocity);

    // Create a few test entities for a Position, Velocity query
    ecs_entity_t e1 = ecs_new_entity(ecs, "e1");
    ecs_set(ecs, e1, Position, {10, 20});
    ecs_set(ecs, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_entity(ecs, "e2");
    ecs_set(ecs, e2, Position, {10, 20});
    ecs_set(ecs, e2, Velocity, {3, 4});

    // This entity will not match as it does not have Position, Velocity
    ecs_entity_t e3 = ecs_new_entity(ecs, "e3");
    ecs_set(ecs, e3, Position, {10, 20});

    // Run the system
    ecs_run(ecs, move, 0.0f, NULL);

    return ecs_fini(ecs);

    // Output
    //  Move begin
    //  e1: {11.000000, 22.000000}
    //  e2: {13.000000, 24.000000}
    //  Move end
}
