#include <nested_prefab.h>

/* Component types */
typedef struct {
    double x, y;
} Position;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);

    /* Create a prefab with a child entity. When this prefab is instantiated, 
     * the child will be instantiated too as a child of the instance.  */
    ECS_PREFAB(world, RootPrefab, Position);
        ecs_set(world, RootPrefab, Position, {10, 20});

        /* The child needs to explicitly set the parent in the EcsPrefab
         * component. This is needed for Flecs to register the child with the
         * parent prefab. */
        ECS_PREFAB(world, Child, (ChildOf, RootPrefab), Position);
            ecs_set(world, Child, Position, {30, 40});

    /* Create instance of root */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, RootPrefab);

    /* Lookup child in the instance we just created. This child will have e in
     * its type with a ChildOf pair, and the prefab Child in its type with an
     * IsA pair. */
    ecs_entity_t child = ecs_lookup_child(world, e, "Child");
    printf("Child type = [%s]\n", ecs_type_str(world, ecs_get_type(world, child)));

    /* Print position of e and of the child. Note that since we did not override
     * any components, both position components are owned by the prefabs, not by
     * the instances. */
    const Position *p = ecs_get(world, e, Position);
    printf("Position of e = {%f, %f}\n", p->x, p->y);

    p = ecs_get(world, child, Position);
    printf("Position of Child = {%f, %f}\n", p->x, p->y);

    /* Cleanup */
    return ecs_fini(world);
}
