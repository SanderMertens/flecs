#include <core.h>

void Singleton_add_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    test_assert(!ecs_has(world, TagA, TagA));

    ecs_singleton_add(world, TagA);

    test_assert(ecs_has(world, TagA, TagA));

    ecs_fini(world);
}

void Singleton_remove_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    test_assert(!ecs_has(world, TagA, TagA));

    ecs_singleton_add(world, TagA);

    test_assert(ecs_has(world, TagA, TagA));

    ecs_singleton_remove(world, TagA);

    test_assert(!ecs_has(world, TagA, TagA));

    ecs_fini(world);
}

void Singleton_set_get_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_singleton_set(world, Position, {10, 20});
    
    const Position *p = ecs_singleton_get(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Singleton_get_mut_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_singleton_set(world, Position, {10, 20});
    
    Position *p = ecs_singleton_get_mut(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Singleton_ensure_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Position *p_mut = ecs_singleton_ensure(world, Position);
    p_mut->x = 10;
    p_mut->y = 20;
    
    const Position *p = ecs_singleton_get(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Singleton_emplace_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    bool is_new = false;
    {
        Position *p = ecs_singleton_emplace(world, Position, &is_new);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
        test_assert(is_new);
    }

    {
        Position *p = ecs_singleton_emplace(world, Position, &is_new);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_assert(!is_new);
    }

    ecs_fini(world);
}

static
void IncSingleton(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    p->x ++;
    p->y ++;
}

void Singleton_singleton_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, IncSingleton, EcsOnUpdate, Position($));

    ecs_singleton_set(world, Position, {10, 20});

    ecs_progress(world, 0);

    const Position *p = ecs_singleton_get(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);

    ecs_fini(world);    
}
