#include <core.h>

void Reference_setup(void) {
    ecs_log_set_level(-3);
}

void Reference_get_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_add(world, e, Velocity);

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_add(world, e, Velocity);
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Velocity);

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t dummy = ecs_new_w(world, Position);
    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_delete(world, dummy);

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_after_realloc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    int i;
    for (i = 0; i < 1000; i ++) {
        // Creating lots of entities which will trigger allocations
        ecs_new_w(world, Position);
    }

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_staged(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_defer_begin(world);

    /* ecs_set() makes immediate changes */
    ecs_set(world, e, Position, {30, 40});

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_defer_end(world);

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Reference_get_ref_after_new_in_stage(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_defer_begin(world);

    ecs_new_w(world, Position);

    /* ecs_set() makes immediate changes */
    ecs_set(world, e, Position, {30, 40});

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_defer_end(world);

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Reference_get_ref_monitored(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_new_w_pair(world, EcsChildOf, e);
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_nonexisting(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    
    ecs_ref_t ref = ecs_ref_init(world, e, Velocity);
    const Velocity *p = ecs_ref_get(world, &ref, Velocity);
    test_assert(p == NULL);

    ecs_fini(world);
}

static ECS_MOVE(Position, dst, src, {
    ecs_os_memcpy_t(dst, src, Position);
})

void Reference_get_ref_after_realloc_w_lifecycle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position)
    });

    ECS_ENTITY(world, e, Position);
    ECS_ENTITY(world, e2, Position);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    for (int i = 0; i < 1000; i ++) {
        ECS_ENTITY(world, e, Position);
    }

    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reference_get_ref_w_low_id_tag(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_component_init(world, &(ecs_component_desc_t){
        .type.size = 0,
        .type.alignment = 0
    });

    ecs_entity_t comp = ecs_component_init(world, &(ecs_component_desc_t){
        .type.size = 4,
        .type.alignment = 4
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, tag);
    ecs_add_id(world, e, comp);

    test_assert(ecs_get_id(world, e, comp) != NULL);

    ecs_ref_t ref = ecs_ref_init_id(world, e, comp);
    test_assert(ecs_ref_get_id(world, &ref, comp) == 
        ecs_get_id(world, e, comp));

    ecs_fini(world);
}

void Reference_get_ref_w_low_id_tag_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_component_init(world, &(ecs_component_desc_t){
        .type.size = 0,
        .type.alignment = 0
    });

    ecs_entity_t id = ecs_new_low_id(world);

    ecs_entity_t comp = ecs_component_init(world, &(ecs_component_desc_t){
        .type.size = 4,
        .type.alignment = 4
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, tag);
    ecs_add_id(world, e, comp);

    test_assert(ecs_get_id(world, e, comp) != NULL);

    ecs_ref_t ref = ecs_ref_init_id(world, e, comp);

    ecs_add_id(world, e, id);

    test_assert(ecs_ref_get_id(world, &ref, comp) == 
        ecs_get_id(world, e, comp));

    ecs_fini(world);
}

void Reference_aba_table(void) {
  ecs_world_t *world = ecs_mini();

  ECS_COMPONENT(world, Position);

  ecs_entity_t t1 = ecs_new(world);
  ecs_entity_t t2 = ecs_new(world);
  ecs_entity_t t3 = ecs_new(world);
  ecs_entity_t e = ecs_new(world);

  ecs_set_pair(world, e, Position, t1, {10, 20});
  ecs_set_pair(world, e, Position, t2, {20, 30});

  ecs_ref_t r = ecs_ref_init_id(world, e, ecs_pair_t(Position, t2));

  ecs_delete(world, t1);
  ecs_set_pair(world, e, Position, t3, {30, 40});

  Position *p = ecs_ref_get_id(world, &r, ecs_pair_t(Position, t2));
  test_assert(p != NULL);
  test_int(p->x, 20);
  test_int(p->y, 30);
  
  ecs_fini(world);
}
