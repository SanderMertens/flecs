#include <parent_hierarchy.h>
#include <stdio.h>

// ChildOf hierarchies are optimized for scenarios with few parents that have
// lots of children. Parent hierarchies are optimized for scenarios with lots of
// parents that have a small number of children. See the documentation for more
// information:
//   https://www.flecs.dev/flecs/md_docs_2HierarchiesManual.html#hierarchy-storage
//
// This example is the same as the hierarchy example, but instead of using
// ChildOf hierarchies it uses Parent hierarchies.

typedef struct {
    double x, y;
} Position;

// Forward declare component so we can use it from functions other than main
ECS_COMPONENT_DECLARE(Position);

void iterate_tree(ecs_world_t *world, ecs_entity_t e, Position p_parent) {
    // Print hierarchical name of entity & the entity type
    char *path_str = ecs_get_path(world, e);
    char *type_str = ecs_type_str(world, ecs_get_type(world, e));
    printf("%s [%s]\n", path_str, type_str);
    ecs_os_free(type_str);
    ecs_os_free(path_str);

    // Get entity position
    const Position *ptr = ecs_get(world, e, Position);
    
    // Calculate actual position
    Position p_actual = {ptr->x + p_parent.x, ptr->y + p_parent.y};
    printf("{%f, %f}\n\n", p_actual.x, p_actual.y);

    // Iterate children recursively
    ecs_iter_t it = ecs_children(world, e);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            iterate_tree(world, it.entities[i], p_actual);
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Star);
    ECS_TAG(world, Planet);
    ECS_TAG(world, Moon);

    // Create a simple hierarchy.
    // Use the ecs_new_w_parent function to create entities that use Parent
    // hierarchies.
    //
    // Entities can be reparented afterwards by setting the Parent component:
    //   ecs_set(world, moon, EcsParent, { mars });

    ecs_entity_t sun = ecs_entity(world, { .name = "Sun" });
    ecs_add(world, sun, Star);
    ecs_set(world, sun, Position, {1, 1});

        ecs_entity_t mercury = ecs_new_w_parent(world, sun, "Mercury");
        ecs_add(world, mercury, Planet);
        ecs_set(world, mercury, Position, {1, 1});

        ecs_entity_t venus = ecs_new_w_parent(world, sun, "Venus");
        ecs_add(world, venus, Planet);
        ecs_set(world, venus, Position, {2, 2});

        ecs_entity_t earth = ecs_new_w_parent(world, sun, "Earth");
        ecs_add(world, earth, Planet);
        ecs_set(world, earth, Position, {3, 3});

            ecs_entity_t moon = ecs_new_w_parent(world, earth, "Moon");
            ecs_add(world, moon, Moon);
            ecs_set(world, moon, Position, {0.1, 0.1});

    // Is the Moon a child of Earth?
    printf("Child of Earth? %d\n\n", ecs_has_pair(world, moon, EcsChildOf, earth));

    // Lookup moon by name
    ecs_entity_t e = ecs_lookup(world, "Sun.Earth.Moon");
    char *path = ecs_get_path(world, e);
    printf("Moon found: %s\n\n", path);
    ecs_os_free(path);

    // Do a depth-first walk of the tree
    iterate_tree(world, sun, (Position){0, 0});

    return ecs_fini(world);

    // Output:
    //   Child of Earth? 1
    //   
    //   Moon found: Sun.Earth.Moon
    //   
    //   Sun [Position, Star, (Identifier,Name)]
    //   {1.000000, 1.000000}
    //   
    //   Sun.Mercury [Parent, Position, Planet, (Identifier,Name), (ParentDepth,@1)]
    //   {2.000000, 2.000000}
    //   
    //   Sun.Venus [Parent, Position, Planet, (Identifier,Name), (ParentDepth,@1)]
    //   {3.000000, 3.000000}
    //   
    //   Sun.Earth [Parent, Position, Planet, (Identifier,Name), (ParentDepth,@1)]
    //   {4.000000, 4.000000}
    //   
    //   Sun.Earth.Moon [Parent, Position, Moon, (Identifier,Name), (ParentDepth,@2)]
    //   {4.100000, 4.100000}
}
