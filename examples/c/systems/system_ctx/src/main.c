#include <system_ctx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Applications can pass context data to a system. A common use case where this
// comes in handy is when a system needs to iterate more than one query. The
// following example shows how to pass a custom query into a system for a simple
// collision detection example.

typedef struct {
    double x, y;
} Position;

typedef struct {
    double value;
} Radius;

double distance(const Position *p1, const Position *p2) {
    return sqrt(pow(p2->x - p1->x, 2) + pow(p2->y - p1->y, 2));
}

void Collide(ecs_iter_t *it) {
    ecs_query_t *q_collide = it->ctx; // Get query from system context
    const Position *p1 = ecs_field(it, Position, 1);
    const Radius *r1 = ecs_field(it, Radius, 2);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e1 = it->entities[i];

        // For each matching entity, iterate the query
        ecs_iter_t qit = ecs_query_iter(it->world, q_collide);
        while (ecs_query_next(&qit)) {
            const Position *p2 = ecs_field(&qit, Position, 1);
            const Radius *r2 = ecs_field(&qit, Radius, 2);
            for (int j = 0; j < qit.count; j ++) {
                ecs_entity_t e2 = qit.entities[j];
                if (e1 == e2) {
                    // don't collide with self
                    continue;
                }

                if (e2 > e1) {
                    // Simple trick to prevent collisions from being detected
                    // twice with the entities reversed.
                    continue;
                }

                // Check for collision
                double d = distance(&p1[i], &p2[j]);
                double r = r1[i].value + r2[j].value;
                if (r > d) {
                    printf("%u (r: %.0f) and %u (r: %.0f) collided! (d = %.2f)\n",
                        (uint32_t)e1, r1[i].value, (uint32_t)e2, r2[j].value, d);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Radius);

    // Create a query for Position that we can use inside the collide system to
    // check each entity with each other entity.
    ecs_query_t  *q_position = ecs_query(ecs, {
        .filter.terms = {
            { ecs_id(Position), .inout = EcsIn },
            { ecs_id(Radius), .inout = EcsIn }
        }
    });

    // Create collide system that passes query as context
    ecs_entity_t collide = ecs_system(ecs, {
        .query.filter.terms = {
            { .id = ecs_id(Position), .inout = EcsIn },
            { ecs_id(Radius), .inout = EcsIn }
        },
        .callback = Collide,
        .ctx = q_position
    });

    // Create a few test entities
    for (int i = 0; i < 10; i ++) {
        ecs_entity_t e = ecs_new_id(ecs);
        ecs_set(ecs, e, Position, { .x = rand() % 100, .y = rand() % 100 });
        ecs_set(ecs, e, Radius, { rand() % 10 + 1 });
    }

    // Run the system
    ecs_run(ecs, collide, 0.0f, NULL);

    return ecs_fini(ecs);

    // Output
    //  508 (r: 6) and 505 (r: 4) collided! (d = 9.22)
    //  510 (r: 10) and 508 (r: 6) collided! (d = 14.32)
    //  513 (r: 10) and 506 (r: 3) collided! (d = 3.61)
    //  514 (r: 6) and 512 (r: 8) collided! (d = 11.40)
}
