#include <core.h>

static ecs_id_t NamePair;

static
ECS_CTOR(Position, ptr, {
    ptr->x = 15;
    ptr->y = 25;
});

static
void Position_custom_copy(void *dst, const void *src, int32_t count, const ecs_type_info_t *ti) {
    test_int(count, 1);
    ((Position*)dst)->x = ((Position*)src)->x + 1;
    ((Position*)dst)->y = ((Position*)src)->y + 1;
}

static
ECS_ON_ADD(Position, ptr, {
    ecs_os_zeromem(ptr);
});

static
ECS_ON_SET(Position, ptr, {
    ptr->x ++;
    ptr->y ++;
});

void Prefab_setup(void) {
    NamePair = ecs_pair(ecs_id(EcsIdentifier), EcsName);
}

static
void Iter(ecs_iter_t *it) {
    Mass *m_ptr = ecs_field(it, Mass, 0);
    bool shared = !ecs_field_is_self(it, 0);

    Position *p = ecs_field(it, Position, 1);

    Velocity *v = NULL;
    if (it->field_count >= 3) {
        v = ecs_field(it, Velocity, 2);
    }

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        Mass m = 1;
        if (m_ptr) {
            if (shared) {
                m = *m_ptr;
            } else {
                m = m_ptr[i];
            }
        }
        
        p[i].x = 10 * m;
        p[i].y = 20 * m;

        if (v) {
            v[i].x = 30 * m;
            v[i].y = 40 * m;
        }
    }
}

void Prefab_new_w_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    ecs_add(world, e1, Velocity);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has_pair(world, e2, EcsIsA, Prefab));
    test_assert( !ecs_has_id(world, e2, EcsPrefab));
    test_assert( !ecs_has_id(world, e2, NamePair));
    test_assert( !ecs_get_id(world, e2, NamePair));

    ecs_add(world, e2, Velocity);
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has_pair(world, e2, EcsIsA, Prefab));
    test_assert( !ecs_has_id(world, e2, EcsPrefab));
    test_assert( !ecs_has_id(world, e2, NamePair));
    test_assert( !ecs_get_id(world, e2, NamePair));

    const Position *p_1 = ecs_get(world, e1, Position);
    const Position *p_2 = ecs_get(world, e2, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Velocity *v_1 = ecs_get(world, e1, Velocity);
    const Velocity *v_2 = ecs_get(world, e2, Velocity);
    
    test_assert(v_1 != NULL);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);

    ecs_fini(world);
}

void Prefab_new_w_count_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    const ecs_entity_t *ids = ecs_bulk_new_w_id(world, ecs_pair(EcsIsA, Prefab), 10);
    test_assert(ids != NULL);

    ecs_entity_t i;
    const Position *p_prev = NULL;
    for (i = 0; i < 10; i ++) {
        ecs_entity_t e = ids[i];
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);

        if (p_prev) test_ptr(p, p_prev);
        test_int(p->x, 10);
        test_int(p->y, 20);

        p_prev = p;
    }

    ecs_fini(world);
}

void Prefab_new_w_type_w_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position, auto_override | Velocity);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));

    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has_pair(world, e2, EcsIsA, Prefab));
    test_assert( !ecs_has_id(world, e2, EcsPrefab));
    test_assert( !ecs_has_id(world, e2, NamePair));

    const Position *p_1 = ecs_get(world, e1, Position);
    const Position *p_2 = ecs_get(world, e2, Position);
    const Position *p_prefab = ecs_get(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_1 == p_prefab);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Velocity *v_1 = ecs_get(world, e1, Velocity);
    const Velocity *v_2 = ecs_get(world, e2, Velocity);
    
    test_assert(v_1 != NULL);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);

    ecs_fini(world);
}

void Prefab_add_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_w(world, Velocity);
    test_assert(e1 != 0);

    ecs_add_pair(world, e1, EcsIsA, Prefab);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));

    ecs_entity_t e2 = ecs_new_w(world, Velocity);
    test_assert(e2 != 0);

    ecs_add_pair(world, e2, EcsIsA, Prefab);
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has_pair(world, e2, EcsIsA, Prefab));
    test_assert( !ecs_has_id(world, e2, EcsPrefab));
    test_assert( !ecs_has_id(world, e2, NamePair));

    const Position *p_1 = ecs_get(world, e1, Position);
    const Position *p_2 = ecs_get(world, e2, Position);
    const Position *p_prefab = ecs_get(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_prefab == p_1);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Velocity *v_1 = ecs_get(world, e1, Velocity);
    const Velocity *v_2 = ecs_get(world, e2, Velocity);
    
    test_assert(v_1 != NULL);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);

    ecs_fini(world);
}

void Prefab_remove_prefab_after_new(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    ecs_remove_pair(world, e1, EcsIsA, Prefab);
    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    ecs_fini(world);
}

void Prefab_remove_prefab_after_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != 0);

    ecs_add_pair(world, e1, EcsIsA, Prefab);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    ecs_remove_pair(world, e1, EcsIsA, Prefab);
    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    ecs_fini(world);
}

void Prefab_override_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e1, Position, {20, 30});
    p = ecs_get(world, e1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    ecs_fini(world);
}

void Prefab_override_remove_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e1, Position, {20, 30});
    p = ecs_get(world, e1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    ecs_remove(world, e1, Position);
    p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_assert(p == p_prefab);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Prefab_override_2_of_3_components_1_self(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position, Velocity, Mass);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});
    ecs_set(world, Prefab, Mass, {50});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);

    ecs_set(world, e1, Rotation, {60});

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e1, Mass));
    test_assert( ecs_has(world, e1, Rotation));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e1, Position, {20, 30});
    p = ecs_get(world, e1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_set(world, e1, Velocity, {40, 50});
    v = ecs_get(world, e1, Velocity);
    test_int(v->x, 40);
    test_int(v->y, 50);

    const Velocity *v_prefab = ecs_get(world, Prefab, Velocity);
    test_assert(v != v_prefab);
    test_int(v_prefab->x, 30);
    test_int(v_prefab->y, 40);

    const Mass *m = ecs_get(world, e1, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    const Mass *m_prefab = ecs_get(world, Prefab, Mass);
    test_assert(m_prefab != NULL);
    test_ptr(m_prefab, m);

    const Rotation *r = ecs_get(world, e1, Rotation);
    test_assert(r != NULL);
    test_int(*r, 60);

    ecs_fini(world);
}

void Prefab_new_type_w_1_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position, Velocity, auto_override | Position);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));

    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    const Velocity *v_prefab = ecs_get(world, Prefab, Velocity);
    test_assert(v_prefab != NULL);
    test_assert(v == v_prefab);
    test_int(v_prefab->x, 30);
    test_int(v_prefab->y, 40);

    ecs_fini(world);
}

void Prefab_new_type_w_2_overrides(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position, Velocity, auto_override | Position, auto_override | Velocity);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));

    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_id(world, e1, EcsPrefab));
    test_assert( !ecs_has_id(world, e1, NamePair));
    test_assert( !ecs_get_id(world, e1, NamePair));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    const Velocity *v_prefab = ecs_get(world, Prefab, Velocity);
    test_assert(v_prefab != NULL);
    test_assert(v != v_prefab);
    test_int(v_prefab->x, 30);
    test_int(v_prefab->y, 40);

    ecs_fini(world);
}

void Prefab_get_ptr_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e1 != 0);
    test_assert( ecs_has_pair(world, e1, EcsIsA, Prefab));
    test_assert( ecs_get_id(world, e1, Prefab) == NULL);

    ecs_fini(world);
}

static
void Prefab_w_shared(ecs_iter_t *it) {
    Velocity *v = NULL;
    if (it->field_count >= 2) {
        v = ecs_field(it, Velocity, 1);
        if (v) {
            test_assert(!ecs_field_is_self(it, 1));
        }
    }
    
    Mass *m = NULL;
    if (it->field_count >= 3) {
        m = ecs_field(it, Mass, 2);
    }

    probe_iter(it);

    Position *pos = ecs_field(it, Position, 0);

    for (int i = 0; i < it->count; i ++) {
        Position *p = &pos[i];
        p->x += v->x;
        p->y += v->y;

        if (m) {
            p->x += *m;
            p->y += *m;
        }
    }
}

void Prefab_iterate_w_prefab_shared(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Velocity);
    ECS_SYSTEM(world, Prefab_w_shared, EcsOnUpdate, 
        Position, 
        Velocity(self|up IsA));

    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_ENTITY(world, e1, (IsA, Prefab), Position);
    test_assert(e1 != 0);
    ecs_set(world, e1, Position, {0, 0});

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_shared);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], Prefab);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Prefab_match_entity_prefab_w_system_optional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Velocity, Mass);
    ECS_SYSTEM(world, Prefab_w_shared, EcsOnUpdate, 
        Position, 
        Velocity(self|up IsA), 
        ?Mass(self|up IsA));

    ecs_set(world, Prefab, Velocity, {1, 2});
    ecs_set(world, Prefab, Mass, {3});

    ECS_ENTITY(world, e1, (IsA, Prefab), Position);
    test_assert(e1 != 0);
    ecs_set(world, e1, Position, {0, 0});

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_shared);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], Prefab);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], Prefab);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 5);

    ecs_fini(world);
}

void Prefab_prefab_in_system_expr(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab1, Velocity);
    ECS_PREFAB(world, Prefab2, Velocity);
    ECS_SYSTEM(world, Prefab_w_shared, EcsOnUpdate, Position, Velocity(self|up IsA), Mass, (IsA, Prefab1));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Prefab_w_shared
    });

    ecs_set(world, Prefab1, Velocity, {1, 2});
    ecs_set(world, Prefab2, Velocity, {1, 2});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, Prefab1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, Prefab2);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e2, Position, {0, 0});
    ecs_set(world, e3, Position, {0, 0});

    ecs_set(world, e1, Mass, {0});
    ecs_set(world, e2, Mass, {0});
    ecs_set(world, e3, Mass, {0});

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_shared);
    test_int(ctx.term_count, 4);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], Prefab1);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);    
    test_int(ctx.c[0][3], ecs_pair(EcsIsA, Prefab1));
    test_int(ctx.s[0][3], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

static
void Dummy(ecs_iter_t *it) {
    probe_iter(it);
}

void Prefab_dont_match_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Prefab_has_on_instantiate_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(EcsPrefab) });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_owns(world, inst, Position));

    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p == (const Position*)ecs_get_mut(world, inst, Position));
        test_assert(p != ecs_get(world, base, Position));
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_remove(world, inst, Position);
    test_assert(!ecs_has(world, inst, Position));
    test_assert(!ecs_owns(world, inst, Position));

    {
        test_assert(NULL == ecs_get(world, inst, Position));
        test_assert(NULL == ecs_get_mut(world, inst, Position));
    }

    ecs_fini(world);
}

void Prefab_has_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(EcsPrefab) });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has(world, inst, Position));
    test_assert(!ecs_owns(world, inst, Position));

    ecs_add(world, inst, Position);
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_owns(world, inst, Position));

    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p == (const Position*)ecs_get_mut(world, inst, Position));
        test_assert(p != ecs_get(world, base, Position));
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_remove(world, inst, Position);
    test_assert(ecs_has(world, inst, Position));
    test_assert(!ecs_owns(world, inst, Position));

    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(NULL == ecs_get_mut(world, inst, Position));
        test_assert(p == ecs_get(world, base, Position));
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Prefab_has_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(EcsPrefab) });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(!ecs_has(world, inst, Position));
    test_assert(!ecs_owns(world, inst, Position));

    ecs_add(world, inst, Position);
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_owns(world, inst, Position));

    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p == (const Position*)ecs_get_mut(world, inst, Position));
        test_assert(p != ecs_get(world, base, Position));
    }

    ecs_remove(world, inst, Position);
    test_assert(!ecs_has(world, inst, Position));
    test_assert(!ecs_owns(world, inst, Position));

    {
        test_assert(NULL == ecs_get(world, inst, Position));
        test_assert(NULL == ecs_get_mut(world, inst, Position));
    }

    ecs_fini(world);
}

void Prefab_new_w_count_w_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position, Velocity, auto_override | Velocity);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    const ecs_entity_t *ids = ecs_bulk_new_w_id(
        world, ecs_pair(EcsIsA, Prefab), 100);
    test_assert(ids != NULL);

    const Position *prefab_p = ecs_get(world, Prefab, Position);
    const Velocity *prefab_v = ecs_get(world, Prefab, Velocity);

    int i;
    for (i = 0; i < 100; i ++) {
        ecs_entity_t e = ids[i];
        test_assert( ecs_has(world, e, Position));
        test_assert( ecs_has(world, e, Velocity));
        test_assert( ecs_has_pair(world, e, EcsIsA, Prefab));

        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_assert(p == prefab_p);
        test_int(p->x, 10);
        test_int(p->y, 20);

        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v != NULL);
        test_assert(v != prefab_v);
        test_int(v->x, 30);
        test_int(v->y, 40);
    }

    ecs_fini(world);
}

void Prefab_new_w_count_w_override_w_on_set_hook(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_set_hooks(world, Position, {
        .on_set = ecs_on_set(Position)
    });

    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    const ecs_entity_t *ids = ecs_bulk_new_w_id(
        world, ecs_pair(EcsIsA, Prefab), 100);
    test_assert(ids != NULL);

    int i;
    for (i = 0; i < 100; i ++) {
        ecs_entity_t e = ids[i];
        test_assert( ecs_has(world, e, Position));
        test_assert( ecs_has_pair(world, e, EcsIsA, Prefab));

        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 12);
        test_int(p->y, 22);
    }

    ecs_fini(world);
}

