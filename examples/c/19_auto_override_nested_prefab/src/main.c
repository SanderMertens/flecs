#include <auto_override_nested_prefab.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);

    /* Create root prefab */
    ECS_PREFAB(world, RootPrefab, Position);
        ecs_set(world, RootPrefab, Position, {10, 20});

        /* Create child prefab. Instead of adding the child directly to
         * RootPrefab, create a type that overrides the components from the
         * ChildPrefab. This ensures that when the prefab is instantiated, the
         * components from the child prefab are owned by the instance. */
        ECS_PREFAB(world, ChildPrefab, Position);
        ECS_TYPE(world, Child, INSTANCEOF | ChildPrefab, Position);
            /* Instead of the ChildPrefab, add the Child type to RootPrefab */
            ecs_set(world, Child, EcsPrefab, {.parent = RootPrefab});
            ecs_set(world, ChildPrefab, Position, {30, 40});

    /* Create type that automatically overrides Position from RootPrefab */
    ECS_TYPE(world, Root, INSTANCEOF | RootPrefab, Position);

    /* Create new entity from Root. Don't use ecs_new_instance, as we're using a
     * regular type which already has the INSTANCEOF relationship. */
    ecs_entity_t e = ecs_new(world, Root);

    /* Lookup child in the instance we just created. This child will have e in
     * its type with a CHILDOF mask, and the prefab ChildPrefab in its type with
     * an INSTANCEOF mask. Note how the identifier is Child, not ChildPrefab. */
    ecs_entity_t child = ecs_lookup_child(world, e, "Child");
    printf("Child type = [%s]\n", ecs_type_to_expr(world, ecs_get_type(world, child)));

    /* Print position of e and of the child. Note that since types were used to
     * automatically override the components, the components are owned by both
     * e and child. */
    Position *p = ecs_get_ptr(world, e, Position);
    printf("Position of e = {%f, %f} (owned = %d)\n", p->x, p->y, ecs_has_owned(world, e, Position));

    p = ecs_get_ptr(world, child, Position);
    printf("Position of Child = {%f, %f} (owned = %d)\n", p->x, p->y, ecs_has_owned(world, child, Position));

    /* Cleanup */
    return ecs_fini(world);
}
