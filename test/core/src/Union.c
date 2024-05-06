#include <core.h>

void Union_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Likes, Union);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Likes, Apples));

    ecs_add_pair(world, e, Likes, Apples);
    test_assert(ecs_has_pair(world, e, Likes, Apples));

    ecs_fini(world);
}

void Union_add_twice(void) {
    // Implement testcase
}

void Union_add_replace(void) {
    // Implement testcase
}

void Union_add_remove(void) {
    // Implement testcase
}

void Union_add_remove_recycled(void) {
    // Implement testcase
}

void Union_add_remove_add(void) {
    // Implement testcase
}

void Union_get_target_none(void) {
    // Implement testcase
}

void Union_get_target(void) {
    // Implement testcase
}

void Union_get_recycled_target(void) {
    // Implement testcase
}

void Union_get_target_after_replace(void) {
    // Implement testcase
}

void Union_get_target_after_remove(void) {
    // Implement testcase
}

void Union_has_wildcard(void) {
    // Implement testcase
}

void Union_has_any(void) {
    // Implement testcase
}