void Prefab_override_2_components_different_size(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Color);
    ecs_add_pair(world, ecs_id(Color), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Position, Velocity, Color, auto_override | Velocity, auto_override | Color);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});
    ecs_set(world, Prefab, Color, {1, 2, 3, 4});

    const ecs_entity_t *ids = ecs_bulk_new_w_id(world, ecs_pair(EcsIsA, Prefab), 100);
    test_assert(ids != NULL);

    const Position *prefab_p = ecs_get(world, Prefab, Position);
    const Velocity *prefab_v = ecs_get(world, Prefab, Velocity);
    const Color *prefab_c = ecs_get(world, Prefab, Color);

    test_assert(prefab_p != NULL);
    test_assert(prefab_v != NULL);
    test_assert(prefab_c != NULL);

    int i;
    for (i = 0; i < 100; i ++) {
        ecs_entity_t e = ids[i];
        test_assert( ecs_has(world, e, Position));
        test_assert( ecs_has(world, e, Velocity));
        test_assert( ecs_has_pair(world, e, EcsIsA, Prefab));

        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_assert(p == prefab_p);

        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 30);
        test_int(v->y, 40);
        test_assert(v != prefab_v);

        const Color *c = ecs_get(world, e, Color);
        test_assert(c != NULL);
        test_int(c->r, 1);
        test_int(c->g, 2);
        test_int(c->b, 3);
        test_int(c->a, 4);        
        test_assert(c != prefab_c);        
    }

    ecs_fini(world);
}

void Prefab_ignore_prefab_parent_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, parent, Prefab, Position);
    ECS_ENTITY(world, child, Prefab, (ChildOf, parent), Velocity);

    test_assert( ecs_has_pair(world, child, EcsChildOf, parent));
    test_assert( !ecs_has(world, child, Position));
    test_assert( ecs_has(world, child, Velocity));

    ecs_fini(world);
}

static
void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].y;
        p[i].y += v[i].y;
    }
}

static
void AddVelocity(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Velocity, {1, 1});
    }
}

void Prefab_match_table_created_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {10});

    ECS_ENTITY(world, e, (IsA, Prefab), Position);

    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, [out] !Velocity);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity, Mass(self|up IsA));

    ecs_set(world, e, Position, {0, 0});
    
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));
    test_assert( ecs_has(world, e, Mass));

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 1);

    ecs_progress(world, 1);

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Prefab_prefab_w_1_child(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Parent, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_PREFAB(world, Child, (ChildOf, Parent), Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p_e = ecs_get(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);

    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    const Position *p_child = ecs_get(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e2 != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_get(world, e2, Position) == p_e);

    ecs_entity_t e_child2 = ecs_lookup_child(world, e2, "Child");
    test_assert(e_child2 != 0);
    test_assert(ecs_has(world, e_child2, Position));
    test_assert(ecs_has_pair(world, e_child2, EcsChildOf, e2));
    test_assert( ecs_get(world, e_child2, Position) != p_child);
    
    p_child = ecs_get(world, e_child2, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    ecs_fini(world);
}

void Prefab_prefab_w_2_children(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Parent, Prefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child1, Prefab, (ChildOf, Parent), Position);
            ecs_set(world, Child1, Position, {2, 3});

        ECS_ENTITY(world, Child2, Prefab, (ChildOf, Parent), Position);
            ecs_set(world, Child2, Position, {3, 4});

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child1");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    e_child = ecs_lookup_child(world, e, "Child2");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Prefab_prefab_w_grandchild(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Parent, Prefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, Prefab, (ChildOf, Parent), Position);
            ecs_set(world, Child, Position, {2, 3});

            ECS_ENTITY(world, GrandChild, Prefab, (ChildOf, Parent.Child), Position, Rotation);
                ecs_set(world, GrandChild, Position, {3, 4});

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_entity_t e_grandchild = ecs_lookup_child(world, e_child, "GrandChild");
    test_assert(e_grandchild != 0);
    test_assert(ecs_has(world, e_grandchild, Position));
    test_assert(ecs_has_pair(world, e_grandchild, EcsChildOf, e_child));

    p = ecs_get(world, e_grandchild, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Prefab_prefab_tree_1_2_1(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Rotation);
    ecs_add_pair(world, ecs_id(Rotation), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, parent, Prefab, Position);
        ecs_set(world, parent, Position, {1, 2});

        ECS_ENTITY(world, child_1, Prefab, (ChildOf, parent), Position);
            ecs_set(world, child_1, Position, {2, 3});

        ECS_ENTITY(world, child_2, Prefab, (ChildOf, parent), Position);
            ecs_set(world, child_2, Position, {4, 5});

            ECS_ENTITY(world, grandchild, Prefab, (ChildOf, parent.child_2), Position);
                ecs_set(world, grandchild, Position, {6, 7});

    test_assert(ecs_has_pair(world, child_1, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, child_2, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, grandchild, EcsChildOf, child_2));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "child_1");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    e_child = ecs_lookup_child(world, e, "child_2");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 5);    

    ecs_entity_t e_grandchild = ecs_lookup_child(world, e_child, "grandchild");
    test_assert(e_grandchild != 0);
    test_assert(ecs_has(world, e_grandchild, Position));
    test_assert(ecs_has_pair(world, e_grandchild, EcsChildOf, e_child));

    p = ecs_get(world, e_grandchild, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 7);

    ecs_fini(world);
}

void Prefab_prefab_w_base_w_child(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Prefab, Velocity);
        ecs_set(world, Base, Velocity, {3, 4});

    ECS_ENTITY(world, Parent, Prefab, (IsA, Base), Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, Prefab, (ChildOf, Parent), Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    const Position *p_e = ecs_get(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);

    const Velocity *v_e = ecs_get(world, e, Velocity);
    test_assert(v_e != NULL);
    test_int(v_e->x, 3);
    test_int(v_e->y, 4);    

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(!ecs_has(world, e_child, Velocity));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    const Position *p_child = ecs_get(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);  

    ecs_fini(world);  
}

void Prefab_prefab_w_child_w_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Prefab, Velocity);
        ecs_set(world, Base, Velocity, {3, 4});

    ECS_ENTITY(world, Parent, Prefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, Prefab, (ChildOf, Parent), (IsA, Base), Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p_e = ecs_get(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);   

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has(world, e_child, Velocity));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    const Position *p_child = ecs_get(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    const Velocity *v_child = ecs_get(world, e_child, Velocity);
    test_assert(v_child != NULL);
    test_int(v_child->x, 3);
    test_int(v_child->y, 4);

    ecs_fini(world);  
}

void Prefab_prefab_w_child_w_base_w_children(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Prefab, Velocity);
        ecs_set(world, Base, Velocity, {3, 4});

        ECS_ENTITY(world, BaseChild, Prefab, (ChildOf, Base), Position);
            ecs_set(world, BaseChild, Position, {4, 5});

    ECS_ENTITY(world, Parent, Prefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, Prefab, (ChildOf, Parent), (IsA, Base), Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p_e = ecs_get(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);   

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has(world, e_child, Velocity));
    test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

    const Position *p_child = ecs_get(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    const Velocity *v_child = ecs_get(world, e_child, Velocity);
    test_assert(v_child != NULL);
    test_int(v_child->x, 3);
    test_int(v_child->y, 4); 

    ecs_entity_t e_base_child = ecs_lookup_child(world, e_child, "BaseChild");
    test_assert(e_base_child != 0);
    test_assert(ecs_has(world, e_base_child, Position));
    test_assert(!ecs_has(world, e_base_child, Velocity));
    test_assert(ecs_has_pair(world, e_base_child, EcsChildOf, e_child)); 

    const Position *p_base_child = ecs_get(world, e_base_child, Position);
    test_assert(p_base_child != NULL);
    test_assert(p_base_child != p_child);
    test_int(p_base_child->x, 4);
    test_int(p_base_child->y, 5);

    ecs_fini(world);
}

void Prefab_prefab_w_child_new_w_count(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Parent, Prefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, Prefab, (ChildOf, Parent), Position);
            ecs_set(world, Child, Position, {2, 3});

    const ecs_entity_t *ids = ecs_bulk_new_w_id(world, ecs_pair(EcsIsA, Parent), 3);
    test_assert(ids != NULL);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_entity_t e = ids[i];
        test_assert( ecs_has(world, e, Position));

        const Position *p_e = ecs_get(world, e, Position);
        test_assert(p_e != NULL);
        test_int(p_e->x, 1);
        test_int(p_e->y, 2);

        ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
        test_assert(e_child != 0);
        test_assert(ecs_has(world, e_child, Position));
        test_assert(ecs_has_pair(world, e_child, EcsChildOf, e));

        const Position *p_child = ecs_get(world, e_child, Position);
        test_assert(p_child != NULL);
        test_int(p_child->x, 2);
        test_int(p_child->y, 3);
    }

    ecs_fini(world);
}

void Prefab_prefab_auto_override_child_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Parent, Prefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, ChildPrefab, Prefab, Position, Velocity);
            ecs_set(world, ChildPrefab, Position, {2, 3});
            ecs_set(world, ChildPrefab, Velocity, {4, 5});

        ECS_ENTITY(world, Child, Prefab, (ChildOf, Parent), (IsA, ChildPrefab), Velocity);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, Parent);
    test_assert(e2 != 0);
    test_assert( ecs_has(world, e2, Position));

    const Position *p1 = ecs_get(world, e1, Position);
    test_assert(p1 != NULL);
    test_int(p1->x, 1);
    test_int(p1->y, 2);

    const Position *p2 = ecs_get(world, e2, Position);
    test_assert(p2 != NULL);
    test_int(p2->x, 1);
    test_int(p2->y, 2);    
    test_assert(p1 == p2);

    ecs_entity_t child_1 = ecs_lookup_child(world, e1, "Child");
    test_assert(child_1 != 0);

    test_assert( ecs_has(world, child_1, Position));
    test_assert( ecs_has(world, child_1, Velocity));

    ecs_entity_t child_2 = ecs_lookup_child(world, e2, "Child");
    test_assert(child_2 != 0);
    test_assert( ecs_has(world, child_2, Position));
    test_assert( ecs_has(world, child_2, Velocity));

    const Position *child_p1 = ecs_get(world, child_1, Position);
    test_assert(child_p1 != NULL);
    test_int(child_p1->x, 2);
    test_int(child_p1->y, 3);

    const Position *child_p2 = ecs_get(world, child_2, Position);
    test_assert(child_p2 != NULL);
    test_int(child_p2->x, 2);
    test_int(child_p2->y, 3);   
    test_assert(child_p1 == child_p2); 

    const Velocity *child_v1 = ecs_get(world, child_1, Velocity);
    test_assert(child_v1 != NULL);
    test_int(child_v1->x, 4);
    test_int(child_v1->y, 5);

    const Velocity *child_v2 = ecs_get(world, child_2, Velocity);
    test_assert(child_v2 != NULL);
    test_int(child_v2->x, 4);
    test_int(child_v2->y, 5); 
    test_assert(child_v1 != child_v2);

    ecs_fini(world);
}

static int invoked;

void PrefabReactiveTest(ecs_iter_t *it) {
    invoked ++;
}

void Prefab_ignore_on_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_OBSERVER(world, PrefabReactiveTest, EcsOnAdd, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_fini(world);
}

void Prefab_ignore_on_remove(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_OBSERVER(world, PrefabReactiveTest, EcsOnRemove, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_remove(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_fini(world);
}

void Prefab_ignore_on_set(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_OBSERVER(world, PrefabReactiveTest, EcsOnSet, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_set(world, Prefab, Position, {0, 0});

    test_int(invoked, 0);

    ecs_fini(world);
}

void Prefab_on_set_on_instance(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_OBSERVER(world, PrefabReactiveTest, EcsOnSet, Position(self|up IsA));

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_set(world, Prefab, Position, {1, 2});

    test_int(invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_int(invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void InstantiateInProgress(ecs_iter_t *it) {
    ecs_id_t Prefab = ecs_field_id(it, 1);
    ecs_entity_t *ids = ecs_get_ctx(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ids[i] = ecs_new_w_pair(it->world, EcsIsA, Prefab);
    }
}

void Prefab_instantiate_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, InstantiateInProgress, EcsOnUpdate, Position, Prefab());

    const ecs_entity_t *dummy_ids = ecs_bulk_new(world, Position, 10);
    test_assert(dummy_ids != NULL);

    ecs_entity_t ids[10];
    ecs_set_ctx(world, ids, NULL);

    ecs_progress(world, 1);

    test_int(ecs_count_id(world, ecs_pair(EcsIsA, Prefab)), 10);

    const Velocity *v_prefab = ecs_get(world, Prefab, Velocity);

    int i;
    for (i = 0; i < 10; i ++) {
        const Velocity *v = ecs_get(world, ids[i], Velocity);
        test_assert(v == v_prefab);
    }

    ecs_fini(world);
}

void NewInProgress(ecs_iter_t *it) {
    ecs_id_t Prefab = ecs_field_id(it, 1);

    ecs_entity_t *ids = ecs_get_ctx(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ids[i] = ecs_new_w_pair(it->world, EcsIsA, Prefab);
    }
}

void Prefab_copy_from_prefab_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Velocity, auto_override | Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, NewInProgress, EcsOnUpdate, Position, Prefab());

    ecs_entity_t ids[10];
    ecs_set_ctx(world, ids, NULL);

    const ecs_entity_t *dummy_ids = ecs_bulk_new(world, Position, 10);
    test_assert(dummy_ids != NULL);

    /* Create one prefab instance so table is already created (case where table
     * is not created is tested in copy_from_prefab_first_instance_in_progress*/
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e != 0);

    ecs_progress(world, 1);

    test_int(ecs_count_id(world, ecs_pair(EcsIsA, Prefab)), 11);

    int i;
    for (i = 0; i < 10; i ++) {
        const Velocity *v = ecs_get(world, ids[i], Velocity);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void Prefab_copy_from_prefab_first_instance_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Velocity, auto_override | Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, NewInProgress, EcsOnUpdate, Position, Prefab());

    ecs_entity_t ids[10];
    ecs_set_ctx(world, ids, NULL);

    const ecs_entity_t *dummy_ids = ecs_bulk_new(world, Position, 10);
    test_assert(dummy_ids != NULL);

    ecs_progress(world, 1);

    test_int(ecs_count_id(world, ecs_pair(EcsIsA, Prefab)), 10);

    int i;
    for (i = 0; i < 10; i ++) {
        const Velocity *v = ecs_get(world, ids[i], Velocity);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void Prefab_ref_after_realloc(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {2});

    ECS_ENTITY(world, e1, Position, Mass);
    ecs_set(world, e1, Mass, {3});

    ECS_ENTITY(world, e2, Position, (IsA, Prefab));

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(self|up IsA), Position);

    /* Trigger a realloc of the table in which the prefab is stored. This should
     * cause systems with refs to re-resolve their cached ref ptrs  */
    for (int i = 0; i < 1000; i ++) {
        ecs_clone(world, 0, Prefab, false);
    }
    
    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Prefab_revalidate_ref_w_mixed_table_refs(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_ENTITY(world, e1, Position, (IsA, Prefab));
    ECS_ENTITY(world, e2, Position, Mass);
    ecs_set(world, e2, Mass, {3});

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(self|up IsA), Position);

    /* Trigger a realloc of the table in which the prefab is stored. This should
     * cause systems with refs to re-resolve their cached ref ptrs  */
    for (int i = 0; i < 1000; i ++) {
        ecs_clone(world, 0, Prefab, false);
    }
    
    ecs_set(world, Prefab, Mass, {2});
    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Prefab_no_overwrite_on_2nd_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_ENTITY(world, e1, Position, (IsA, Prefab));

    ecs_add(world, e1, Mass);
    test_int( *ecs_get(world, e1, Mass), 1);

    ecs_set(world, e1, Mass, {2});
    test_int( *ecs_get(world, e1, Mass), 2);

    ecs_add(world, e1, Mass);
    test_int( *ecs_get(world, e1, Mass), 2);

    ecs_fini(world);
}

void AddPrefab(ecs_iter_t *it) {
    ecs_id_t Prefab = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_pair(it->world, it->entities[i], EcsIsA, Prefab);
    }
}

