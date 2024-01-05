#include <basics.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, Walking);

    // Create an entity with name Bob
    ecs_entity_t bob = ecs_set_name(ecs, 0, "Bob");

    // The set operation finds or creates a component, and sets it.
    ecs_set(ecs, bob, Position, {10, 20});
    // The add operation adds a component without setting a value. This is
    // useful for tags, or when adding a component with its default value.    
    ecs_add(ecs, bob, Walking);

    // Get the value for the Position component
    const Position *ptr = ecs_get(ecs, bob, Position);
    printf("{%f, %f}\n", ptr->x, ptr->y);

    // Overwrite the value of the Position component
    ecs_set(ecs, bob, Position, {20, 30});

    // Create another named entity
    ecs_entity_t alice = ecs_set_name(ecs, 0, "Alice");
    ecs_set(ecs, alice, Position, {10, 20});
    ecs_add(ecs, alice, Walking);

    // Print all the components the entity has. This will output:
    //    Position, Walking, (Identifier,Name)
    char *str = ecs_type_str(ecs, ecs_get_type(ecs, alice));
    printf("[%s]\n", str);
    ecs_os_free(str);

    // Remove tag
    ecs_remove(ecs, alice, Walking);

    // Iterate all entities with Position
    ecs_iter_t it = ecs_term_iter(ecs, &(ecs_term_t){ .id = ecs_id(Position) });
    while (ecs_term_next(&it)) {
        Position *p = ecs_field(&it, Position, 1);
        for (int i = 0; i < it.count; i ++) {
            printf("%s: {%f, %f}\n", ecs_get_name(ecs, it.entities[i]),
                p[i].x, p[i].y);
        }
    }

    return ecs_fini(ecs);

    // Output
    //  {10.000000, 20.000000}
    //  [Position, Walking, (Identifier,Name)]
    //  Alice: {10.000000, 20.000000}
    //  Bob: {20.000000, 30.000000}
}
