#include <api.h>

void Queries_query_changed_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_new(world, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_delete(world, e1);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_add(world, e1, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_remove(world, e1, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_set(world, e1, Position, {10, 20});
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Sys(ecs_iter_t *it) { }

void Queries_query_change_after_out_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [out] Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_progress(world, 0);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_change_after_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [in] Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_progress(world, 0);
    test_assert(ecs_query_changed(q) == false);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}