void Prefab_no_overwrite_on_2nd_add_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, Prefab());

    ECS_ENTITY(world, e1, Position, (IsA, Prefab));
    ecs_add(world, e1, Mass);
    test_int( *ecs_get(world, e1, Mass), 1);

    ecs_set(world, e1, Mass, {2});
    test_int( *ecs_get(world, e1, Mass), 2);

    ecs_progress(world, 1);

    test_int( *ecs_get(world, e1, Mass), 2);

    ecs_fini(world);
}

void Prefab_no_instantiate_on_2nd_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, (ChildOf, Prefab), Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, Prefab());

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert( ecs_has_pair(world, e, EcsIsA, Prefab));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child != 0);
    const Velocity *v = ecs_get(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    ecs_set(world, e_child, Velocity, {4, 5});
    v = ecs_get(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);

    ecs_add_pair(world, e, EcsIsA, Prefab);

    ecs_entity_t e_child_2 = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child == e_child_2);
    v = ecs_get(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);
    
    ecs_fini(world);
}

void Prefab_no_instantiate_on_2nd_add_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, (ChildOf, Prefab), Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, Prefab());

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert( ecs_has_pair(world, e, EcsIsA, Prefab));
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child != 0);
    const Velocity *v = ecs_get(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    ecs_set(world, e_child, Velocity, {4, 5});
    v = ecs_get(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);

    ecs_progress(world, 1);

    ecs_entity_t e_child_2 = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child == e_child_2);
    v = ecs_get(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);
    
    ecs_fini(world);
}

void NewPrefab_w_count(ecs_iter_t *it) {
    ecs_entity_t *ids = ecs_get_ctx(it->world);
    ecs_id_t Prefab = ecs_field_id(it, 0);

    const ecs_entity_t *new_ids = ecs_bulk_new_w_id(it->world, ecs_pair(EcsIsA, Prefab), 3);
    test_assert(new_ids != NULL);
    memcpy(ids, new_ids, sizeof(ecs_entity_t) * 3);
}

void Prefab_nested_prefab_in_progress_w_count(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, (ChildOf, Prefab), Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, NewPrefab_w_count, EcsOnUpdate, Prefab());

    ecs_entity_t ids[3] = {0};
    ecs_set_ctx(world, ids, NULL);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_has_pair(world, ids[i], EcsIsA, Prefab);

        const Position *p = ecs_get(world, ids[i], Position);
        test_assert(p != NULL); 
        test_int(p->x, 1);
        test_int(p->y, 2);

        ecs_entity_t child = ecs_lookup_child(world, ids[i], "ChildPrefab");
        test_assert(child != 0);

        const Velocity *v = ecs_get(world, child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
    }
    
    ecs_fini(world);
}

static int on_set_velocity_invoked;

static
void OnSetVelocity(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 0);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 0);

    on_set_velocity_invoked ++;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);

        if (ecs_field_is_self(it, 0)) {
            v[i].x ++;
            v[i].y ++;
        }
    }
}

void Prefab_nested_prefab_in_progress_w_count_set_after_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, (ChildOf, Prefab), Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, NewPrefab_w_count, EcsOnUpdate, Prefab());
    ECS_OBSERVER(world, OnSetVelocity, EcsOnSet, Velocity);

    test_int(on_set_velocity_invoked, 0);

    ecs_entity_t ids[3] = {0};
    ecs_set_ctx(world, ids, NULL);

    ecs_progress(world, 1);

    test_assert(ids != NULL);
    test_int(on_set_velocity_invoked, 3);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_has_pair(world, ids[i], EcsIsA, Prefab);

        const Position *p = ecs_get(world, ids[i], Position);
        test_assert(p != NULL); 
        test_int(p->x, 1);
        test_int(p->y, 2);

        ecs_entity_t child = ecs_lookup_child(world, ids[i], "ChildPrefab");
        test_assert(child != 0);

        const Velocity *v = ecs_get(world, child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3 + 1);
        test_int(v->y, 4 + 1);
    }
    
    ecs_fini(world);
}

void AddPrefabInProgress(ecs_iter_t *it) {
    ecs_id_t Prefab = ecs_field_id(it, 1);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_pair(it->world, it->entities[i], EcsIsA, Prefab);
        test_assert( ecs_has_id(it->world, it->entities[i], Prefab));
        test_assert( ecs_has(it->world, it->entities[i], Velocity));
        
        const Velocity *v = ecs_get(it->world, it->entities[i], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);

        test_assert( ecs_get(it->world, Prefab, Velocity) == v);
    }
}

void Prefab_get_ptr_from_prefab_from_new_table_in_progress(void) {    
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Prefab, Velocity);
        ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, AddPrefabInProgress, EcsOnUpdate, Position, Prefab(), Velocity());

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);

    ecs_progress(world, 1);

    test_assert( ecs_has_pair(world, e, EcsIsA, Prefab));
    test_assert( ecs_has(world, e, Velocity));
    
    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    test_assert( ecs_get(world, Prefab, Velocity) == v);

    ecs_fini(world);
}

void TestBase(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    test_assert(!ecs_field_is_self(it, 1));

    test_assert(p != NULL);
    test_assert(v != NULL);
    test_int(it->count, 1);
}

void Prefab_match_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Base, Velocity);
        ecs_set(world, Base, Velocity, {1, 2});

    ECS_PREFAB(world, Child, (IsA, Base));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Child);
    ecs_add(world, e, Position);

    ECS_SYSTEM(world, TestBase, EcsOnUpdate, Position, Velocity(self|up IsA));

    ecs_progress(world, 1);

    ecs_fini(world);
}

static
void AddMass(ecs_iter_t *it) {
    ecs_id_t ecs_id(Mass) = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Mass);
    }
}

void Prefab_match_base_after_add_in_prev_phase(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, Base, Velocity);
        ecs_set(world, Base, Velocity, {1, 2});

    ECS_PREFAB(world, Child, (IsA, Base));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Child);
    ecs_add(world, e, Position);

    ECS_SYSTEM(world, AddMass, EcsPreUpdate, Position, !Mass);
    ECS_SYSTEM(world, TestBase, EcsOnUpdate, Position, Velocity(self|up IsA));

    ecs_progress(world, 1);

    ecs_fini(world);
}

void Prefab_override_watched_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    /* Create a system that listens for Position */
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position(self|up));

    /* Create a prefab with Position */
    ECS_PREFAB(world, Prefab, Position);

    /* Create an instance of Prefab, this will cause Prefab to be watched since
     * it will be matched as reference with the system */
    ECS_ENTITY(world, Entity1, (IsA, Prefab));

    /* Another instance of Prefab is created, prefab data is resolved to check
     * if components need to be overridden. Index will be negative, so code 
     * needs to flip sign on the index, or this will fail. */
    ECS_ENTITY(world, Entity2, (IsA, Prefab), Position);

    const Position *p1 = ecs_get(world, Prefab, Position);
    const Position *p2 = ecs_get(world, Entity1, Position);
    const Position *p3 = ecs_get(world, Entity2, Position);

    test_assert(p1 == p2);
    test_assert(p2 != p3);

    ecs_fini(world);
}

void Prefab_rematch_twice(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, 
        Position(self|up IsA), 
        Velocity(self|up IsA), 
        Mass(self|up IsA));

    ECS_PREFAB(world, Prefab, Position);
    ECS_ENTITY(world, Entity, (IsA, Prefab));

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_progress(world, 1);
    test_int(ctx.count, 0);
    ctx.count = 0;

    ecs_add(world, Prefab, Velocity);

    ecs_progress(world, 1);
    test_int(ctx.count, 0);
    ctx.count = 0;

    ecs_add(world, Prefab, Mass);

    ecs_progress(world, 1);
    test_int(ctx.count, 1);

    ecs_fini(world);
}

static
void AddPosition(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 0);
    
    ecs_entity_t *base = ecs_get_ctx(it->world);

    ecs_add(it->world, *base, Position);
}

void Prefab_add_to_empty_base_in_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ECS_SYSTEM(world, AddPosition, EcsOnUpdate, Position());

    ecs_set_ctx(world, &base, NULL);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, base, Position));

    ecs_fini(world);
}

void Prefab_inherit_disabled(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Velocity);
    
    ecs_add_id(world, e2, EcsDisabled);
    ecs_add_pair(world, e1, EcsIsA, e2);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has_id(world, e1, EcsDisabled));
    test_assert( ecs_has_pair(world, e1, EcsIsA, e2));
    test_assert( !ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has_id(world, e2, EcsDisabled));

    ecs_fini(world);
}

static bool has_cloned = false;

static
void CloneInOnAdd(ecs_iter_t *it)
{
    Position *p = ecs_field(it, Position, 0);

    int i;
    for (i = 0; i < it->count; i ++) {
        bool has_cloned_test = has_cloned;

        /* Prevent recursive OnAdd calls */
        has_cloned = true;

        p[i].x = 10;
        p[i].y = 20;

        if (!has_cloned_test) {
            ecs_entity_t e = it->entities[i];
            ecs_entity_t clone = ecs_clone(it->world, 0, e, true);
            ecs_add_pair(it->world, e, EcsIsA, clone);
        }
    }
}

void Prefab_clone_after_inherit_in_on_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_OBSERVER(world, CloneInOnAdd, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(has_cloned == true);

    ecs_fini(world);
}

void Prefab_override_from_nested(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    
    ECS_PREFAB(world, BasePrefab, Position);
    ecs_set(world, BasePrefab, Position, {10, 20});

    ECS_PREFAB(world, SubPrefab, (IsA, BasePrefab), Velocity, auto_override | Position, auto_override | Velocity);
    ecs_set(world, SubPrefab, Velocity, {30, 40});

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, SubPrefab);
    test_assert(e1 != 0);
    test_assert( ecs_owns(world, e1, Position));
    test_assert( ecs_owns(world, e1, Velocity));
    test_assert( ecs_owns(world, e1, Position));
    test_assert( ecs_owns(world, e1, Velocity));
    test_assert( ecs_has_pair(world, e1, EcsIsA, SubPrefab));
    test_assert( ecs_has_pair(world, e1, EcsIsA, BasePrefab));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

static
void OnAddEntity(ecs_iter_t *it) {
    ecs_entity_t *e = ecs_field(it, ecs_entity_t, 0);

    int i;
    for (i = 0; i < it->count; i ++) {
        e[i] = it->entities[i];
    }
}

void Prefab_create_multiple_nested_w_on_set(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, ecs_entity_t);
    ecs_add_pair(world, ecs_id(ecs_entity_t), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, Child, (ChildOf, Prefab), Velocity, ecs_entity_t);
            ecs_set(world, Child, Velocity, {3, 4});

    ECS_OBSERVER(world, OnAddEntity, EcsOnSet, ecs_entity_t);
    ECS_OBSERVER(world, OnSetVelocity, EcsOnSet, Velocity);

    test_int(on_set_velocity_invoked, 0);

    /* Create two entities with separate calls */ 
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, Prefab);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, Prefab);

    test_int(on_set_velocity_invoked, 2);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));

    ecs_entity_t child1 = ecs_lookup_child(world, e1, "Child");
    test_assert(child1 != 0);

    ecs_entity_t child2 = ecs_lookup_child(world, e2, "Child");
    test_assert(child2 != 0);
    test_assert(child1 != child2);

    const ecs_entity_t *self1 = ecs_get(world, child1, ecs_entity_t);
    test_assert(self1 != NULL);
    test_assert(*self1 == child1);

    const ecs_entity_t *self2 = ecs_get(world, child2, ecs_entity_t);
    test_assert(self2 != NULL);
    test_assert(self1 != self2);
    test_assert(*self2 == child2);

    test_assert( ecs_has(world, child1, Velocity));
    test_assert( ecs_has(world, child2, Velocity));
    test_assert( ecs_owns(world, child1, Velocity));
    test_assert( ecs_owns(world, child2, Velocity));

    const Velocity *v1 = ecs_get(world, child1, Velocity);
    test_assert(v1 != NULL);
    test_int(v1->x, 4);
    test_int(v1->y, 5);

    const Velocity *v2 = ecs_get(world, child2, Velocity);
    test_assert(v2 != NULL);
    test_assert(v1 != v2);
    test_int(v2->x, 4);
    test_int(v2->y, 5);

    ecs_fini(world);
}

