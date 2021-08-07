#include <nested_variant.h>

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

    /* Create a base prefab which will be inherited from by a child prefab */
    ECS_PREFAB(world, BasePrefab, Position);
        ecs_set(world, BasePrefab, Position, {10, 20});

    /* Create the root of the prefab hierarchy  */
    ECS_PREFAB(world, RootPrefab, Position);
        ecs_set(world, RootPrefab, Position, {10, 20});

        /* Create two child prefabs that inherit from BasePrefab */
        ECS_PREFAB(world, Child1, (ChildOf, RootPrefab), (IsA, BasePrefab), Velocity);
            ecs_set(world, Child1, Velocity, {30, 40});

        ECS_PREFAB(world, Child2, (ChildOf, RootPrefab), (IsA, BasePrefab), Velocity);
            ecs_set(world, Child2, Velocity, {50, 60});

    /* Create instance of RootPrefab */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, RootPrefab);

    /* Print types of child1 and child2 */
    ecs_entity_t child1 = ecs_lookup_child(world, e, "Child1");
    printf("Child1 type = [%s]\n", ecs_type_str(world, ecs_get_type(world, child1)));

    ecs_entity_t child2 = ecs_lookup_child(world, e, "Child2");
    printf("Child2 type = [%s]\n", ecs_type_str(world, ecs_get_type(world, child2)));

    /* e shares Position from RootPrefab */
    const Position *p = ecs_get(world, e, Position);
    printf("Position of e = {%f, %f}\n", p->x, p->y);

    /* Children will share Position from ChildBase and Velocity from the Child1
     * and Child2 prefabs respectively */
    p = ecs_get(world, child1, Position);
    const Velocity *v = ecs_get(world, child1, Velocity);
    printf("Child1 Position = {%f, %f}, Velocity = {%f, %f}\n", p->x, p->y, v->x, v->y);

    p = ecs_get(world, child2, Position);
    v = ecs_get(world, child2, Velocity);
    printf("Child2 Position = {%f, %f}, Velocity = {%f, %f}\n", p->x, p->y, v->x, v->y);

    /* Cleanup */
    return ecs_fini(world);
}
