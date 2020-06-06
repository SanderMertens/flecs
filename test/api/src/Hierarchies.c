#include <api.h>

void Hierarchies_delete_children() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t child = ecs_new_w_entity(world, ECS_CHILDOF | e);
    test_assert(child != 0);

    ecs_delete(world, e);

    test_assert(ecs_get_type(world, child) == NULL);

    ecs_fini(world);
}

void Hierarchies_tree_iter_empty() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    ecs_view_t view = ecs_tree_iter(world, Parent);
    test_assert( !ecs_tree_next(&view));

    ecs_fini(world);
}

void Hierarchies_tree_iter_1_table() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    ECS_ENTITY(world, Child1, CHILDOF | Parent);
    ECS_ENTITY(world, Child2, CHILDOF | Parent);
    ECS_ENTITY(world, Child3, CHILDOF | Parent);

    ecs_view_t view = ecs_tree_iter(world, Parent);
    test_assert( ecs_tree_next(&view) == true);
    test_int( view.count, 3);

    test_assert(view.entities[0] == Child1);
    test_assert(view.entities[1] == Child2);
    test_assert(view.entities[2] == Child3);

    test_assert( !ecs_tree_next(&view));

    ecs_fini(world);
}

void Hierarchies_tree_iter_2_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, 0);

    ECS_ENTITY(world, Child1, CHILDOF | Parent);
    ECS_ENTITY(world, Child2, CHILDOF | Parent);
    ECS_ENTITY(world, Child3, CHILDOF | Parent, Position);
    ECS_ENTITY(world, Child4, CHILDOF | Parent, Position);

    ecs_view_t view = ecs_tree_iter(world, Parent);
    test_assert( ecs_tree_next(&view) == true);
    test_int( view.count, 2);
    test_assert(view.entities[0] == Child1);
    test_assert(view.entities[1] == Child2);

    test_assert( ecs_tree_next(&view) == true);
    test_int( view.count, 2);
    test_assert(view.entities[0] == Child3);
    test_assert(view.entities[1] == Child4);

    test_assert( !ecs_tree_next(&view));

    ecs_fini(world);
}