static ecs_entity_t new_instance_1, new_instance_2;

static
void CreateInstances(ecs_iter_t *it) {
    ecs_id_t Prefab = ecs_field_id(it, 0);

    new_instance_1 = ecs_new_w_pair(it->world, EcsIsA, Prefab);
    new_instance_2 = ecs_new_w_pair(it->world, EcsIsA, Prefab);
}

void Prefab_create_multiple_nested_w_on_set_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, ecs_entity_t);
    ecs_add_pair(world, ecs_id(ecs_entity_t), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

        ECS_PREFAB(world, Child, (ChildOf, Prefab), (IsA, ChildPrefab), ecs_entity_t);

    ECS_SYSTEM(world, CreateInstances, EcsOnUpdate, Prefab());
    ECS_OBSERVER(world, OnAddEntity, EcsOnSet, ecs_entity_t);
    ECS_OBSERVER(world, OnSetVelocity, EcsOnSet, Velocity(self|up IsA));

    ecs_progress(world, 1);

    test_int(on_set_velocity_invoked, 2);

    ecs_entity_t e1 = new_instance_1;
    ecs_entity_t e2 = new_instance_2;

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));

    ecs_entity_t child1 = ecs_lookup_child(world, e1, "Child");
    test_assert(child1 != 0);

    ecs_entity_t child2 = ecs_lookup_child(world, e2, "Child");
    test_assert(child2 != 0);
    test_assert(child1 != child2);

    const ecs_entity_t *self1 = ecs_get(world, child1, ecs_entity_t);
    test_assert(self1 != NULL);
    test_assert(*self1 == child1);

    const ecs_entity_t *self2 = ecs_get(world, child2, ecs_entity_t);
    test_assert(self2 != NULL);
    test_assert(self1 != self2);
    test_assert(*self2 == child2);

    test_assert( ecs_has(world, child1, Velocity));
    test_assert( ecs_has(world, child2, Velocity));
    test_assert( ecs_owns(world, child1, Velocity));
    test_assert( ecs_owns(world, child2, Velocity));

    const Velocity *v1 = ecs_get(world, child1, Velocity);
    test_assert(v1 != NULL);
    test_int(v1->x, 3);
    test_int(v1->y, 4);

    const Velocity *v2 = ecs_get(world, child2, Velocity);
    test_assert(v2 != NULL);
    test_assert(v1 != v2);
    test_int(v2->x, 3);
    test_int(v2->y, 4);

    ecs_fini(world);
}

void Prefab_single_on_set_on_child_w_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, ecs_entity_t);
    ecs_add_pair(world, ecs_id(ecs_entity_t), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

        ECS_PREFAB(world, Child, (ChildOf, Prefab), (IsA, ChildPrefab), Velocity);

    ECS_OBSERVER(world, OnSetVelocity, EcsOnSet, Velocity);

    test_int(on_set_velocity_invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);

    /* Make sure OnSet is invoked only once, even though Child overrides a
     * component from a ChildPrefab. Velocity is already overridden for Child,
     * and while it would technically not be incorrect to issue an OnSet for the
     * override, we don't want to spam the application with redundant triggers. */
    test_int(on_set_velocity_invoked, 1);

    test_assert( ecs_has(world, e, Position));

    ecs_entity_t child = ecs_lookup_child(world, e, "Child");
    test_assert(child != 0);

    test_assert( ecs_has(world, child, Velocity));
    test_assert( ecs_owns(world, child, Velocity));

    const Velocity *v = ecs_get(world, child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);

    ecs_fini(world);
}

void Prefab_auto_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position, Velocity, auto_override | Position);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_owns(world, e, Velocity));

    ecs_fini(world);
}

void Prefab_auto_override_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position, Velocity, auto_override | Position, auto_override | Velocity);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_owns(world, e, Velocity));

    ecs_fini(world);
}

void Prefab_auto_override_nested(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Prefab, Position, Velocity, auto_override | Position);
    ECS_PREFAB(world, Prefab_2, (IsA, Prefab));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab_2);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_owns(world, e, Velocity));

    ecs_fini(world);
}

void Prefab_auto_override_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TgtA);
    ecs_add_pair(world, TgtA, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, TgtB);
    ecs_add_pair(world, TgtB, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, Rel);
    ecs_add_pair(world, Rel, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtA));
    test_assert(!ecs_has_pair(world, base, Rel, TgtA));
    test_assert(!ecs_has_pair(world, base, Rel, TgtB));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtA)));

    ecs_add_pair(world, base, Rel, TgtA);
    ecs_add_pair(world, base, Rel, TgtB);

    test_assert(ecs_has_pair(world, base, Rel, TgtA));
    test_assert(ecs_has_pair(world, base, Rel, TgtB));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtA)));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, inst, Rel, TgtA));
    test_assert(ecs_has_pair(world, inst, Rel, TgtB));

    test_assert(ecs_owns_pair(world, inst, Rel, TgtA));
    test_assert(!ecs_owns_pair(world, inst, Rel, TgtB));

    ecs_fini(world);
}

void Prefab_auto_override_pair_w_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TgtA);
    ecs_add_pair(world, TgtA, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, TgtB);
    ecs_add_pair(world, TgtB, EcsOnInstantiate, EcsInherit);

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(ecs_id(Position), TgtA));
    test_assert(ecs_get_pair(world, base, Position, TgtA) == NULL);

    ecs_set_pair(world, base, Position, TgtA, {10, 20});
    ecs_set_pair(world, base, Position, TgtB, {20, 30});

    const Position *pa = ecs_get_pair(world, base, Position, TgtA);
    const Position *pb = ecs_get_pair(world, base, Position, TgtB);
    test_assert(pa != NULL);
    test_assert(pb != NULL);
    test_int(pa->x, 10);
    test_int(pa->y, 20);
    test_int(pb->x, 20);
    test_int(pb->y, 30);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    const Position *pia = ecs_get_pair(world, inst, Position, TgtA);
    const Position *pib = ecs_get_pair(world, inst, Position, TgtB);
    test_assert(pia != pa);
    test_assert(pib == pb);
    test_int(pia->x, 10);
    test_int(pia->y, 20);

    test_assert(ecs_owns_pair(world, inst, ecs_id(Position), TgtA));
    test_assert(!ecs_owns_pair(world, inst, ecs_id(Position), TgtB));

    ecs_fini(world);
}

void Prefab_auto_override_2_pairs(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TgtA);
    ecs_add_pair(world, TgtA, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, TgtB);
    ecs_add_pair(world, TgtB, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, Rel);
    ecs_add_pair(world, Rel, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtA));
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtB));
    test_assert(!ecs_has_pair(world, base, Rel, TgtA));
    test_assert(!ecs_has_pair(world, base, Rel, TgtB));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtA)));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtB)));

    ecs_add_pair(world, base, Rel, TgtA);
    ecs_add_pair(world, base, Rel, TgtB);

    test_assert(ecs_has_pair(world, base, Rel, TgtA));
    test_assert(ecs_has_pair(world, base, Rel, TgtB));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtA)));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(Rel, TgtB)));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, inst, Rel, TgtA));
    test_assert(ecs_has_pair(world, inst, Rel, TgtB));

    test_assert(ecs_owns_pair(world, inst, Rel, TgtA));
    test_assert(ecs_owns_pair(world, inst, Rel, TgtB));

    ecs_fini(world);
}

void Prefab_auto_override_2_pairs_w_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TgtA);
    ecs_add_pair(world, TgtA, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, TgtB);
    ecs_add_pair(world, TgtB, EcsOnInstantiate, EcsInherit);

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(ecs_id(Position), TgtA));
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(ecs_id(Position), TgtB));
    test_assert(ecs_get_pair(world, base, Position, TgtA) == NULL);
    test_assert(ecs_get_pair(world, base, Position, TgtB) == NULL);

    ecs_set_pair(world, base, Position, TgtA, {10, 20});
    ecs_set_pair(world, base, Position, TgtB, {20, 30});

    const Position *pa = ecs_get_pair(world, base, Position, TgtA);
    const Position *pb = ecs_get_pair(world, base, Position, TgtB);
    test_assert(pa != NULL);
    test_assert(pb != NULL);
    test_int(pa->x, 10);
    test_int(pa->y, 20);
    test_int(pb->x, 20);
    test_int(pb->y, 30);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    const Position *pia = ecs_get_pair(world, inst, Position, TgtA);
    const Position *pib = ecs_get_pair(world, inst, Position, TgtB);
    test_assert(pia != pa);
    test_assert(pib != pb);
    test_int(pia->x, 10);
    test_int(pia->y, 20);
    test_int(pib->x, 20);
    test_int(pib->y, 30);
    
    test_assert(ecs_owns_pair(world, inst, ecs_id(Position), TgtA));
    test_assert(ecs_owns_pair(world, inst, ecs_id(Position), TgtB));

    ecs_fini(world);
}

void Prefab_auto_override_2_pairs_same_obj(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);
    ecs_add_pair(world, Tgt, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, RelA);
    ecs_add_pair(world, RelA, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, RelB);
    ecs_add_pair(world, RelB, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(RelA, Tgt));
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(RelB, Tgt));
    test_assert(!ecs_has_pair(world, base, RelA, Tgt));
    test_assert(!ecs_has_pair(world, base, RelB, Tgt));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(RelA, Tgt)));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(RelB, Tgt)));

    ecs_add_pair(world, base, RelA, Tgt);
    ecs_add_pair(world, base, RelB, Tgt);

    test_assert(ecs_has_pair(world, base, RelA, Tgt));
    test_assert(ecs_has_pair(world, base, RelB, Tgt));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(RelA, Tgt)));
    test_assert(ecs_has_id(world, base, ECS_AUTO_OVERRIDE | ecs_pair(RelB, Tgt)));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, inst, RelA, Tgt));
    test_assert(ecs_has_pair(world, inst, RelB, Tgt));

    test_assert(ecs_owns_pair(world, inst, RelA, Tgt));
    test_assert(ecs_owns_pair(world, inst, RelB, Tgt));

    ecs_fini(world);
}

void Prefab_prefab_instanceof_hierarchy(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, Base, Position);
        ECS_PREFAB(world, BaseChild, Position, (ChildOf, Base));

    ECS_PREFAB(world, ThePrefab, (IsA, Base));

    /* Ensure that child has not been instantiated by making
     * sure there are no matching entities for Position up to this point */
    ecs_query_t *q = ecs_query(world, { .expr = "Position(self|up IsA)" });

    ecs_iter_t qit = ecs_query_iter(world, q);
    test_assert(!ecs_query_next(&qit));

    /* Instantiate the prefab */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, ThePrefab);
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup_child(world, e, "BaseChild");
    test_assert(child != 0);

    qit = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&qit) == true);
    test_int(qit.count, 1);
    test_assert(ecs_query_next(&qit) == true);
    test_int(qit.count, 1);
    test_assert(ecs_query_next(&qit) == false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Prefab_override_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_set(world, base, Position, {10, 20});
    
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Tag));

    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_owns(world, e, Tag));

    ecs_fini(world);
}

void Prefab_empty_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_pair(world, e, EcsIsA, base));

    ecs_fini(world);
}

void Prefab_instanceof_0(void) {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    test_expect_abort();

    /* Not allowed */
    ecs_new_w_pair(world, EcsIsA, 0);

    ecs_fini(world);
}

void Prefab_instantiate_empty_child_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_PREFAB(world, Prefab, #0);

    /* Forces creation of child table without children */
    ecs_table_t *table = ecs_table_add_id(world, 0, ecs_childof(Prefab));
    test_assert(table != NULL);

    /* Main goal of this test is to ensure this doesn't crash */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, EcsIsA, Prefab));

    /* This shouldn't crash either */
    ecs_delete(world, Prefab);
    test_assert(!ecs_is_alive(world, Prefab));

    ecs_fini(world);
}

void Prefab_instantiate_emptied_child_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_PREFAB(world, Prefab, #0);

    /* Create & remove prefab child */
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, Prefab);
    test_assert(child != 0);

    /* Deleting the child will leave an initialized but empty table */
    ecs_delete(world, child);
    test_assert(!ecs_is_alive(world, child));

    /* Main goal of this test is to ensure this doesn't crash */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, EcsIsA, Prefab));

    /* This shouldn't crash either */
    ecs_delete(world, Prefab);
    test_assert(!ecs_is_alive(world, Prefab));

    ecs_fini(world);
}

void Prefab_override_2_prefabs(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, PrefabA, Position);
    ECS_PREFAB(world, PrefabB, Velocity);

    ecs_set(world, PrefabA, Position, {10, 20});
    ecs_set(world, PrefabB, Velocity, {1, 2});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, PrefabA);
    ecs_add_pair(world, e, EcsIsA, PrefabB);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_assert(ecs_owns(world, e, Position));
    test_assert(ecs_owns(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Prefab_rematch_after_add_instanceof_to_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, { .expr = "Position(up)" });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_assert(base != 0);
    test_assert(parent != 0);
    test_assert(child != 0);

    ecs_add_pair(world, parent, EcsIsA, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], child);

    Position *p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Prefab_child_of_instance(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, EcsChildOf, base);

    test_expect_abort();

    /* Should trigger an assert */
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_fini(world);
}

