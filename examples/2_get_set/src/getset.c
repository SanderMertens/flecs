#include <include/getset.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, 0);

    /* Add & initialize Position component */
    ecs_set(world, e, Position, {.x = 10, .y = 20});

    /* Get value of component */
    Position p = ecs_get(world, e, Position);
    printf("Position = %d, %d\n", p.x, p.y);

    /* Set value using set_ptr */
    p.x = 15;
    p.y = 25;
    ecs_set_ptr(world, e, Position_h, &p);

    /* Get value with get_ptr */
    Position *p_ptr = ecs_get_ptr(world, e, Position_h);
    printf("Position = %d, %d\n", p_ptr->x, p_ptr->y);

    return ecs_fini(world);
}
