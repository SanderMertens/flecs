#include <disable_component.h>

typedef struct {
    double x, y;
} Position;

void PrintPosition(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    Position *p = ecs_term(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("%s: {%f, %f}\n", 
            ecs_get_name(world, it->entities[i]), p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, PrintPosition, 0, Position);

    ECS_ENTITY(world, e1, 0);
    ECS_ENTITY(world, e2, 0);
    ECS_ENTITY(world, e3, 0);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    printf("e1 enabled: %d\n", ecs_is_component_enabled(world, e1, Position));
    printf("e2 enabled: %d\n", ecs_is_component_enabled(world, e2, Position));
    printf("e3 enabled: %d\n", ecs_is_component_enabled(world, e3, Position));

    printf("\n1st run: all components enabled\n");
    ecs_run(world, PrintPosition, 0, NULL);

    /* Enable component of entity 1, disable component of entity 2 */
    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, false);

    /* e2 will now show up as disabled */
    printf("\n");
    printf("e1 enabled: %d\n", ecs_is_component_enabled(world, e1, Position));
    printf("e2 enabled: %d\n", ecs_is_component_enabled(world, e2, Position));
    printf("e3 enabled: %d\n", ecs_is_component_enabled(world, e3, Position));    

    printf("\n2nd run: e2 is disabled\n");
    ecs_run(world, PrintPosition, 0, NULL);

    /* Print types. Both e1 and e2 will have DISABLED|Position. This does not
     * actually mean that both are disabled. Instead it means that both entities
     * have a bitset that tracks whether the component is disabled. */
    char *type = NULL;
    printf("\n");
    printf("e1 type: %s\n", type = ecs_type_str(world, ecs_get_type(world, e1))); free(type);
    printf("e2 type: %s\n", type = ecs_type_str(world, ecs_get_type(world, e2))); free(type);
    printf("e3 type: %s\n", type = ecs_type_str(world, ecs_get_type(world, e3))); free(type);

    ecs_fini(world);
}