void Prefab_rematch_after_prefab_delete(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, { .expr = "Position(up IsA)" });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    Position *p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(!ecs_query_next(&it));

    ecs_delete(world, base);

    it = ecs_query_iter(world, q);
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Prefab_add_tag_w_low_id_to_instance(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t Tag = ecs_new_low_id(world);
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_add_id(world, base, Tag);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add(world, e, Position);
    ecs_add_id(world, e, Tag);
    
    test_assert(ecs_has_id(world, e, Tag));

    ecs_fini(world);
}

void Prefab_get_type_after_base_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Position);
    test_assert(base != 0);
    test_assert( ecs_get_type(world, base) != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_get_type(world, base) != NULL);

    ecs_fini(world);
}

void Prefab_get_type_after_recycled_base_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);
    test_assert( ecs_get_type(world, base) != NULL);
    test_int( ecs_get_type(world, base)->count, 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_new_w(world, Position);
    test_assert(base != 0);
    test_assert(ecs_entity_t_lo(base) != base); // Ensure recycled
    test_assert( ecs_get_type(world, base) != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_get_type(world, base) != NULL);

    ecs_fini(world);
}

void Prefab_new_w_recycled_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_new(world);
    test_assert(base != 0);
    test_assert(ecs_entity_t_lo(base) != base); // Ensure recycled

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));

    ecs_fini(world);
}

void Prefab_add_recycled_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_new(world);
    test_assert(base != 0);
    test_assert(ecs_entity_t_lo(base) != base); // Ensure recycled

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));

    ecs_fini(world);
}

void Prefab_remove_recycled_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_new(world);
    test_assert(base != 0);
    test_assert(ecs_entity_t_lo(base) != base); // Ensure recycled

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));

    ecs_remove_pair(world, e, EcsIsA, base);
    test_assert( !ecs_has_pair(world, e, EcsIsA, base));

    ecs_fini(world);
}

void Prefab_get_from_recycled_base(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(base != 0);
    test_assert(ecs_entity_t_lo(base) != base); // Ensure recycled
    test_assert( ecs_get_type(world, base) != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(p == ecs_get(world, base, Position));

    ecs_fini(world);
}

void Prefab_override_from_recycled_base(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(base != 0);
    test_assert(ecs_entity_t_lo(base) != base); // Ensure recycled

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_owns(world, e, Position));

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Prefab_remove_override_from_recycled_base(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(base != 0);
    test_assert(ecs_entity_t_lo(base) != base); // Ensure recycled

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_owns(world, e, Position));

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_owns(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_owns(world, e, Position));

    ecs_fini(world);
}

void Prefab_instantiate_tree_from_recycled_base(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(base != 0);
    ecs_add_id(world, base, EcsPrefab);

    ecs_entity_t base_child = ecs_new_w_pair(world, EcsChildOf, base);
    test_assert(base_child != 0);

    ecs_set_name(world, base_child, "Child");

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(p == ecs_get(world, base, Position));

    ecs_entity_t instance_child = ecs_lookup_child(world, e, "Child");
    test_assert(instance_child != 0);
    test_assert( ecs_has_pair(world, instance_child, EcsChildOf, e));

    ecs_fini(world);
}

void Prefab_rematch_after_add_to_recycled_base(void) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag, Position(up IsA)" });

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_delete(world, base);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_new(world);
    test_assert(base != 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, EcsIsA, base));
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), false);

    ecs_set(world, base, Position, {10, 20});

    ecs_progress(world, 0);

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);

    const Position *p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(ecs_field_src(&it, 1) == base);
    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Prefab_get_tag_from_2nd_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_new(world);
    ecs_entity_t base_2 = ecs_new_w_id(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base_1);
    ecs_add_pair(world, e, EcsIsA, base_2);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has_id(world, e, Tag));

    ecs_fini(world);
}

void Prefab_get_component_from_2nd_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_new(world);
    ecs_entity_t base_2 = ecs_new_w(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base_1);
    ecs_add_pair(world, e, EcsIsA, base_2);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_2, Position));

    ecs_fini(world);
}

void Prefab_get_component_from_1st_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_new_w(world, Position);
    ecs_entity_t base_2 = ecs_new_w(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base_1);
    ecs_add_pair(world, e, EcsIsA, base_2);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_1, Position));

    ecs_fini(world);
}

void Prefab_get_component_from_2nd_base_of_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_new(world);
    ecs_entity_t base_2 = ecs_new_w(world, Position);

    ecs_entity_t base_3 = ecs_new(world);
    ecs_add_pair(world, base_3, EcsIsA, base_1);
    ecs_add_pair(world, base_3, EcsIsA, base_2);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_3);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_2, Position));

    ecs_fini(world);
}

void Prefab_get_component_from_1st_base_of_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_new_w(world, Position);
    ecs_entity_t base_2 = ecs_new_w(world, Position);

    ecs_entity_t base_3 = ecs_new(world);
    ecs_add_pair(world, base_3, EcsIsA, base_1);
    ecs_add_pair(world, base_3, EcsIsA, base_2);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_3);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_1, Position));

    ecs_fini(world);
}

void Prefab_get_component_from_2nd_base_prefab_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, base_1, #0);
    ECS_PREFAB(world, base_2, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base_1);
    ecs_add_pair(world, e, EcsIsA, base_2);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_2, Position));

    ecs_fini(world);
}

void Prefab_get_component_from_1st_base_prefab_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, base_1, Position);
    ECS_PREFAB(world, base_2, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base_1);
    ecs_add_pair(world, e, EcsIsA, base_2);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_1, Position));

    ecs_fini(world);
}

void Prefab_get_component_from_2nd_base_of_base_prefab_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, base_1, #0);
    ECS_PREFAB(world, base_2, Position);
    ECS_PREFAB(world, base_3, (IsA, base_1), (IsA, base_2));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_3);

    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_2, Position));

    ecs_fini(world);
}

void Prefab_get_component_from_1st_base_of_base_prefab_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_PREFAB(world, base_1, Position);
    ECS_PREFAB(world, base_2, Position);
    ECS_PREFAB(world, base_3, (IsA, base_1), (IsA, base_2));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_3);
    test_assert(ecs_has_pair(world, e, EcsIsA, base_1));
    test_assert(ecs_has_pair(world, e, EcsIsA, base_2));

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base_1, Position));

    ecs_fini(world);
}

void Prefab_fail_on_override_final(void) {
    install_test_abort();
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_new_w_id(world, EcsFinal);

    test_expect_abort();
    ecs_new_w_pair(world, EcsIsA, base);

    ecs_fini(world);
}

static
int child_count(ecs_world_t *world, ecs_entity_t e) {
    int32_t count = 0;
    ecs_iter_t it = ecs_each_pair(world, EcsChildOf, e);

    while (ecs_each_next(&it)) {
        count += it.count;
    }

    return count;
}

void Prefab_instantiate_tree_once(void) {
    ecs_world_t *world = ecs_init();

    ECS_PREFAB(world, Cannon, #0);
    ECS_PREFAB(world, Turret, #0);
    ECS_PREFAB(world, CannonA, (IsA, Cannon), (ChildOf, Turret));
    ECS_PREFAB(world, CannonB, (IsA, Cannon), (ChildOf, Turret));

    ECS_PREFAB(world, SpaceShip, #0);
    ECS_PREFAB(world, TurretA, (IsA, Turret), (ChildOf, SpaceShip));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, SpaceShip);
    test_assert(inst != 0);
    test_int(1, child_count(world, inst));

    ecs_entity_t turret_a = ecs_lookup_child(world, inst, "TurretA");
    test_assert(turret_a != 0);
    test_int(2, child_count(world, turret_a));

    ecs_fini(world);
}

void Prefab_nested_prefab_w_named_children(void) {
    ecs_world_t *world = ecs_init();

    ECS_PREFAB(world, Cannon, #0);
    
    ECS_PREFAB(world, Turret, #0);
    ecs_set_scope(world, Turret);
        ECS_ENTITY(world, CannonA, (IsA, Cannon));
    ecs_set_scope(world, 0);

    ECS_PREFAB(world, SpaceShip, #0);
    ecs_set_scope(world, SpaceShip);
        ECS_PREFAB(world, TurretA, (IsA, Turret));
    ecs_set_scope(world, 0);

    test_assert( ecs_has_pair(world, CannonA, EcsChildOf, Turret));
    test_assert( ecs_has_pair(world, CannonA, EcsIsA, Cannon));
    test_str("CannonA", ecs_get_name(world, CannonA));

    test_assert( ecs_has_pair(world, TurretA, EcsChildOf, SpaceShip));
    test_assert( ecs_has_pair(world, TurretA, EcsIsA, Turret));
    test_str("TurretA", ecs_get_name(world, TurretA));

    ecs_fini(world);
}

void Prefab_dont_copy_children_for_non_prefab_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, Base, Position);
    ECS_ENTITY(world, Child, (ChildOf, Base));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Base);
    test_assert(e != 0);
    test_assert(ecs_get(world, e, Position) != NULL);

    test_assert(ecs_lookup_child(world, e, "Child") == 0);
    
    ecs_fini(world);
}

void Prefab_get_component_pair_from_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, Obj);
    ecs_add_pair(world, Obj, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_insert(world, ecs_pair_value(Position, Obj, {10, 20}));
    test_assert(ecs_has_pair(world, base, ecs_id(Position), Obj));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, inst, ecs_id(Position), Obj));

    const Position *p = ecs_get_pair(world, inst, Position, Obj);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(p == ecs_get_pair(world, base, Position, Obj));

    ecs_fini(world);
}

void Prefab_get_component_pair_from_prefab_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, Obj);
    ecs_add_pair(world, Obj, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set_pair(world, base, Position, Obj, {10, 20});
    test_assert(ecs_has_pair(world, base, ecs_id(Position), Obj));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, inst, ecs_id(Position), Obj));

    const Position *p = ecs_get_pair(world, inst, Position, Obj);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(p == ecs_get_pair(world, base, Position, Obj));

    ecs_fini(world);
}

void Prefab_override_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsDontInherit);

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor
    });

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert( !ecs_has(world, inst, Position));

    ecs_add(world, inst, Position);
    test_assert( ecs_has(world, inst, Position));
    const Position *p = ecs_get(world, inst, Position);
    test_int(p->x, 0);
    test_int(p->y, 0);

    ecs_fini(world);
}

void Prefab_prefab_child_w_dont_inherit_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, DontInherit));
    ECS_TAG(world, TagB);
    ecs_add_pair(world, TagB, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_child = ecs_entity(world, { .name = "Base.Child", .add = ecs_ids( EcsPrefab ) });
    ecs_add(world, base_child, TagA);
    ecs_add(world, base_child, TagB);
    test_assert(base != 0);
    test_assert(base_child != 0);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst != 0);

    ecs_entity_t inst_child = ecs_lookup_child(world, inst, "Child");
    test_assert(inst_child != 0);
    test_assert(!ecs_has(world, inst_child, TagA));
    test_assert(ecs_has(world, inst_child, TagB));

    ecs_fini(world);
}

