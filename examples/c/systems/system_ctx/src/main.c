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

double sqr(double value) {
    return value * value;
}

double distance_sqr(const Position *p1, const Position *p2) {
    return sqr(p2->x - p1->x) + sqr(p2->y - p1->y);
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
                double d_sqr = distance_sqr(&p1[i], &p2[j]);
                double r_sqr = sqr(r1[i].value + r2[j].value);
                if (r_sqr > d_sqr) {
                    printf("%u and %u collided!\n", (uint32_t)e1, (uint32_t)e2);
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
    //  508 and 505 collided!
    //  510 and 508 collided!
    //  513 and 506 collided!
    //  514 and 512 collided!
}
