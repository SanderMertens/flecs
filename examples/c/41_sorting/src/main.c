#include <sorting.h>

typedef struct Position {
    float x;
    float y;
} Position;

/* Order by x member of Position */
int compare_position(
    ecs_entity_t e1,
    void *ptr1,
    ecs_entity_t e2,
    void *ptr2)
{
    Position *p1 = ptr1;
    Position *p2 = ptr2;
    return p1->x - p2->x;
}

/* Iterate query, printed values will be ordered */
void print_query(ecs_query_t *q) {
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_column(&it, Position, 1);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            printf("{%f, %f}\n", p[i].x, p[i].y);
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Create entities, set Position in random order */
    ecs_entity_t e = ecs_set(world, 0, Position, {3, 0});
    ecs_set(world, 0, Position, {1, 0});
    ecs_set(world, 0, Position, {6, 0});
    ecs_set(world, 0, Position, {2, 0});
    ecs_set(world, 0, Position, {5, 0});
    ecs_set(world, 0, Position, {4, 0});

    /* Create a query for component Position */
    ecs_query_t *q = ecs_query_new(world, "Position");

    /* Order by Position component */
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    /* Iterate query, print values of Position */
    printf("-- First iteration\n");
    print_query(q);

    /* Change the value of one entity, invalidating the order */
    ecs_set(world, e, Position, {7, 0});

    /* Iterate query again, printed values are still ordered */
    printf("\n-- Second iteration\n");
    print_query(q);    

    /* Cleanup */
    return ecs_fini(world);
}