void Prefab_prefab_child_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, Bar);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    ecs_entity_t turret = ecs_entity(world, { .name = "Turret", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t turret_head = ecs_entity(world, { .name = "Turret.Head", .add = ecs_ids( EcsPrefab ) });
    ecs_add(world, turret_head, Foo);

    ecs_entity_t railgun = ecs_entity(world, { .name = "Railgun", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, railgun, EcsIsA, turret);
    ecs_entity_t railgun_head = ecs_entity(world, { .name = "Railgun.Head", .add = ecs_ids( EcsPrefab ) });
    ecs_add(world, railgun_head, Bar);

    ecs_entity_t inst = ecs_entity(world, { .name = "inst" });
    ecs_add_pair(world, inst, EcsIsA, railgun);

    ecs_entity_t head = ecs_lookup_child(world, inst, "Head");
    test_assert(head != 0);
    test_assert(ecs_has(world, head, Foo));
    test_assert(ecs_has(world, head, Bar));

    ecs_fini(world);
}

void Prefab_prefab_child_override_w_exclusive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Exclusive, (OnInstantiate, Inherit));
    ECS_TAG(world, ObjA);
    ecs_add_pair(world, ObjA, EcsOnInstantiate, EcsInherit);
    ECS_TAG(world, ObjB);
    ecs_add_pair(world, ObjB, EcsOnInstantiate, EcsInherit);

    ecs_entity_t turret = ecs_entity(world, { .name = "Turret", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t turret_head = ecs_entity(world, { .name = "Turret.Head", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, turret_head, Rel, ObjA);
    test_assert(ecs_has_pair(world, turret_head, Rel, ObjA));

    ecs_entity_t railgun = ecs_entity(world, { .name = "Railgun", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, railgun, EcsIsA, turret);
    ecs_entity_t railgun_head = ecs_entity(world, { .name = "Railgun.Head", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, railgun_head, Rel, ObjB);
    test_assert(!ecs_has_pair(world, railgun_head, Rel, ObjA));
    test_assert(ecs_has_pair(world, railgun_head, Rel, ObjB));

    ecs_entity_t inst = ecs_entity(world, { .name = "inst" });
    ecs_add_pair(world, inst, EcsIsA, railgun);

    ecs_entity_t head = ecs_lookup_child(world, inst, "Head");
    test_assert(head != 0);
    test_assert(!ecs_has_pair(world, head, Rel, ObjA));
    test_assert(ecs_has_pair(world, head, Rel, ObjB));

    ecs_fini(world);
}

void Prefab_prefab_1_slot(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst != 0);
    
    ecs_entity_t inst_slot = ecs_get_target(world, inst, base_slot, 0);
    test_assert(inst_slot != 0);
    test_str(ecs_get_name(world, inst_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot, EcsChildOf, inst));

    ecs_fini(world);
}

void Prefab_prefab_2_slots(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot_a = ecs_entity(world, { .name = "Base.SlotA", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot_b = ecs_entity(world, { .name = "Base.SlotB", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot_a, EcsSlotOf, base);
    ecs_add_pair(world, base_slot_b, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot_a, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot_a, EcsSlotOf, base));
    test_assert(ecs_has_pair(world, base_slot_b, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot_b, EcsSlotOf, base));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst != 0);
    
    ecs_entity_t inst_slot_a = ecs_get_target(world, inst, base_slot_a, 0);
    test_assert(inst_slot_a != 0);
    test_str(ecs_get_name(world, inst_slot_a), "SlotA");
    test_assert(!ecs_has_pair(world, inst_slot_a, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot_a, EcsChildOf, inst));

    ecs_entity_t inst_slot_b = ecs_get_target(world, inst, base_slot_b, 0);
    test_assert(inst_slot_b != 0);
    test_str(ecs_get_name(world, inst_slot_b), "SlotB");
    test_assert(!ecs_has_pair(world, inst_slot_b, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot_b, EcsChildOf, inst));

    ecs_fini(world);
}

void Prefab_prefab_w_nested_slot(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot_slot = ecs_entity(world, { .name = "Base.Slot.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    ecs_add_pair(world, base_slot_slot, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));
    test_assert(ecs_has_pair(world, base_slot_slot, EcsChildOf, base_slot));
    test_assert(ecs_has_pair(world, base_slot_slot, EcsSlotOf, base));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst != 0);
    
    ecs_entity_t inst_slot = ecs_get_target(world, inst, base_slot, 0);
    test_assert(inst_slot != 0);
    test_str(ecs_get_name(world, inst_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot, EcsChildOf, inst));

    ecs_entity_t inst_slot_slot = ecs_get_target(world, inst, base_slot_slot, 0);
    test_assert(inst_slot_slot != 0);
    test_assert(inst_slot_slot != inst_slot);
    test_str(ecs_get_name(world, inst_slot_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot_slot, EcsChildOf, inst_slot));

    ecs_fini(world);
}

void Prefab_prefab_w_mixed_slots(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot_slot = ecs_entity(world, { .name = "Base.Slot.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    ecs_add_pair(world, base_slot_slot, EcsSlotOf, base_slot);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));
    test_assert(ecs_has_pair(world, base_slot_slot, EcsChildOf, base_slot));
    test_assert(ecs_has_pair(world, base_slot_slot, EcsSlotOf, base_slot));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst != 0);
    
    ecs_entity_t inst_slot = ecs_get_target(world, inst, base_slot, 0);
    test_assert(inst_slot != 0);
    test_str(ecs_get_name(world, inst_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot, EcsChildOf, inst));

    ecs_entity_t inst_slot_slot = ecs_get_target(world, inst, base_slot_slot, 0);
    test_assert(inst_slot_slot == 0);

    inst_slot_slot = ecs_get_target(world, inst_slot, base_slot_slot, 0);
    test_assert(inst_slot_slot != 0);
    test_assert(inst_slot_slot != inst_slot);
    test_str(ecs_get_name(world, inst_slot_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot_slot, EcsChildOf, inst_slot));

    ecs_fini(world);
}

void Prefab_prefab_variant_w_slot(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t variant = ecs_entity(world, { .name = "Variant", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t variant_slot = ecs_entity(world, { .name = "Variant.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, variant, EcsIsA, base);
    ecs_add_pair(world, variant_slot, EcsSlotOf, variant);
    test_assert(ecs_has_pair(world, variant_slot, EcsChildOf, variant));
    test_assert(ecs_has_pair(world, variant_slot, EcsSlotOf, variant));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, variant);
    test_assert(inst != 0);
    
    ecs_entity_t inst_slot = ecs_get_target(world, inst, variant_slot, 0);
    test_assert(inst_slot != 0);
    test_str(ecs_get_name(world, inst_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot, EcsChildOf, inst));

    ecs_fini(world);
}

void Prefab_prefab_variant_w_base_slot(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));

    ecs_entity_t variant = ecs_entity(world, { .name = "Variant", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, variant, EcsIsA, base);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, variant);
    test_assert(inst != 0);
    
    ecs_entity_t inst_slot = ecs_get_target(world, inst, base_slot, 0);
    test_assert(inst_slot != 0);
    test_str(ecs_get_name(world, inst_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot, EcsChildOf, inst));

    ecs_fini(world);
}

void Prefab_prefab_variant_w_mixed_slots(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.BaseSlot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));

    ecs_entity_t variant = ecs_entity(world, { .name = "Variant", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, variant, EcsIsA, base);
    ecs_entity_t variant_slot = ecs_entity(world, { .name = "Variant.VariantSlot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, variant, EcsIsA, base);
    ecs_add_pair(world, variant_slot, EcsSlotOf, variant);
    test_assert(ecs_has_pair(world, variant_slot, EcsChildOf, variant));
    test_assert(ecs_has_pair(world, variant_slot, EcsSlotOf, variant));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, variant);
    test_assert(inst != 0);
    
    ecs_entity_t inst_base_slot = ecs_get_target(world, inst, base_slot, 0);
    test_assert(inst_base_slot != 0);
    test_str(ecs_get_name(world, inst_base_slot), "BaseSlot");
    test_assert(!ecs_has_pair(world, inst_base_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_base_slot, EcsChildOf, inst));

    ecs_entity_t inst_variant_slot = ecs_get_target(world, inst, variant_slot, 0);
    test_assert(inst_variant_slot != 0);
    test_assert(inst_variant_slot != inst_base_slot);
    test_str(ecs_get_name(world, inst_variant_slot), "VariantSlot");
    test_assert(!ecs_has_pair(world, inst_variant_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_variant_slot, EcsChildOf, inst));

    ecs_fini(world);
}

void Prefab_override_slot(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst != 0);
    
    ecs_entity_t inst_slot = ecs_get_target(world, inst, base_slot, 0);
    test_assert(inst_slot != 0);
    test_str(ecs_get_name(world, inst_slot), "Slot");
    test_assert(!ecs_has_pair(world, inst_slot, EcsSlotOf, EcsWildcard));
    test_assert(ecs_has_pair(world, inst_slot, EcsChildOf, inst));

    ecs_entity_t slot_override = ecs_new(world);
    ecs_add_pair(world, inst, base_slot, slot_override);

    test_assert(ecs_has_pair(world, inst, base_slot, slot_override));
    test_assert(!ecs_has_pair(world, inst, base_slot, inst_slot));

    ecs_fini(world);
}

void Prefab_2_instances_w_slots_same_table(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));

    ecs_entity_t inst_1 = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst_1 != 0);
    ecs_entity_t inst_2 = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst_2 != 0);

    ecs_entity_t slot_1 = ecs_get_target(world, inst_1, base_slot, 0);
    test_assert(slot_1 != 0);
    ecs_entity_t slot_2 = ecs_get_target(world, inst_2, base_slot, 0);
    test_assert(slot_2 != 0);
    test_assert(slot_1 != slot_2);

    /* Make sure slots don't fragment */
    test_assert(ecs_get_table(world, inst_1) == ecs_get_table(world, inst_2));

    ecs_fini(world);
}

void Prefab_slot_has_union(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_entity(world, { .name = "Base", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t base_slot = ecs_entity(world, { .name = "Base.Slot", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, base_slot, EcsSlotOf, base);
    test_assert(ecs_has_pair(world, base_slot, EcsChildOf, base));
    test_assert(ecs_has_pair(world, base_slot, EcsSlotOf, base));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(inst != 0);

    ecs_entity_t inst_slot = ecs_get_target(world, inst, base_slot, 0);
    test_assert(inst_slot != 0);
    test_assert(base_slot != 0);

    test_assert( ecs_has_id(world, base_slot, EcsExclusive));
    test_assert( ecs_has_id(world, base_slot, EcsDontFragment));

    test_assert( !ecs_has_id(world, inst_slot, EcsExclusive));
    test_assert( !ecs_has_id(world, inst_slot, EcsDontFragment));

    ecs_fini(world);
}

void Prefab_slot_override(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t turret = ecs_entity(world, { .name = "Turret", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t turret_base = ecs_entity(world, { .name = "Turret.Base", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, turret_base, EcsSlotOf, turret);
    ecs_entity_t turret_head = ecs_entity(world, { .name = "Turret.Head", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, turret_head, EcsSlotOf, turret);

    ecs_entity_t railgun = ecs_entity(world, { .name = "Railgun", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, railgun, EcsIsA, turret);
    ecs_entity_t railgun_head = ecs_entity(world, { .name = "Railgun.Head", .add = ecs_ids( EcsPrefab ) });

    test_assert(ecs_has_pair(world, railgun_head, EcsSlotOf, turret));
    ecs_add_pair(world, railgun_head, EcsSlotOf, railgun);
    test_assert(ecs_has_pair(world, railgun_head, EcsSlotOf, railgun));
    test_assert(!ecs_has_pair(world, railgun_head, EcsSlotOf, turret));

    ecs_entity_t railgun_beam = ecs_entity(world, { .name = "Railgun.Beam", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, railgun_beam, EcsSlotOf, railgun);

    ecs_entity_t inst = ecs_entity(world, { .name = "inst" });
    ecs_add_pair(world, inst, EcsIsA, railgun);

    ecs_entity_t head = ecs_get_target(world, inst, turret_head, 0);
    ecs_entity_t head_r = ecs_get_target(world, inst, railgun_head, 0);
    ecs_entity_t base = ecs_get_target(world, inst, turret_base, 0);
    ecs_entity_t beam = ecs_get_target(world, inst, railgun_beam, 0);

    test_assert(head == 0);
    test_assert(head_r != 0);
    test_assert(base != 0);
    test_assert(beam != 0);

    char *path = ecs_get_path(world, head_r);
    test_str(path, "inst.Head");
    ecs_os_free(path);

    path = ecs_get_path(world, base);
    test_str(path, "inst.Base");
    ecs_os_free(path);

    path = ecs_get_path(world, beam);
    test_str(path, "inst.Beam");
    ecs_os_free(path);

    ecs_fini(world);
}

void Prefab_base_slot_override(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t turret = ecs_entity(world, { .name = "Turret", .add = ecs_ids( EcsPrefab ) });
    ecs_entity_t turret_base = ecs_entity(world, { .name = "Turret.Base", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, turret_base, EcsSlotOf, turret);
    ecs_entity_t turret_head = ecs_entity(world, { .name = "Turret.Head", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, turret_head, EcsSlotOf, turret);

    ecs_entity_t railgun = ecs_entity(world, { .name = "Railgun", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, railgun, EcsIsA, turret);
    ecs_entity_t railgun_head = ecs_entity(world, { .name = "Railgun.Head", .add = ecs_ids( EcsPrefab ) });
    test_assert(ecs_has_pair(world, railgun_head, EcsSlotOf, turret));

    ecs_entity_t railgun_beam = ecs_entity(world, { .name = "Railgun.Beam", .add = ecs_ids( EcsPrefab ) });
    ecs_add_pair(world, railgun_beam, EcsSlotOf, railgun);

    ecs_entity_t inst = ecs_entity(world, { .name = "inst" });
    ecs_add_pair(world, inst, EcsIsA, railgun);

    ecs_entity_t head = ecs_get_target(world, inst, turret_head, 0);
    ecs_entity_t head_r = ecs_get_target(world, inst, railgun_head, 0);
    ecs_entity_t base = ecs_get_target(world, inst, turret_base, 0);
    ecs_entity_t beam = ecs_get_target(world, inst, railgun_beam, 0);

    test_assert(head != 0);
    test_assert(head_r == 0);
    test_assert(base != 0);
    test_assert(beam != 0);

    char *path = ecs_get_path(world, head);
    test_str(path, "inst.Head");
    ecs_os_free(path);

    path = ecs_get_path(world, base);
    test_str(path, "inst.Base");
    ecs_os_free(path);

    path = ecs_get_path(world, beam);
    test_str(path, "inst.Beam");
    ecs_os_free(path);

    ecs_fini(world);
}

void Prefab_has_slot_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t slot = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add_pair(world, slot, EcsSlotOf, p);
    ecs_add(world, slot, Foo);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t inst_slot = ecs_get_target(world, inst, slot, 0);
    test_assert(inst_slot != 0);
    test_assert(ecs_has(world, inst_slot, Foo));

    ecs_delete(world, inst);
    ecs_make_alive(world, inst);

    test_assert(!ecs_has_pair(world, inst, slot, inst_slot));
    test_assert(ecs_get_target(world, inst, slot, 0) == 0);

    ecs_fini(world);
}

void Prefab_has_slot_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t slot = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add_pair(world, slot, EcsSlotOf, p);
    ecs_add(world, slot, Foo);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t inst_slot = ecs_get_target(world, inst, slot, 0);
    test_assert(inst_slot != 0);
    test_assert(ecs_has(world, inst_slot, Foo));

    ecs_clear(world, inst);

    test_assert(!ecs_has_pair(world, inst, slot, inst_slot));
    test_assert(ecs_get_target(world, inst, slot, 0) == 0);

    ecs_fini(world);
}

void Prefab_override_twice_w_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10});
    ecs_auto_override(world, base, Position);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_set(world, e1, Position, { 20 });

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add(world, e2, Position);

    const Position *ptr = ecs_get(world, e1, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 20);

    ptr = ecs_get(world, e2, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);

    ecs_fini(world);
}

void Prefab_override_twice_w_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10});
    ecs_auto_override(world, base, Position);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_set(world, e1, Position, { 20 });
    
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_set(world, e2, Position, {30});

    const Position *ptr = ecs_get(world, e1, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 20);

    ptr = ecs_get(world, e2, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 30);

    ecs_fini(world);
}

static int position_copy_invoked = 0;

static ECS_COPY(Position, dst, src, {
    dst->x = src->x;
    dst->y = src->y;
    position_copy_invoked ++;
})

void Prefab_auto_override_copy_once(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_set_hooks(world, Position, {
        .copy = ecs_copy(Position)
    });

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});
    ecs_auto_override(world, base, Position);

    test_int(position_copy_invoked, 1);
    position_copy_invoked = 0;

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    const Position *ptr = ecs_get(world, inst, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    test_int(position_copy_invoked, 1);

    ecs_fini(world);
}

void Prefab_always_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsOverride);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set(world, b, Position, {10, 20});
    ecs_set(world, b, Velocity, {1, 2});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, b);
    test_assert(ecs_has(world, i, Position));
    test_assert(ecs_has(world, i, Velocity));
    test_assert(ecs_owns(world, i, Position));
    test_assert(!ecs_owns(world, i, Velocity));

    {
        const Position *ptr = ecs_get(world, i, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    {
        const Velocity *ptr = ecs_get(world, i, Velocity);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1);
        test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Prefab_always_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ecs_add_pair(world, RelA, EcsOnInstantiate, EcsOverride);
    ECS_TAG(world, RelB);
    ecs_add_pair(world, RelB, EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set_pair_second(world, b, RelA, Position, {10, 20});
    ecs_set_pair_second(world, b, RelB, Velocity, {1, 2});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, b);
    test_assert(ecs_has_pair(world, i, RelA, ecs_id(Position)));
    test_assert(ecs_has_pair(world, i, RelB, ecs_id(Velocity)));
    test_assert(ecs_owns_pair(world, i, RelA, ecs_id(Position)));
    test_assert(!ecs_owns_pair(world, i, RelB, ecs_id(Velocity)));

    {
        const Position *ptr = ecs_get_pair_second(world, i, RelA, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    {
        const Velocity *ptr = ecs_get_pair_second(world, i, RelB, Velocity);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1);
        test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Prefab_child_of_prefab_is_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, base);
    test_assert(ecs_has_id(world, child, EcsPrefab));
    test_assert(ecs_has_pair(world, child, EcsChildOf, base));

    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    test_assert(ecs_has_id(world, grand_child, EcsPrefab));
    test_assert(ecs_has_pair(world, grand_child, EcsChildOf, child));

    ecs_fini(world);
}

void Prefab_child_of_prefab_w_prefab_is_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t child = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, child, EcsChildOf, base);
    test_assert(ecs_has_id(world, child, EcsPrefab));
    test_assert(ecs_has_pair(world, child, EcsChildOf, base));

    ecs_fini(world);
}

void Prefab_child_of_prefab_w_prefab_is_prefab_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t child = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, child, Position);
    ecs_add_pair(world, child, EcsChildOf, base);

    test_assert(ecs_has_id(world, child, EcsPrefab));
    test_assert(ecs_has(world, child, Position));
    test_assert(ecs_has_pair(world, child, EcsChildOf, base));

    ecs_fini(world);
}

void Prefab_override_exclusive(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_ENTITY(ecs, Rel, Exclusive, (OnInstantiate, Inherit));

    ecs_entity_t t1 = ecs_new(ecs);
    ecs_entity_t t2 = ecs_new(ecs);

    ecs_entity_t e = ecs_new(ecs);
    ecs_add_pair(ecs, e, Rel, t1);

    ecs_entity_t p = ecs_new(ecs);
    ecs_auto_override_pair(ecs, p, Rel, t2);

    ecs_add_pair(ecs, e, EcsIsA, p);

    test_assert(ecs_has_pair(ecs, e, Rel, t2));
    test_assert(!ecs_has_pair(ecs, e, Rel, t1));

    ecs_fini(ecs);
}

void Prefab_override_exclusive_2_lvls(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_ENTITY(ecs, Rel, Exclusive, (OnInstantiate, Inherit));

    ecs_entity_t t1 = ecs_new(ecs);
    ecs_entity_t t2 = ecs_new(ecs);
    ecs_entity_t t3 = ecs_new(ecs);

    ecs_entity_t e = ecs_new(ecs);
    ecs_add_pair(ecs, e, Rel, t1);

    ecs_entity_t p = ecs_new(ecs);
    ecs_auto_override_pair(ecs, p, Rel, t2);

    ecs_entity_t p2 = ecs_new(ecs);
    ecs_auto_override_pair(ecs, p2, Rel, t3);
    ecs_add_pair(ecs, p2, EcsIsA, p);

    ecs_add_pair(ecs, e, EcsIsA, p2);

    test_assert(ecs_has_pair(ecs, e, Rel, t2));
    test_assert(!ecs_has_pair(ecs, e, Rel, t1));
    test_assert(!ecs_has_pair(ecs, e, Rel, t3));

    ecs_fini(ecs);
}

static
ecs_entity_t make_prefabs(ecs_world_t *ecs) {
    ecs_entity_t root = ecs_new_w_id(ecs, EcsPrefab);
    ecs_entity_t parent = ecs_new_w_pair(ecs, EcsChildOf, root);
    ecs_new_w_pair(ecs, EcsChildOf, parent);
    return root;
}

void Prefab_hierarchy_w_recycled_id(void) {
    ecs_world_t* ecs = ecs_mini();

    {
        ecs_entity_t p = make_prefabs(ecs);
        test_assert(p != 0);
        ecs_entity_t i = ecs_new_w_pair(ecs, EcsIsA, p);
        test_assert(i != 0);
        ecs_delete(ecs, i);
    }

    {
        ecs_entity_t p = make_prefabs(ecs);
        test_assert(p != 0);
        ecs_entity_t i = ecs_new_w_pair(ecs, EcsIsA, p);
        test_assert(i != 0);
    }

    ecs_fini(ecs);
}

void Prefab_disable_ids(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, A, 0);
    ECS_ENTITY(world, B, 0);
    ECS_PREFAB(world, Prefab, A, B);

    test_assert(!ecs_has_id(world, A, EcsDisabled));
    test_assert(!ecs_has_id(world, B, EcsDisabled));
    test_assert(!ecs_has_id(world, EcsPrefab, EcsDisabled));

    ecs_enable(world, Prefab, false);

    test_assert(ecs_has_id(world, A, EcsDisabled));
    test_assert(ecs_has_id(world, B, EcsDisabled));
    test_assert(!ecs_has_id(world, EcsPrefab, EcsDisabled));

    ecs_enable(world, Prefab, true);

    test_assert(!ecs_has_id(world, A, EcsDisabled));
    test_assert(!ecs_has_id(world, B, EcsDisabled));
    test_assert(!ecs_has_id(world, EcsPrefab, EcsDisabled));

    ecs_fini(world);
}

void Prefab_disable_nested_ids(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, A, 0);
    ECS_ENTITY(world, B, 0);
    ECS_PREFAB(world, PrefabX, A);
    ECS_PREFAB(world, PrefabY, B, PrefabX);

    test_assert(!ecs_has_id(world, A, EcsDisabled));
    test_assert(!ecs_has_id(world, B, EcsDisabled));
    test_assert(!ecs_has_id(world, PrefabX, EcsDisabled));
    test_assert(!ecs_has_id(world, EcsPrefab, EcsDisabled));

    ecs_enable(world, PrefabY, false);

    test_assert(ecs_has_id(world, A, EcsDisabled));
    test_assert(ecs_has_id(world, B, EcsDisabled));
    test_assert(!ecs_has_id(world, PrefabX, EcsDisabled));
    test_assert(!ecs_has_id(world, EcsPrefab, EcsDisabled));

    ecs_enable(world, PrefabY, true);

    test_assert(!ecs_has_id(world, A, EcsDisabled));
    test_assert(!ecs_has_id(world, B, EcsDisabled));
    test_assert(!ecs_has_id(world, PrefabX, EcsDisabled));
    test_assert(!ecs_has_id(world, EcsPrefab, EcsDisabled));

    ecs_fini(world);
}

