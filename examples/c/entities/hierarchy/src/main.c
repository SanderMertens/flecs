#include <hierarchy.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position;

// Forward declare component so we can use it from functions other than main
ECS_COMPONENT_DECLARE(Position);

void iterate_tree(ecs_world_t *ecs, ecs_entity_t e, Position p_parent) {
    // Print hierarchical name of entity & the entity type
    char *path_str = ecs_get_fullpath(ecs, e);
    char *type_str = ecs_type_str(ecs, ecs_get_type(ecs, e));
    printf("%s [%s]\n", path_str, type_str);
    ecs_os_free(type_str);
    ecs_os_free(path_str);

    // Get entity position
    const Position *ptr = ecs_get(ecs, e, Position);
    
    // Calculate actual position
    Position p_actual = {ptr->x + p_parent.x, ptr->y + p_parent.y};
    printf("{%f, %f}\n\n", p_actual.x, p_actual.y);

    // Iterate children recursively
    ecs_iter_t it = ecs_children(ecs, e);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            iterate_tree(ecs, it.entities[i], p_actual);
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT_DEFINE(ecs, Position);
    ECS_TAG(ecs, Star);
    ECS_TAG(ecs, Planet);
    ECS_TAG(ecs, Moon);

    // Create a simple hierarchy.
    // Hierarchies use ECS relationships and the builtin flecs::ChildOf relationship to
    // create entities as children of other entities.

    ecs_entity_t sun = ecs_new_entity(ecs, "Sun");
    ecs_add(ecs, sun, Star);
    ecs_set(ecs, sun, Position, {1, 1});

        ecs_entity_t mercury = ecs_new_entity(ecs, "Mercury");
        ecs_add_pair(ecs, mercury, EcsChildOf, sun);
        ecs_add(ecs, mercury, Planet);
        ecs_set(ecs, mercury, Position, {1, 1});

        ecs_entity_t venus = ecs_new_entity(ecs, "Venus");
        ecs_add_pair(ecs, venus, EcsChildOf, sun);
        ecs_add(ecs, venus, Planet);
        ecs_set(ecs, venus, Position, {2, 2});

        ecs_entity_t earth = ecs_new_entity(ecs, "Earth");
        ecs_add_pair(ecs, earth, EcsChildOf, sun);
        ecs_add(ecs, earth, Planet);
        ecs_set(ecs, earth, Position, {3, 3});

            ecs_entity_t moon = ecs_new_entity(ecs, "Moon");
            ecs_add_pair(ecs, moon, EcsChildOf, earth);
            ecs_add(ecs, moon, Moon);
            ecs_set(ecs, moon, Position, {0.1, 0.1});

    // Is the Moon a child of Earth?
    printf("Child of Earth? %d\n", ecs_has_pair(ecs, moon, EcsChildOf, earth));

    // Do a depth-first walk of the tree
    iterate_tree(ecs, sun, (Position){0, 0});

    return ecs_fini(ecs);
}
