#include <prefab_variant.h>

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create a BasePrefab from which will be specialized by other prefabs */
    ECS_PREFAB(world, BasePrefab, Position);
    ecs_set(world, BasePrefab, Position, {10, 20});

    /* Create two prefab specializations. This uses the same inheritance
     * mechanism as is used with regular entities. */
    ECS_PREFAB(world, SubPrefab1, (IsA, BasePrefab), Velocity);
    ecs_set(world, SubPrefab1, Velocity, {30, 40});

    ECS_PREFAB(world, SubPrefab2, (IsA, BasePrefab), Velocity);
    ecs_set(world, SubPrefab2, Velocity, {50, 60});

    /* Create two types for SubPrefab1 and SubPrefab2 which automatically
     * override the component values. The Position component will be overridden
     * from the BasePrefab, while Velocity will be overridden from SubPrefab1
     * and SubPrefab2 respectively. */
    ECS_TYPE(world, Sub1, (IsA, SubPrefab1), Position, Velocity);
    ECS_TYPE(world, Sub2, (IsA, SubPrefab2), Position, Velocity);

    /* Create new entities from Sub1 and Sub2 */
    ecs_entity_t e1 = ecs_new(world, Sub1);
    ecs_entity_t e2 = ecs_new(world, Sub2);

    /* Print values of e1 */
    const Position *p = ecs_get(world, e1, Position);
    const Velocity *v = ecs_get(world, e1, Velocity);
    printf("e1 Position: {%f, %f} Velocity: {%f, %f}\n", p->x, p->y, v->x, v->y);

    /* Print values of e2 */
    p = ecs_get(world, e2, Position);
    v = ecs_get(world, e2, Velocity);
    printf("e2 Position: {%f, %f} Velocity: {%f, %f}\n", p->x, p->y, v->x, v->y);

    /* Cleanup */
    return ecs_fini(world);
}