void Prefab_prefab_w_children_w_isa_auto_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t child = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);

    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsIsA, child);
    ecs_add_pair(world, child_1, EcsChildOf, base);
    ecs_set_name(world, child_1, "child_1");

    ecs_entity_t child_2 = ecs_new_w_pair(world, EcsIsA, child);
    ecs_add_pair(world, child_2, EcsChildOf, base);
    ecs_set_name(world, child_2, "child_2");

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    {
        ecs_entity_t inst_child = ecs_lookup_from(world, inst, "child_1");
        test_assert(inst_child != 0);
        test_assert(ecs_owns(world, inst_child, Position));

        const Position *p = ecs_get(world, inst_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        ecs_entity_t inst_child = ecs_lookup_from(world, inst, "child_2");
        test_assert(inst_child != 0);
        test_assert(ecs_owns(world, inst_child, Position));

        const Position *p = ecs_get(world, inst_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Prefab_prefab_child_w_override(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    ecs_entity_t p = ecs_entity(ecs, { .name = "p", .add = ecs_ids(EcsPrefab) });
    ecs_entity_t c = ecs_new_w_pair(ecs, EcsChildOf, p);
    ecs_set_name(ecs, c, "c");
    ecs_auto_override(ecs, c, Position);

    ecs_entity_t i = ecs_new_w_pair(ecs, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(ecs, i, "c");
    test_assert(ic != 0);
    test_assert(ecs_has(ecs, ic, Position));
    test_assert(!ecs_has_id(ecs, ic, ECS_AUTO_OVERRIDE | ecs_id(Position)));

    ecs_fini(ecs);
}

void Prefab_prefab_child_w_override_and_higher_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t p = ecs_entity(ecs, { .name = "p", .add = ecs_ids(EcsPrefab) });
    ecs_entity_t c = ecs_new_w_pair(ecs, EcsChildOf, p);
    ecs_set_name(ecs, c, "c");
    ecs_auto_override(ecs, c, Position);
    ecs_add(ecs, c, Velocity);

    ecs_entity_t i = ecs_new_w_pair(ecs, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(ecs, i, "c");
    test_assert(ic != 0);
    test_assert(ecs_has(ecs, ic, Position));
    test_assert(ecs_has(ecs, ic, Velocity));
    test_assert(!ecs_has_id(ecs, ic, ECS_AUTO_OVERRIDE | ecs_id(Position)));
    test_assert(!ecs_has_id(ecs, ic, ECS_AUTO_OVERRIDE | ecs_id(Velocity)));

    ecs_fini(ecs);
}

void Prefab_prefab_child_w_override_and_lower_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Velocity);
    ECS_COMPONENT(ecs, Position);

    ecs_entity_t p = ecs_entity(ecs, { .name = "p", .add = ecs_ids(EcsPrefab) });
    ecs_entity_t c = ecs_new_w_pair(ecs, EcsChildOf, p);
    ecs_set_name(ecs, c, "c");
    ecs_auto_override(ecs, c, Position);
    ecs_add(ecs, c, Velocity);

    ecs_entity_t i = ecs_new_w_pair(ecs, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(ecs, i, "c");
    test_assert(ic != 0);
    test_assert(ecs_has(ecs, ic, Position));
    test_assert(ecs_has(ecs, ic, Velocity));
    test_assert(!ecs_has_id(ecs, ic, ECS_AUTO_OVERRIDE | ecs_id(Position)));
    test_assert(!ecs_has_id(ecs, ic, ECS_AUTO_OVERRIDE | ecs_id(Velocity)));

    ecs_fini(ecs);
}

void Prefab_prefab_1_child_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_2_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_3_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c3, "c3");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, i, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_2_children_2_types_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_3_children_3_types_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Zoo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c3, "c3");

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Zoo);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, i, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_2_children_2_types_reverse_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");

    ecs_add(world, c2, Bar);
    ecs_add(world, c1, Foo);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_3_children_3_types_reverse_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Zoo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c3, "c3");

    ecs_add(world, c3, Zoo);
    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, i, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_2_lvl_nested_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, c1);
    ecs_set_name(world, c2, "c2");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, ic1, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_3_lvl_nested_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, c1);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, c2);
    ecs_set_name(world, c3, "c3");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, ic1, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, ic2, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_recycled_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    test_assert(p != (uint32_t)p);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - (uint32_t)p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_recycled_instance_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_delete(world, ecs_new(world));

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(i != (uint32_t)i);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - p) == ((uint32_t)ic - (uint32_t)i));

    ecs_fini(world);
}

