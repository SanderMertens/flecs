#include <iterate_components.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position, Velocity;

void iterate_components(ecs_world_t *ecs, ecs_entity_t e) {
    // First get the entity's type, which is a vector of (component) ids.
    const ecs_type_t *type = ecs_get_type(ecs, e);

    // 1. The easiest way to print the components is to use ecs_type_str
    char *type_str = ecs_type_str(ecs, type);
    printf("ecs_type_str: %s\n\n", type_str);
    ecs_os_free(type_str);

    // 2. To print individual ids, iterate the type array with ecs_id_str
    ecs_id_t *type_ids = type->array;
    int32_t i, count = type->count;

    for (i = 0; i < count; i ++) {
        ecs_id_t id = type_ids[i];
        char *id_str = ecs_id_str(ecs, id);
        printf("%d: %s\n", i, id_str);
        ecs_os_free(id_str);
    }

    printf("\n");

    // 3. we can also inspect and print the ids in our own way. This is a
    // bit more complicated as we need to handle the edge cases of what can be
    // encoded in an id, but provides the most flexibility.
    for (i = 0; i < count; i ++) {
        ecs_id_t id = type_ids[i];

        printf("%d: ", i);
        
        ecs_id_t role = id & ECS_ROLE_MASK;
        if (role) {
            printf("role: %s, ", ecs_role_str(role));
        }

        if (ECS_HAS_ROLE(id, PAIR)) { // See relations
            ecs_entity_t rel = ecs_pair_first(ecs, id);
            ecs_entity_t obj = ecs_pair_second(ecs, id);
            printf("rel: %s, obj: %s",
                ecs_get_name(ecs, rel), ecs_get_name(ecs, obj));
        } else {
            ecs_entity_t comp = id & ECS_COMPONENT_MASK;
            printf("entity: %s", ecs_get_name(ecs, comp));
        }

        printf("\n");
    }

    printf("\n\n");
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Ordinary components
    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    // A tag
    ECS_TAG(ecs, Human);

    // Two tags used to create a pair
    ECS_TAG(ecs, Eats);
    ECS_TAG(ecs, Apples);

    // Create an entity which all of the above
    ecs_entity_t Bob = ecs_new_id(ecs);

    ecs_set(ecs, Bob, Position, {10, 20});
    ecs_set(ecs, Bob, Velocity, {1, 1});
    ecs_add(ecs, Bob, Human);
    ecs_add_pair(ecs, Bob, Eats, Apples);

    // Iterate & components of Bob
    printf("Bob's components:\n");
    iterate_components(ecs, Bob);

    // We can use the same function to iterate the components of a component
    printf("Position's components:\n");
    iterate_components(ecs, ecs_id(Position));

    return ecs_fini(ecs);
}

// Output:

// Bob's components:
// ecs_type_str: Position,Velocity,Human,(Eats,Apples)

// 0: Position
// 1: Velocity
// 2: Human
// 3: (Eats,Apples)

// 0: entity: Position
// 1: entity: Velocity
// 2: entity: Human
// 3: role: PAIR, rel: Eats, obj: Eats


// Position's components:
// ecs_type_str: EcsComponent,(Identifier,Name),(Identifier,Symbol),(OnDelete,Panic)

// 0: Component
// 1: (Identifier,Name)
// 2: (Identifier,Symbol)
// 3: (OnDelete,Panic)

// 0: entity: Component
// 1: role: PAIR, rel: Identifier, obj: Identifier
// 2: role: PAIR, rel: Identifier, obj: Identifier
// 3: role: PAIR, rel: OnDelete, obj: OnDelete
