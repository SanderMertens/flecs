#include <auto_override_nested_prefab.h>

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

    /* Create root prefab */
    ECS_PREFAB(world, RootPrefab, Position);
        ecs_set(world, RootPrefab, Position, {10, 20});

        /* Create a standalone ChildPrefab that is not a child of RootPrefab.
         * This will cause it to not be instantiated when RootPrefab is
         * is instantiated. */
        ECS_PREFAB(world, ChildPrefab, Position);
            ecs_set(world, ChildPrefab, Position, {30, 40});

        /* Create a prefab that is an instance of ChildPrefab that overrides the
         * Position component. When RootPrefab is instantiated, it will have a
         * child with the exact same type as Child, meaning that it will
         * automatically override Position. */
        ECS_PREFAB(world, Child, (ChildOf, RootPrefab), (IsA, ChildPrefab), Position);

    /* Create type that automatically overrides Position from RootPrefab */
    ECS_TYPE(world, Root, (IsA, RootPrefab), Position);

    /* Create new entity from Root. Don't use EcsIsA, as we're using a
     * regular type which already has the IsA relationship. */
    ecs_entity_t e = ecs_new(world, Root);

    /* Lookup child in the instance we just created. This child will have e in
     * its type with a ChildOf pair, and the prefab ChildPrefab in its type with
     * an IsA pair. Note how the identifier is Child, not ChildPrefab. */
    ecs_entity_t child = ecs_lookup_child(world, e, "Child");
    printf("Child type = [%s]\n", ecs_type_str(world, ecs_get_type(world, child)));

    /* Print position of e and of the child. Note that since types were used to
     * automatically override the components, the components are owned by both
     * e and child. */
    const Position *p = ecs_get(world, e, Position);
    printf("Position of e = {%f, %f} (owned = %d)\n", p->x, p->y, ecs_owns(world, e, Position, true));

    p = ecs_get(world, child, Position);
    printf("Position of Child = {%f, %f} (owned = %d)\n", p->x, p->y, ecs_owns(world, child, Position, true));

    /* Cleanup */
    return ecs_fini(world);
}