void Prefab_prefab_children_recycled_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_delete(world, ecs_new(world));
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c != (uint32_t)c);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert(((uint32_t)c - p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_recycled_children_recycled_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    test_assert(p != (uint32_t)p);

    ecs_delete(world, ecs_new(world));

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c != (uint32_t)c);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert(((uint32_t)c - (uint32_t)p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_recycled_children_recycled_offset_id_different_generation(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    test_assert(p != (uint32_t)p);

    ecs_delete(world, ecs_new(world));
    ecs_delete(world, ecs_new(world));

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c != (uint32_t)c);
    test_assert((c >> 32) != (p >> 32));
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert(((uint32_t)c - (uint32_t)p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_1_child_offset_id_occupied(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new(world);
    ecs_entity_t x = ecs_new(world); // Occupy instance child id

    test_assert((c - p) == (x - i));

    ecs_add_pair(world, i, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);
    test_assert(ic != x);

    test_assert((c - p) != (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_1_child_offset_id_recycled_occupied(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new(world);
    ecs_delete(world, ecs_new(world));
    ecs_entity_t x = ecs_new(world); // Occupy instance child id

    test_assert((c - p) == ((uint32_t)x - i));

    ecs_add_pair(world, i, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);
    test_assert(ic != x);
    test_assert((uint32_t)ic != (uint32_t)x);

    test_assert((c - p) != (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_child_offset_w_smaller_child_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_delete(world, a);
    ecs_delete(world, b);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c < p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - p) != (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_1_child_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_2_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_3_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c3, "c3");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, i, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_2_children_2_types_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_3_children_3_types_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Zoo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c3, "c3");

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Zoo);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, i, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_2_children_2_types_reverse_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");

    ecs_add(world, c2, Bar);
    ecs_add(world, c1, Foo);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_3_children_3_types_reverse_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Zoo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c3, "c3");

    ecs_add(world, c3, Zoo);
    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, i, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, i, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_2_lvl_nested_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, c1);
    ecs_set_name(world, c2, "c2");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, ic1, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_3_lvl_nested_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c1, "c1");
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, c1);
    ecs_set_name(world, c2, "c2");
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, c2);
    ecs_set_name(world, c3, "c3");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic1 = ecs_lookup_from(world, i, "c1");
    test_assert(ic1 != 0);
    test_assert((c1 - p) == (ic1 - i));

    ecs_entity_t ic2 = ecs_lookup_from(world, ic1, "c2");
    test_assert(ic2 != 0);
    test_assert((c2 - p) == (ic2 - i));

    ecs_entity_t ic3 = ecs_lookup_from(world, ic2, "c3");
    test_assert(ic3 != 0);
    test_assert((c3 - p) == (ic3 - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_recycled_children_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    test_assert(p != (uint32_t)p);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - (uint32_t)p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_recycled_instance_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_delete(world, ecs_new(world));

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(i != (uint32_t)i);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - p) == ((uint32_t)ic - (uint32_t)i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_children_recycled_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_delete(world, ecs_new(world));
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c != (uint32_t)c);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert(((uint32_t)c - p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_recycled_children_recycled_offset_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    test_assert(p != (uint32_t)p);

    ecs_delete(world, ecs_new(world));

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c != (uint32_t)c);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert(((uint32_t)c - (uint32_t)p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_recycled_children_recycled_offset_id_different_generation(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    test_assert(p != (uint32_t)p);

    ecs_delete(world, ecs_new(world));
    ecs_delete(world, ecs_new(world));

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c != (uint32_t)c);
    test_assert((c >> 32) != (p >> 32));
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert(((uint32_t)c - (uint32_t)p) == (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_1_child_offset_id_occupied(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new(world);
    ecs_entity_t x = ecs_new(world); // Occupy instance child id

    test_assert((c - p) == (x - i));

    ecs_add_pair(world, i, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);
    test_assert(ic != x);

    test_assert((c - p) != (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_1_child_offset_id_recycled_occupied(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new(world);
    ecs_delete(world, ecs_new(world));
    ecs_entity_t x = ecs_new(world); // Occupy instance child id

    test_assert((c - p) == ((uint32_t)x - i));

    ecs_add_pair(world, i, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);
    test_assert(ic != x);
    test_assert((uint32_t)ic != (uint32_t)x);

    test_assert((c - p) != (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_child_offset_w_smaller_child_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_delete(world, a);
    ecs_delete(world, b);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    test_assert(c < p);
    ecs_set_name(world, c, "c");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t ic = ecs_lookup_from(world, i, "c");
    test_assert(ic != 0);

    test_assert((c - p) != (ic - i));

    ecs_fini(world);
}

void Prefab_prefab_ordered_children_nested(void) {
    // Implement testcase
}

void Prefab_prefab_w_union(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new(world);
    ecs_add_id(world, rel, EcsDontFragment);
    ecs_add_id(world, rel, EcsExclusive);
    ecs_entity_t tgt = ecs_new(world);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, EcsPrefab);
    ecs_add_pair(world, base, rel, tgt);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, inst, rel, tgt));

    ecs_fini(world);
}

void Prefab_prefab_child_w_union(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new(world);
    ecs_add_id(world, rel, EcsDontFragment);
    ecs_add_id(world, rel, EcsExclusive);
    ecs_entity_t tgt = ecs_new(world);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, EcsPrefab);

    ecs_entity_t base_child = ecs_entity(world, { .name = "child", .parent = base });
    ecs_add_pair(world, base_child, rel, tgt);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t inst_child = ecs_lookup_from(world, inst, "child");
    test_assert(inst_child != 0);
    test_assert(ecs_has_pair(world, inst_child, rel, tgt));

    ecs_fini(world);
}

void Prefab_prefab_w_union_and_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t rel = ecs_new(world);
    ecs_add_id(world, rel, EcsDontFragment);
    ecs_add_id(world, rel, EcsExclusive);
    ecs_entity_t tgt = ecs_new(world);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, EcsPrefab);
    ecs_add_pair(world, base, rel, tgt);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, inst, rel, tgt));

    const Position *p = ecs_get(world, inst, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Prefab_defer_instantiate_and_set_inherit_and_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});
    ecs_set(world, base, Velocity, {1, 2});

    ecs_defer_begin(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_set(world, inst, Position, {20, 30});
    ecs_set(world, inst, Velocity, {2, 3});
    test_assert(!ecs_has_pair(world, inst, EcsIsA, base));
    test_assert(!ecs_has(world, inst, Position));
    test_assert(!ecs_has(world, inst, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has_pair(world, inst, EcsIsA, base));
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_has(world, inst, Velocity));
    test_assert(ecs_owns(world, inst, Position));
    test_assert(ecs_owns(world, inst, Velocity));

    const Position *p = ecs_get(world, inst, Position);
    test_assert(p != NULL);
    test_int(p->x, 20); test_int(p->y, 30);

    const Velocity *v = ecs_get(world, inst, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 2); test_int(v->y, 3);

    ecs_fini(world);
}

void Prefab_defer_instantiate_and_set_inherit_and_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Velocity, {1, 2});

    ecs_defer_begin(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_set(world, inst, Position, {20, 30});
    ecs_set(world, inst, Velocity, {2, 3});
    test_assert(!ecs_has_pair(world, inst, EcsIsA, base));
    test_assert(!ecs_has(world, inst, Position));
    test_assert(!ecs_has(world, inst, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has_pair(world, inst, EcsIsA, base));
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_has(world, inst, Velocity));
    test_assert(ecs_owns(world, inst, Position));
    test_assert(ecs_owns(world, inst, Velocity));

    const Position *p = ecs_get(world, inst, Position);
    test_assert(p != NULL);
    test_int(p->x, 20); test_int(p->y, 30);

    const Velocity *v = ecs_get(world, inst, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 2); test_int(v->y, 3);

    ecs_fini(world);
}

void Prefab_instantiate_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has_pair(world, inst, EcsIsA, p));
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_owns(world, inst, Position));
    {
        const Position *ptr = ecs_get(world, inst, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, inst, EcsIsA, p));
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_owns(world, inst, Position));
    {
        const Position *ptr = ecs_get(world, inst, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Prefab_instantiate_w_union_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, DontFragment, Exclusive);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, p, Rel, TgtA);

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has_pair(world, inst, EcsIsA, p));
    test_assert(ecs_has_pair(world, inst, Rel, TgtA));
    test_assert(!ecs_has_pair(world, inst, Rel, TgtB));
    test_assert(ecs_get_target(world, inst, Rel, 0) == TgtA);
    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, inst, EcsIsA, p));
    test_assert(ecs_has_pair(world, inst, Rel, TgtA));
    test_assert(!ecs_has_pair(world, inst, Rel, TgtB));
    test_assert(ecs_get_target(world, inst, Rel, 0) == TgtA);

    ecs_fini(world);
}

void Prefab_instantiate_w_slot_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t slot = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add_pair(world, slot, EcsSlotOf, p);
    ecs_add(world, slot, Foo);

    ecs_defer_begin(world);
    
    ecs_defer_suspend(world);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entity_t inst_slot = ecs_get_target(world, inst, slot, 0);
    test_assert(inst_slot != 0);
    test_assert(ecs_has(world, inst_slot, Foo));

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_get_target(world, inst, slot, 0) == inst_slot);
    test_assert(ecs_has(world, inst_slot, Foo));

    ecs_fini(world);
}

void Prefab_instantiate_w_sparse_component_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    ecs_defer_resume(world);
    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Prefab_instantiate_w_sparse_tag_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ecs_add_id(world, Foo, EcsSparse);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, p, Foo);

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has(world, inst, Foo));
    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has(world, inst, Foo));

    ecs_fini(world);
}

void Prefab_instantiate_w_sparse_pair_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ECS_TAG(world, Tgt);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set_pair(world, p, Position, Tgt, {10, 20});

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    {
        const Position *p = ecs_get_pair(world, inst, Position, Tgt);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    ecs_defer_resume(world);
    ecs_defer_end(world);

    {
        const Position *p = ecs_get_pair(world, inst, Position, Tgt);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Prefab_instantiate_w_sparse_pair_tag_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);
    ecs_add_id(world, Foo, EcsSparse);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, p, Foo, Tgt);

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has_pair(world, inst, Foo, Tgt));
    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, inst, Foo, Tgt));

    ecs_fini(world);
}

void Prefab_instantiate_w_non_fragmenting_component_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    ecs_defer_resume(world);
    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, inst, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Prefab_instantiate_w_non_fragmenting_tag_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ecs_add_id(world, Foo, EcsDontFragment);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, p, Foo);

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has(world, inst, Foo));
    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has(world, inst, Foo));

    ecs_fini(world);
}

void Prefab_instantiate_w_non_fragmenting_pair_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ECS_TAG(world, Tgt);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set_pair(world, p, Position, Tgt, {10, 20});

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    {
        const Position *p = ecs_get_pair(world, inst, Position, Tgt);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    ecs_defer_resume(world);
    ecs_defer_end(world);

    {
        const Position *p = ecs_get_pair(world, inst, Position, Tgt);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Prefab_instantiate_w_non_fragmenting_pair_tag_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);
    ecs_add_id(world, Foo, EcsDontFragment);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, p, Foo, Tgt);

    ecs_defer_begin(world);
    ecs_defer_suspend(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has_pair(world, inst, Foo, Tgt));
    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, inst, Foo, Tgt));

    ecs_fini(world);
}

void Prefab_create_instances_w_override_and_ctor(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});

    for (int i = 0; i < 8; i ++) {
        ecs_entity_t i = ecs_new(world);
        ecs_add_pair(world, i, EcsIsA, p);
        const Position *p = ecs_get(world, i, Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Prefab_create_instances_w_override_and_copy_ctor(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .copy = Position_custom_copy
    });

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});

    for (int i = 0; i < 8; i ++) {
        ecs_entity_t i = ecs_new(world);
        ecs_add_pair(world, i, EcsIsA, p);
        const Position *p = ecs_get(world, i, Position);
        test_int(p->x, 12);
        test_int(p->y, 22);
    }

    ecs_fini(world);
}

void Prefab_create_instances_w_override_and_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = ecs_on_add(Position)
    });

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});

    for (int i = 0; i < 8; i ++) {
        ecs_entity_t i = ecs_new(world);
        ecs_add_pair(world, i, EcsIsA, p);
        const Position *p = ecs_get(world, i, Position);
        test_int(p->x, 0);
        test_int(p->y, 0);
    }

    ecs_fini(world);
}

void Prefab_create_instances_w_override_and_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_set = ecs_on_set(Position)
    });

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});

    for (int i = 0; i < 8; i ++) {
        ecs_entity_t i = ecs_new(world);
        ecs_add_pair(world, i, EcsIsA, p);
        const Position *p = ecs_get(world, i, Position);
        test_int(p->x, 12);
        test_int(p->y, 22);
    }

    ecs_fini(world);
}

void Prefab_remove_all(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, c, Position, {10, 20});

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
        test_assert(ecs_has(world, i, Position));
        {
            ecs_iter_t it = ecs_children(world, i);
            test_bool(true, ecs_children_next(&it));
            test_int(1, it.count);
            test_assert(ecs_has(world, it.entities[0], Position));
            test_bool(false, ecs_children_next(&it));
        }

        ecs_remove_all(world, ecs_id(Position));

        test_assert(!ecs_has(world, i, Position));
        {
            ecs_iter_t it = ecs_children(world, i);
            test_bool(true, ecs_children_next(&it));
            test_int(1, it.count);
            test_assert(!ecs_has(world, it.entities[0], Position));
            test_bool(false, ecs_children_next(&it));
        }
    }

    test_assert(ecs_has(world, p, Position));
    test_assert(ecs_has(world, c, Position));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
        test_assert(ecs_has(world, i, Position));
        {
            ecs_iter_t it = ecs_children(world, i);
            test_bool(true, ecs_children_next(&it));
            test_int(1, it.count);
            test_assert(ecs_has(world, it.entities[0], Position));
            test_bool(false, ecs_children_next(&it));
        }
    }

    ecs_delete(world, p);
    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void Prefab_delete_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, c, Position, {10, 20});

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
        test_assert(ecs_has(world, i, Position));
        {
            ecs_iter_t it = ecs_children(world, i);
            test_bool(true, ecs_children_next(&it));
            test_int(1, it.count);
            test_assert(ecs_has(world, it.entities[0], Position));
            test_bool(false, ecs_children_next(&it));
        }

        ecs_delete_with(world, ecs_id(Position));

        test_assert(!ecs_is_alive(world, i));
    }

    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, c));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
        test_assert(ecs_has(world, i, Position));
        {
            ecs_iter_t it = ecs_children(world, i);
            test_bool(true, ecs_children_next(&it));
            test_int(1, it.count);
            test_assert(ecs_has(world, it.entities[0], Position));
            test_bool(false, ecs_children_next(&it));
        }
    }

    ecs_delete(world, p);
    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}
