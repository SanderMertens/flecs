#include <api.h>

void Reference_setup() {
    ecs_tracing_enable(-3);
}

void Reference_get_ref() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_add(world, e, Velocity);

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    ecs_add(world, e, Velocity);
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Velocity);

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t dummy = ecs_new(world, Position);
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_delete(world, dummy);

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_realloc() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    int i;
    for (i = 0; i < 1000; i ++) {
        // Creating lots of entities which will trigger allocations
        ecs_new(world, Position);
    }

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_staged() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_defer_begin(world);

    ecs_set(world, e, Position, {30, 40});

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_defer_end(world);

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Reference_get_ref_after_new_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_defer_begin(world);

    ecs_new(world, Position);

    ecs_set(world, e, Position, {30, 40});

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_defer_end(world);

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Reference_get_ref_monitored() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    ecs_new_w_pair(world, EcsChildOf, e);
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_nonexisting() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    
    ecs_ref_t ref = {0};
    const Velocity *p = ecs_get_ref(world, &ref, e, Velocity);
    test_assert(p == NULL);

    ecs_fini(world);
}

static
void comp_move(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    memcpy(dst_ptr, src_ptr, size * count);
}

void Reference_get_ref_after_realloc_w_lifecycle() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .move = comp_move
    });

    ECS_ENTITY(world, e, Position);
    ECS_ENTITY(world, e2, Position);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_ref_t ref = {0};
    const Position *p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    for (int i = 0; i < 1000; i ++) {
        ECS_ENTITY(world, e, Position);
    }

    p = ecs_get_ref(world, &ref, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}
