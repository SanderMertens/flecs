#include <sorting.h>

typedef struct {
    double x, y;
} Position;

/* Order by x member of Position */
int compare_position(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2)
{
    (void)e1;
    (void)e2;
    const Position *p1 = ptr1;
    const Position *p2 = ptr2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

/* Iterate iterator, printed values will be ordered */
void print_iter(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        printf("{%f, %f}\n", p[i].x, p[i].y);
    }
}

/* Iterate query */
void print_query(ecs_query_t *q) {
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        print_iter(&it);
    }
}

/* Iterate system */
void PrintSystem(ecs_iter_t *it) {
    print_iter(it);
}

int main(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Create entities, set Position in random order */
    ecs_entity_t e = ecs_set(world, 0, Position, {3, 0});
    ecs_set(world, 0, Position, {1, 0});
    ecs_set(world, 0, Position, {6, 0});
    ecs_set(world, 0, Position, {2, 0});
    ecs_set(world, 0, Position, {5, 0});
    ecs_set(world, 0, Position, {4, 0});

    /* Create system. To enable sorting for a system, we need to access its
     * underlying query object */
    ECS_SYSTEM(world, PrintSystem, 0, Position);
    ecs_query_t *q_system = ecs_get_system_query(world, PrintSystem);

    /* We can now invoke the order_by operation on the query */
    ecs_query_order_by(world, q_system, ecs_id(Position), compare_position);

    /* Create a query for component Position */
    ecs_query_t *q = ecs_query_new(world, "Position");

    /* Order by Position component */
    ecs_query_order_by(world, q, ecs_id(Position), compare_position);

    /* Iterate query, print values of Position */
    printf("-- First iteration\n");
    print_query(q);

    /* Change the value of one entity, invalidating the order */
    ecs_set(world, e, Position, {7, 0});

    /* Iterate query again, printed values are still ordered */
    printf("\n-- Second iteration\n");
    print_query(q);

    /* Create new entity to show that data is also sorted for system */
    ecs_set(world, 0, Position, {3, 0});

    /* Run the system, output will be sorted */
    printf("\n-- System iteration\n");
    ecs_run(world, PrintSystem, 0, NULL);

    /* Cleanup */
    return ecs_fini(world);
}
