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

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ECS_SYSTEM(world, IncSingleton, EcsOnUpdate, Position);

    ecs_singleton_set(world, Position, {10, 20});

    ecs_progress(world, 0);

    const Position *p = ecs_singleton_get(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);

    ecs_fini(world);    
}

void Singleton_trait_add_singleton_to_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ecs_singleton_set(world, Position, {10, 20});

    const Position *p = ecs_singleton_get(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Singleton_trait_add_singleton_pair_to_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ecs_set_pair(world, ecs_id(Position), Position, TgtA, {10, 20});
    ecs_set_pair(world, ecs_id(Position), Position, TgtB, {30, 40});

    {
        const Position *p = ecs_get_pair(
            world, ecs_id(Position), Position, TgtA);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get_pair(
            world, ecs_id(Position), Position, TgtB);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void Singleton_trait_add_singleton_to_other(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ecs_entity_t e = ecs_new(world);

    test_expect_abort();
    ecs_set(world, e, Position, {10, 20});
}

void Singleton_trait_add_singleton_pair_to_other(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);

    ecs_add_id(world, ecs_id(Position), EcsSingleton);
    
    ecs_entity_t e = ecs_new(world);

    test_expect_abort();
    ecs_set_pair(world, e, Position, TgtA, {10, 20});
}

void Singleton_add_trait_after_in_use(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    test_expect_abort();
    ecs_add_id(world, ecs_id(Position), EcsSingleton);
}

void Singleton_add_trait_after_pair_in_use(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tgt);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);

    ecs_entity_t e = ecs_new(world);
    ecs_set_pair(world, e, Position, Tgt, {10, 20});

    test_expect_abort();
    ecs_add_id(world, ecs_id(Position), EcsSingleton);
}
