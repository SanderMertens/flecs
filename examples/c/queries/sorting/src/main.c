#include <sorting.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position;

// Order by x member of Position
int compare_position(
    ecs_entity_t e1,
    const Position *p1,
    ecs_entity_t e2,
    const Position *p2)
{
    (void)e1;
    (void)e2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

void print_position(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    for (int i = 0; i < it->count; i ++) {
        printf("{%.1f, %.1f}\n", p[i].x, p[i].y);
    }
}

// Iterate query, printed values will be ordered
void print_query(ecs_world_t *ecs, ecs_query_t *q) {
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        print_position(&it);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);

    ecs_entity_t e = ecs_insert(ecs, ecs_value(Position, {1, 0}));
    ecs_insert(ecs, ecs_value(Position, {6, 0}));
    ecs_insert(ecs, ecs_value(Position, {2, 0}));
    ecs_insert(ecs, ecs_value(Position, {5, 0}));
    ecs_insert(ecs, ecs_value(Position, {4, 0}));

    // Create a sorted system
    ecs_entity_t sys = ecs_system(ecs, {
        .query = {
            .terms = {{ .id = ecs_id(Position) }},
            .order_by_callback = (ecs_order_by_action_t)compare_position,
            .order_by = ecs_id(Position) 
        },
        .callback = print_position
    });

    // Create sorted query
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {{ .id = ecs_id(Position) }},
        .order_by_callback = (ecs_order_by_action_t)compare_position,
        .order_by = ecs_id(Position)
    });

    // Iterate query, print values of Position
    printf("-- First iteration\n");
    print_query(ecs, q);

    // Change the value of one entity, invalidating the order
    ecs_set(ecs, e, Position, {7, 0});

    // Iterate query again, printed values are still ordered
    printf("\n-- Second iteration\n");
    print_query(ecs, q);

    // Create new entity to show that data is also sorted for system
    ecs_insert(ecs, ecs_value(Position, {3, 0}));

    printf("\n-- System iteration\n");
    ecs_run(ecs, sys, 0, NULL);

    // Output
    //  -- First iteration
    //  {1.0, 0.0}
    //  {2.0, 0.0}
    //  {4.0, 0.0}
    //  {5.0, 0.0}
    //  {6.0, 0.0}
    //  
    //  -- Second iteration
    //  {2.0, 0.0}
    //  {4.0, 0.0}
    //  {5.0, 0.0}
    //  {6.0, 0.0}
    //  {7.0, 0.0}
    //  
    //  -- System iteration
    //  {2.0, 0.0}
    //  {3.0, 0.0}
    //  {4.0, 0.0}
    //  {5.0, 0.0}
    //  {6.0, 0.0}
    //  {7.0, 0.0}

    return ecs_fini(ecs);
}
