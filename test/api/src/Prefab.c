#include <api.h>

static
void Iter(ecs_iter_t *it) {
    ECS_COLUMN(it, Mass, m_ptr, 1);
    bool shared = ecs_is_shared(it, 1);

    ECS_COLUMN(it, Position, p, 2);

    Velocity *v = NULL;
    if (it->column_count >= 3) {
        v = ecs_column(it, Velocity, 3);
    }

    probe_system(it);

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

void Prefab_new_w_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    ecs_add(world, e_1, Velocity);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    ecs_entity_t e_2 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has_entity(world, e_2,  ECS_INSTANCEOF | Prefab));
    test_assert( !ecs_has_entity(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsName));
    test_assert( !ecs_get(world, e_2, EcsName));

    ecs_add(world, e_2, Velocity);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has_entity(world, e_2,  ECS_INSTANCEOF | Prefab));
    test_assert( !ecs_has_entity(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsName));
    test_assert( !ecs_get(world, e_2, EcsName));

    const Position *p_1 = ecs_get(world, e_1, Position);
    const Position *p_2 = ecs_get(world, e_2, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Velocity *v_1 = ecs_get(world, e_1, Velocity);
    const Velocity *v_2 = ecs_get(world, e_2, Velocity);
    
    test_assert(v_1 != NULL);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);

    ecs_fini(world);
}

void Prefab_new_w_count_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_start = ecs_bulk_new_w_entity(world, ECS_INSTANCEOF | Prefab, 10);

    int i;
    const Position *p_prev = NULL;
    for (i = e_start; i < (e_start + 10); i ++) {
        const Position *p = ecs_get(world, i, Position);
        test_assert(p != NULL);

        if (p_prev) test_ptr(p, p_prev);
        test_int(p->x, 10);
        test_int(p->y, 20);

        p_prev = p;
    }

    ecs_fini(world);
}

void Prefab_new_w_type_w_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Velocity);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));

    ecs_entity_t e_2 = ecs_new(world, Type);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has_entity(world, e_2,  ECS_INSTANCEOF | Prefab));
    test_assert( !ecs_has_entity(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsName));

    const Position *p_1 = ecs_get(world, e_1, Position);
    const Position *p_2 = ecs_get(world, e_2, Position);
    const Position *p_prefab = ecs_get(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_1 == p_prefab);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Velocity *v_1 = ecs_get(world, e_1, Velocity);
    const Velocity *v_2 = ecs_get(world, e_2, Velocity);
    
    test_assert(v_1 != NULL);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);

    ecs_fini(world);
}

void Prefab_add_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new(world, Velocity);
    test_assert(e_1 != 0);

    ecs_add_entity(world, e_1, ECS_INSTANCEOF | Prefab);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));

    ecs_entity_t e_2 = ecs_new(world, Velocity);
    test_assert(e_2 != 0);

    ecs_add_entity(world, e_2, ECS_INSTANCEOF | Prefab);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has_entity(world, e_2,  ECS_INSTANCEOF | Prefab));
    test_assert( !ecs_has_entity(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsName));

    const Position *p_1 = ecs_get(world, e_1, Position);
    const Position *p_2 = ecs_get(world, e_2, Position);
    const Position *p_prefab = ecs_get(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_prefab == p_1);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Velocity *v_1 = ecs_get(world, e_1, Velocity);
    const Velocity *v_2 = ecs_get(world, e_2, Velocity);
    
    test_assert(v_1 != NULL);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);

    ecs_fini(world);
}

void Prefab_add_type_w_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Velocity);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new(world, 0);
    test_assert(e_1 != 0);

    ecs_add(world, e_1, Type);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));

    ecs_entity_t e_2 = ecs_new(world, 0);

    ecs_add(world, e_2, Type);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has_entity(world, e_2,  ECS_INSTANCEOF | Prefab));
    test_assert( !ecs_has_entity(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsName));

    const Position *p_1 = ecs_get(world, e_1, Position);
    const Position *p_2 = ecs_get(world, e_2, Position);
    const Position *p_prefab = ecs_get(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_prefab == p_1);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Velocity *v_1 = ecs_get(world, e_1, Velocity);
    const Velocity *v_2 = ecs_get(world, e_2, Velocity);
    
    test_assert(v_1 != NULL);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);

    ecs_fini(world);
}

void Prefab_remove_prefab_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    ecs_remove_entity(world, e_1, ECS_INSTANCEOF | Prefab);
    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    ecs_fini(world);
}

void Prefab_remove_prefab_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new(world, 0);
    test_assert(e_1 != 0);

    ecs_add_entity(world, e_1, ECS_INSTANCEOF | Prefab);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    ecs_remove_entity(world, e_1, ECS_INSTANCEOF | Prefab);
    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    ecs_fini(world);
}

void Prefab_override_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e_1, Position, {20, 30});
    p = ecs_get(world, e_1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    ecs_fini(world);
}

void Prefab_override_remove_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e_1, Position, {20, 30});
    p = ecs_get(world, e_1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    ecs_remove(world, e_1, Position);
    p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_assert(p == p_prefab);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Prefab_override_2_of_3_components_1_self() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_PREFAB(world, Prefab, Position, Velocity, Mass);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});
    ecs_set(world, Prefab, Mass, {50});

    ecs_entity_t e_1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert(e_1 != 0);

    ecs_set(world, e_1, Rotation, {60});

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_1, Mass));
    test_assert( ecs_has(world, e_1, Rotation));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e_1, Position, {20, 30});
    p = ecs_get(world, e_1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_set(world, e_1, Velocity, {40, 50});
    v = ecs_get(world, e_1, Velocity);
    test_int(v->x, 40);
    test_int(v->y, 50);

    const Velocity *v_prefab = ecs_get(world, Prefab, Velocity);
    test_assert(v != v_prefab);
    test_int(v_prefab->x, 30);
    test_int(v_prefab->y, 40);

    const Mass *m = ecs_get(world, e_1, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    const Mass *m_prefab = ecs_get(world, Prefab, Mass);
    test_assert(m_prefab != NULL);
    test_ptr(m_prefab, m);

    const Rotation *r = ecs_get(world, e_1, Rotation);
    test_assert(r != NULL);
    test_int(*r, 60);

    ecs_fini(world);
}

void Prefab_new_type_w_1_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e_1, Velocity);
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

void Prefab_new_type_w_2_overrides() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position, Velocity);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e_1, Velocity);
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

void Prefab_add_type_w_1_overrides() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    ecs_entity_t e_1 = ecs_new(world, 0);
    test_assert(e_1 != 0);

    ecs_add(world, e_1, Type);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e_1, Velocity);
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

void Prefab_add_type_w_2_overrides() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position, Velocity);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    ecs_add(world, e_1, Type);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has_entity(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsName));
    test_assert( !ecs_get(world, e_1, EcsName));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position *p_prefab = ecs_get(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    const Velocity *v = ecs_get(world, e_1, Velocity);
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

void Prefab_get_ptr_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert(e_1 != 0);
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | Prefab));
    test_assert( ecs_get_w_entity(world, e_1, Prefab) == NULL);

    ecs_fini(world);
}

static
void Prefab_w_field(ecs_iter_t *it) {
    probe_system(it);

    for (int i = 0; i < it->count; i ++) {
        Position *p = ecs_field(it, Position, 1, i);
        Velocity *v = ecs_field(it, Velocity, 2, i);
        p->x += v->x;
        p->y += v->y;
    }
}

void Prefab_iterate_w_prefab_field() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);
    ECS_SYSTEM(world, Prefab_w_field, EcsOnUpdate, Position, Velocity);

    ecs_set(world, Prefab, Velocity, {1, 2});

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);
    ecs_set(world, e_1, Position, {0, 0});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_field);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], Prefab);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);    

    ecs_fini(world);
}

static
void Prefab_w_shared(ecs_iter_t *it) {
    Velocity *v = NULL;
    if (it->column_count >= 2) {
        v = ecs_column(it, Velocity, 2);
        if (v) {
            test_assert(ecs_is_shared(it, 2));
        }
    }
    
    Mass *m = NULL;
    if (it->column_count >= 3) {
        m = ecs_column(it, Mass, 3);
    }

    probe_system(it);

    for (int i = 0; i < it->count; i ++) {
        Position *p = ecs_field(it, Position, 1, i);
        p->x += v->x;
        p->y += v->y;

        if (m) {
            p->x += *m;
            p->y += *m;
        }
    }
}

void Prefab_iterate_w_prefab_shared() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);
    ECS_SYSTEM(world, Prefab_w_shared, EcsOnUpdate, Position, Velocity);

    ecs_set(world, Prefab, Velocity, {1, 2});

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);
    ecs_set(world, e_1, Position, {0, 0});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_shared);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], Prefab);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Prefab_match_entity_prefab_w_system_optional() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Velocity, Mass);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);
    ECS_SYSTEM(world, Prefab_w_shared, EcsOnUpdate, Position, Velocity, ?Mass);

    ecs_set(world, Prefab, Velocity, {1, 2});
    ecs_set(world, Prefab, Mass, {3});

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);
    ecs_set(world, e_1, Position, {0, 0});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_shared);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], Prefab);
    test_int(ctx.c[0][2], ecs_entity(Mass));
    test_int(ctx.s[0][2], Prefab);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 5);

    ecs_fini(world);
}

void Prefab_prefab_in_system_expr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab1, Velocity);
    ECS_PREFAB(world, Prefab2, Velocity);
    ECS_SYSTEM(world, Prefab_w_shared, EcsOnUpdate, Position, Velocity, Mass, INSTANCEOF | Prefab1);

    ecs_set(world, Prefab1, Velocity, {1, 2});
    ecs_set(world, Prefab2, Velocity, {1, 2});

    ecs_entity_t e_1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab1);
    ecs_entity_t e_2 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab1);
    ecs_entity_t e_3 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab2);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    ecs_set(world, e_1, Position, {0, 0});
    ecs_set(world, e_2, Position, {0, 0});
    ecs_set(world, e_3, Position, {0, 0});

    ecs_set(world, e_1, Mass, {0});
    ecs_set(world, e_2, Mass, {0});
    ecs_set(world, e_3, Mass, {0});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_shared);
    test_int(ctx.column_count, 4);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], Prefab1);
    test_int(ctx.c[0][2], ecs_entity(Mass));
    test_int(ctx.s[0][2], 0);    
    test_int(ctx.c[0][3], ECS_INSTANCEOF | Prefab1);
    test_int(ctx.s[0][3], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

static
void Dummy(ecs_iter_t *it) {
    probe_system(it);
}

void Prefab_dont_match_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Prefab_new_w_count_w_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Position, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Velocity);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});

    ecs_entity_t e_1 = ecs_bulk_new(world, Type, 100);
    test_assert(e_1 != 0);

    const Position *prefab_p = ecs_get(world, Prefab, Position);
    const Velocity *prefab_v = ecs_get(world, Prefab, Velocity);

    ecs_entity_t e;
    for (e = e_1; e < (e_1 + 100); e ++) {
        test_assert( ecs_has(world, e, Position));
        test_assert( ecs_has(world, e, Velocity));
        test_assert( ecs_has_entity(world, e, ECS_INSTANCEOF | Prefab));

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

void Prefab_override_2_components_different_size() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Color);
    ECS_PREFAB(world, Prefab, Position, Velocity, Color);
    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Velocity, Color);

    ecs_set(world, Prefab, Position, {10, 20});
    ecs_set(world, Prefab, Velocity, {30, 40});
    ecs_set(world, Prefab, Color, {1, 2, 3, 4});

    // TODO: investigate crash when replacing ecs_bulk_new with 
    // ecs_new_w_entity_w_count
    ecs_entity_t e_1 = ecs_bulk_new(world, Type, 100);
    test_assert(e_1 != 0);

    const Position *prefab_p = ecs_get(world, Prefab, Position);
    const Velocity *prefab_v = ecs_get(world, Prefab, Velocity);
    const Color *prefab_c = ecs_get(world, Prefab, Color);

    test_assert(prefab_p != NULL);
    test_assert(prefab_v != NULL);
    test_assert(prefab_c != NULL);

    ecs_entity_t e;
    for (e = e_1; e < (e_1 + 100); e ++) {
        test_assert( ecs_has(world, e, Position));
        test_assert( ecs_has(world, e, Velocity));
        test_assert( ecs_has_entity(world, e, ECS_INSTANCEOF | Prefab));

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

void Prefab_ignore_prefab_parent_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, parent, EcsPrefab, Position);
    ECS_ENTITY(world, child, EcsPrefab, CHILDOF | parent, Velocity);

    test_assert( ecs_has_entity(world, child, ECS_CHILDOF | parent));
    test_assert( !ecs_has(world, child, Position));
    test_assert( ecs_has(world, child, Velocity));

    ecs_fini(world);
}

static
void Move(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].y;
        p[i].y += v[i].y;
    }
}

static
void AddVelocity(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Velocity, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Velocity, {1, 1});
    }
}

void Prefab_match_table_created_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {10});

    ECS_ENTITY(world, e, INSTANCEOF | Prefab, Position);

    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, !Velocity);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity, Mass);

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

void Prefab_prefab_w_1_child() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Parent, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_PREFAB(world, Child, CHILDOF | Parent, Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p_e = ecs_get(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);

    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

    const Position *p_child = ecs_get(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    ecs_entity_t e2 = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
    test_assert(e2 != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_get(world, e2, Position) == p_e);

    ecs_entity_t e_child2 = ecs_lookup_child(world, e2, "Child");
    test_assert(e_child2 != 0);
    test_assert(ecs_has(world, e_child2, Position));
    test_assert(ecs_has_entity(world, e_child2, ECS_CHILDOF | e2));
    test_assert( ecs_get(world, e_child2, Position) != p_child);
    
    p_child = ecs_get(world, e_child2, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    ecs_fini(world);
}

void Prefab_prefab_w_2_children() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child1, EcsPrefab, CHILDOF | Parent, Position);
            ecs_set(world, Child1, Position, {2, 3});

        ECS_ENTITY(world, Child2, EcsPrefab, CHILDOF | Parent, Position);
            ecs_set(world, Child2, Position, {3, 4});

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child1");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    e_child = ecs_lookup_child(world, e, "Child2");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Prefab_prefab_w_grandchild() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, CHILDOF | Parent, Position);
            ecs_set(world, Child, Position, {2, 3});

            ECS_ENTITY(world, GrandChild, EcsPrefab, CHILDOF | Parent.Child, Position, Rotation);
                ecs_set(world, GrandChild, Position, {3, 4});

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

    p = ecs_get(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_entity_t e_grandchild = ecs_lookup_child(world, e_child, "GrandChild");
    test_assert(e_grandchild != 0);
    test_assert(ecs_has(world, e_grandchild, Position));
    test_assert(ecs_has_entity(world, e_grandchild, ECS_CHILDOF | e_child));

    p = ecs_get(world, e_grandchild, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);
}

void Prefab_prefab_w_base_w_child() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, Base, EcsPrefab, Velocity);
        ecs_set(world, Base, Velocity, {3, 4});

    ECS_ENTITY(world, Parent, EcsPrefab, INSTANCEOF | Base, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, CHILDOF | Parent, Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
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
    test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

    const Position *p_child = ecs_get(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);    
}

void Prefab_prefab_w_child_w_base() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, Base, EcsPrefab, Velocity);
        ecs_set(world, Base, Velocity, {3, 4});

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, CHILDOF | Parent, INSTANCEOF | Base, Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
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
    test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

    const Position *p_child = ecs_get(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    const Velocity *v_child = ecs_get(world, e_child, Velocity);
    test_assert(v_child != NULL);
    test_int(v_child->x, 3);
    test_int(v_child->y, 4);     
}

void Prefab_prefab_w_child_w_base_w_children() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, Base, EcsPrefab, Velocity);
        ecs_set(world, Base, Velocity, {3, 4});

        ECS_ENTITY(world, BaseChild, EcsPrefab, CHILDOF | Base, Position);
            ecs_set(world, BaseChild, Position, {4, 5});

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, CHILDOF | Parent, INSTANCEOF | Base, Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
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
    test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

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
    test_assert(ecs_has_entity(world, e_base_child, ECS_CHILDOF | e_child)); 

    const Position *p_base_child = ecs_get(world, e_base_child, Position);
    test_assert(p_base_child != NULL);
    test_assert(p_base_child != p_child);
    test_int(p_base_child->x, 4);
    test_int(p_base_child->y, 5);
}

void Prefab_prefab_w_child_new_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, CHILDOF | Parent, Position);
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_bulk_new_w_entity(world, ECS_INSTANCEOF | Parent, 3);
    test_assert(e != 0);

    int end = e + 3;
    for (; e < end; e ++) {
        test_assert( ecs_has(world, e, Position));

        const Position *p_e = ecs_get(world, e, Position);
        test_assert(p_e != NULL);
        test_int(p_e->x, 1);
        test_int(p_e->y, 2);

        ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
        test_assert(e_child != 0);
        test_assert(ecs_has(world, e_child, Position));
        test_assert(ecs_has_entity(world, e_child, ECS_CHILDOF | e));

        const Position *p_child = ecs_get(world, e_child, Position);
        test_assert(p_child != NULL);
        test_int(p_child->x, 2);
        test_int(p_child->y, 3);
    }

    ecs_fini(world);
}

void Prefab_prefab_auto_override_child_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, ChildPrefab, EcsPrefab, Position, Velocity);
            ecs_set(world, ChildPrefab, Position, {2, 3});
            ecs_set(world, ChildPrefab, Velocity, {4, 5});

        ECS_ENTITY(world, Child, EcsPrefab, CHILDOF | Parent, INSTANCEOF | ChildPrefab, Velocity);

    ecs_entity_t e1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);
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
    test_assert( !ecs_has(world, child_1, EcsType));

    ecs_entity_t child_2 = ecs_lookup_child(world, e2, "Child");
    test_assert(child_2 != 0);
    test_assert( ecs_has(world, child_2, Position));
    test_assert( ecs_has(world, child_2, Velocity));
    test_assert( !ecs_has(world, child_2, EcsType));

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

void Prefab_ignore_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, PrefabReactiveTest, EcsOnAdd, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_fini(world);
}

void Prefab_ignore_on_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, PrefabReactiveTest, EcsOnRemove, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_remove(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_fini(world);
}

void Prefab_ignore_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, PrefabReactiveTest, EcsOnSet, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_set(world, Prefab, Position, {0, 0});

    test_int(invoked, 0);

    ecs_fini(world);
}

void Prefab_on_set_on_instance() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, PrefabReactiveTest, EcsOnSet, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_set(world, Prefab, Position, {1, 2});

    test_int(invoked, 0);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
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
    ECS_COLUMN_ENTITY(it, Prefab, 2);

    ecs_entity_t *ids = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ids[i] = ecs_new_w_entity(it->world, ECS_INSTANCEOF | Prefab);
    }
}

void Prefab_instantiate_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, InstantiateInProgress, EcsOnUpdate, Position, :Prefab);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    ecs_entity_t ids[10];
    ecs_set_context(world, ids);

    ecs_progress(world, 1);

    test_int(ecs_count_entity(world, ECS_INSTANCEOF | Prefab), 10);

    const Velocity *v_prefab = ecs_get(world, Prefab, Velocity);

    int i;
    for (i = 0; i < 10; i ++) {
        const Velocity *v = ecs_get(world, ids[i], Velocity);
        test_assert(v == v_prefab);
    }

    ecs_fini(world);
}

void NewInProgress(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Type, 2);

    ecs_entity_t *ids = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ids[i] = ecs_new(it->world, Type);
    }
}

void Prefab_copy_from_prefab_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Velocity);

    ECS_SYSTEM(world, NewInProgress, EcsOnUpdate, Position, :Type);

    ecs_entity_t ids[10];
    ecs_set_context(world, ids);

    ecs_entity_t e_dummy = ecs_bulk_new(world, Position, 10);
    test_assert(e_dummy != 0);

    /* Create one prefab instance so table is already created (case where table
     * is not created is tested in copy_from_prefab_first_instance_in_progress*/
    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    ecs_progress(world, 1);

    test_int(ecs_count_entity(world, ECS_INSTANCEOF | Prefab), 11);

    int i;
    for (i = 0; i < 10; i ++) {
        const Velocity *v = ecs_get(world, ids[i], Velocity);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void Prefab_copy_from_prefab_first_instance_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Velocity);

    ECS_SYSTEM(world, NewInProgress, EcsOnUpdate, Position, :Type);

    ecs_entity_t ids[10];
    ecs_set_context(world, ids);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    ecs_progress(world, 1);

    test_int(ecs_count_entity(world, ECS_INSTANCEOF | Prefab), 10);

    int i;
    for (i = 0; i < 10; i ++) {
        const Velocity *v = ecs_get(world, ids[i], Velocity);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void Prefab_ref_after_realloc() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_ENTITY(world, e_1, Position, Mass);
    ecs_set(world, e_1, Mass, {3});

    ECS_ENTITY(world, e_2, Position, INSTANCEOF | Prefab);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass, Position);

    ecs_type_t prefab_type = ecs_get_type(world, Prefab);

    /* Trigger a realloc of the table in which the prefab is stored. This should
     * cause systems with refs to re-resolve their cached ref ptrs  */
    ecs_bulk_new_w_type(world, prefab_type, 1000, NULL);
    
    ecs_set(world, Prefab, Mass, {2});
    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Prefab_revalidate_ref_w_mixed_table_refs() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_ENTITY(world, e_1, Position, INSTANCEOF | Prefab);
    ECS_ENTITY(world, e_2, Position, Mass);
    ecs_set(world, e_2, Mass, {3});

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass, Position);

    ecs_type_t prefab_type = ecs_get_type(world, Prefab);

    /* Trigger a realloc of the table in which the prefab is stored. This should
     * cause systems with refs to re-resolve their cached ref ptrs  */
    ecs_bulk_new_w_type(world, prefab_type, 1000, NULL);
    
    ecs_set(world, Prefab, Mass, {2});
    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Prefab_no_overwrite_on_2nd_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_ENTITY(world, e_1, Position, INSTANCEOF | Prefab);

    ecs_add(world, e_1, Mass);
    test_int( *ecs_get(world, e_1, Mass), 1);

    ecs_set(world, e_1, Mass, {2});
    test_int( *ecs_get(world, e_1, Mass), 2);

    ecs_add(world, e_1, Mass);
    test_int( *ecs_get(world, e_1, Mass), 2);

    ecs_fini(world);
}

void AddPrefab(ecs_iter_t *it) {
    ECS_COLUMN_ENTITY(it, Prefab, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_entity(it->world, it->entities[i], ECS_INSTANCEOF | Prefab);
    }
}

void Prefab_no_overwrite_on_2nd_add_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, :Prefab);

    ECS_ENTITY(world, e_1, Position, INSTANCEOF | Prefab);
    ecs_add(world, e_1, Mass);
    test_int( *ecs_get(world, e_1, Mass), 1);

    ecs_set(world, e_1, Mass, {2});
    test_int( *ecs_get(world, e_1, Mass), 2);

    ecs_progress(world, 1);

    test_int( *ecs_get(world, e_1, Mass), 2);

    ecs_fini(world);
}

void Prefab_no_instantiate_on_2nd_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, CHILDOF | Prefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, :Prefab);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert( ecs_has_entity(world, e, ECS_INSTANCEOF | Prefab));
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

    ecs_add_entity(world, e, ECS_INSTANCEOF | Prefab);

    ecs_entity_t e_child_2 = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child == e_child_2);
    v = ecs_get(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);
    
    ecs_fini(world);
}

void Prefab_no_instantiate_on_2nd_add_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, CHILDOF | Prefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, :Prefab);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert( ecs_has_entity(world, e, ECS_INSTANCEOF | Prefab));
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
    ecs_entity_t *result = ecs_get_context(it->world);
    ECS_COLUMN_ENTITY(it, Prefab, 1);
    *result = ecs_bulk_new_w_entity(it->world, ECS_INSTANCEOF | Prefab, 3);
}

void Prefab_nested_prefab_in_progress_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, CHILDOF | Prefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, NewPrefab_w_count, EcsOnUpdate, :Prefab);

    ecs_entity_t result = 0;
    ecs_set_context(world, &result);

    ecs_progress(world, 1);

    test_assert(result != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_has_entity(world, result + i, ECS_INSTANCEOF | Prefab);

        const Position *p = ecs_get(world, result + i, Position);
        test_assert(p != NULL); 
        test_int(p->x, 1);
        test_int(p->y, 2);

        ecs_entity_t child = ecs_lookup_child(world, result + i, "ChildPrefab");
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
    ECS_COLUMN(it, Velocity, v, 1);

    on_set_velocity_invoked ++;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);

        Velocity *staged_v = ecs_get_mut(it->world, it->entities[i], Velocity, NULL);
        test_assert(ecs_get(it->world, it->entities[i], Velocity) == staged_v);

        staged_v->x = v->x + 1;
        staged_v->y = v->y + 1;
    }
}

void Prefab_nested_prefab_in_progress_w_count_set_after_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, CHILDOF | Prefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, NewPrefab_w_count, EcsOnUpdate, :Prefab);
    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    ecs_entity_t result = 0;
    ecs_set_context(world, &result);

    ecs_progress(world, 1);

    test_assert(result != 0);
    test_int(on_set_velocity_invoked, 3);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_has_entity(world, result + i, ECS_INSTANCEOF | Prefab);

        const Position *p = ecs_get(world, result + i, Position);
        test_assert(p != NULL); 
        test_int(p->x, 1);
        test_int(p->y, 2);

        ecs_entity_t child = ecs_lookup_child(world, result + i, "ChildPrefab");
        test_assert(child != 0);

        const Velocity *v = ecs_get(world, child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3 + 1);
        test_int(v->y, 4 + 1);
    }
    
    ecs_fini(world);
}

void AddPrefabInProgress(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Prefab, 2);
    ECS_COLUMN_COMPONENT(it, Velocity, 3);

    ecs_entity_t Prefab = ecs_type_to_entity(it->world, ecs_type(Prefab));

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_entity(it->world, ECS_INSTANCEOF | it->entities[i], Prefab);
        test_assert( ecs_has(it->world, it->entities[i], Prefab));
        test_assert( ecs_has(it->world, it->entities[i], Velocity));
        
        const Velocity *v = ecs_get(it->world, it->entities[i], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);

        test_assert( ecs_get(it->world, Prefab, Velocity) == v);
    }
}

void Prefab_get_ptr_from_prefab_from_new_table_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Velocity);
        ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, AddPrefabInProgress, EcsOnUpdate, Position, :Prefab, :Velocity);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);

    ecs_progress(world, 1);

    test_assert( ecs_has_entity(world, e, ECS_INSTANCEOF | Prefab));
    test_assert( ecs_has(world, e, Velocity));
    
    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    test_assert( ecs_get(world, Prefab, Velocity) == v);

    ecs_fini(world);
}

void TestBase(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    test_assert(ecs_is_shared(it, 2));

    test_assert(p != NULL);
    test_assert(v != NULL);
    test_int(it->count, 1);
}

void Prefab_match_base() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Base, Velocity);
        ecs_set(world, Base, Velocity, {1, 2});

    ECS_PREFAB(world, Child, INSTANCEOF | Base);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Child);
    ecs_add(world, e, Position);

    ECS_SYSTEM(world, TestBase, EcsOnUpdate, Position, Velocity);

    ecs_progress(world, 1);

    ecs_fini(world);
}

static
void AddMass(ecs_iter_t *it) {
    ECS_COLUMN_ENTITY(it, Mass, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Mass);
    }
}

void Prefab_match_base_after_add_in_prev_phase() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_PREFAB(world, Base, Velocity);
        ecs_set(world, Base, Velocity, {1, 2});

    ECS_PREFAB(world, Child, INSTANCEOF | Base);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Child);
    ecs_add(world, e, Position);

    ECS_SYSTEM(world, AddMass, EcsPreUpdate, Position, !Mass);
    ECS_SYSTEM(world, TestBase, EcsOnUpdate, Position, Velocity);

    ecs_progress(world, 1);

    ecs_fini(world);
}

void Prefab_override_watched_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Create a system that listens for Position */
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    /* Create a prefab with Position */
    ECS_PREFAB(world, Prefab, Position);

    /* Create an instance of Prefab, this will cause Prefab to be watched since
     * it will be matched as reference with the system */
    ECS_ENTITY(world, Entity1, INSTANCEOF | Prefab);

    /* Another instance of Prefab is created, prefab data is resolved to check
     * if components need to be overridden. Index will be negative, so code 
     * needs to flip sign on the index, or this will fail. */
    ECS_ENTITY(world, Entity2, INSTANCEOF | Prefab, Position);

    const Position *p1 = ecs_get(world, Prefab, Position);
    const Position *p2 = ecs_get(world, Entity1, Position);
    const Position *p3 = ecs_get(world, Entity2, Position);

    test_assert(p1 == p2);
    test_assert(p2 != p3);

    ecs_fini(world);
}

void Prefab_rematch_twice() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position, Velocity, Mass);

    ECS_PREFAB(world, Prefab, Position);
    ECS_ENTITY(world, Entity, INSTANCEOF | Prefab);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

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
void Inherit(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Position, 1);

    ecs_entity_t *param = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t e_backup = ecs_new(it->world, Position);
        test_assert(e_backup != 0);
        test_assert( ecs_has(it->world, e_backup, Position));

        ecs_add_entity(it->world, e, ECS_INSTANCEOF | e_backup);
        test_assert( ecs_has_entity(it->world, e, e_backup | ECS_INSTANCEOF));

        *param = e_backup;
    }
}

void Prefab_inherit_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);

    ECS_SYSTEM(world, Inherit, EcsOnUpdate, Position);

    ecs_entity_t e_backup;
    ecs_set_context(world, &e_backup);

    ecs_progress(world, 1);

    test_assert(e_backup != 0);
    test_assert( ecs_has(world, e_backup, Position));
    test_assert( ecs_has_entity(world, e1, e_backup | ECS_INSTANCEOF));

    ecs_fini(world);
}

static
void AddPosition(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Position, 1);
    
    ecs_entity_t *base = ecs_get_context(it->world);

    ecs_add(it->world, *base, Position);
}

void Prefab_add_to_empty_base_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new(world, 0);
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_entity(world, e1, ECS_INSTANCEOF | base);

    ECS_SYSTEM(world, AddPosition, EcsOnUpdate, :Position);

    ecs_set_context(world, &base);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, base, Position));

    ecs_fini(world);
}

void Prefab_dont_inherit_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Velocity);
    
    ecs_add_entity(world, e_2, EcsDisabled);
    ecs_add_entity(world, e_1, ECS_INSTANCEOF | e_2);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( !ecs_has_entity(world, e_1, EcsDisabled));
    test_assert( ecs_has_entity(world, e_1, e_2 | ECS_INSTANCEOF));
    test_assert( !ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has_entity(world, e_2, EcsDisabled));

    ecs_fini(world);
}

static bool has_cloned = false;

static
void CloneInOnAdd(ecs_iter_t *it)
{
    ECS_COLUMN(it, Position, p, 1);

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
            test_assert( ecs_has(it->world, clone, Position));

            const Position *p_clone = ecs_get(it->world, clone, Position);
            test_int(p_clone->x, 10);
            test_int(p_clone->y, 20);

            ecs_add_entity(it->world, ECS_INSTANCEOF | clone, e);
        }
    }
}

void Prefab_clone_after_inherit_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, CloneInOnAdd, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(has_cloned == true);

    ecs_fini(world);
}

void Prefab_override_from_nested() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ECS_PREFAB(world, BasePrefab, Position);
    ecs_set(world, BasePrefab, Position, {10, 20});

    ECS_PREFAB(world, SubPrefab, INSTANCEOF | BasePrefab, Velocity);
    ecs_set(world, SubPrefab, Velocity, {30, 40});

    ECS_TYPE(world, Sub, INSTANCEOF | SubPrefab, Position, Velocity);

    ecs_entity_t e_1 = ecs_new(world, Sub);
    test_assert(e_1 != 0);
    test_assert( ecs_has_owned(world, e_1, Position, true));
    test_assert( ecs_has_owned(world, e_1, Velocity, true));
    test_assert( ecs_has_owned(world, e_1, Position, true));
    test_assert( ecs_has_owned(world, e_1, Velocity, true));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | SubPrefab));
    test_assert( ecs_has_entity(world, e_1, ECS_INSTANCEOF | BasePrefab));

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

static
void OnAddEntity(ecs_iter_t *it) {
    ECS_COLUMN(it, ecs_entity_t, e, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        e[i] = it->entities[i];
    }
}

void Prefab_create_multiple_nested_w_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, ecs_entity_t);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);

        ECS_PREFAB(world, Child, CHILDOF | Prefab, Velocity, ecs_entity_t);
            ecs_set(world, Child, Velocity, {3, 4});

    ECS_SYSTEM(world, OnAddEntity, EcsOnSet, ecs_entity_t);
    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    test_int(on_set_velocity_invoked, 0);

    /* Create two entities with separate calls */ 
    ecs_entity_t e1 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    ecs_entity_t e2 = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);

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
    test_assert( ecs_has_owned(world, child1, Velocity, true));
    test_assert( ecs_has_owned(world, child2, Velocity, true));

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
    ECS_COLUMN_ENTITY(it, Prefab, 1);

    new_instance_1 = ecs_new_w_entity(it->world, ECS_INSTANCEOF | Prefab);
    new_instance_2 = ecs_new_w_entity(it->world, ECS_INSTANCEOF | Prefab);
}

void Prefab_create_multiple_nested_w_on_set_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, ecs_entity_t);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});
            
        ECS_PREFAB(world, Child, CHILDOF | Prefab, INSTANCEOF | ChildPrefab, ecs_entity_t);

    ECS_SYSTEM(world, CreateInstances, EcsOnUpdate, :Prefab);
    ECS_SYSTEM(world, OnAddEntity, EcsOnSet, ecs_entity_t);
    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

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
    test_assert( ecs_has_owned(world, child1, Velocity, true));
    test_assert( ecs_has_owned(world, child2, Velocity, true));

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

void Prefab_single_on_set_on_child_w_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, ecs_entity_t);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

        ECS_PREFAB(world, Child, CHILDOF | Prefab, INSTANCEOF | ChildPrefab, Velocity);

    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    test_int(on_set_velocity_invoked, 0);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);

    /* Make sure OnSet is invoked only once, even though Child overrides a
     * component from a ChildPrefab. Velocity is already overridden for Child,
     * and while it would technically not be incorrect to issue an OnSet for the
     * override, we don't want to spam the application with redundant triggers. */
    test_int(on_set_velocity_invoked, 1);

    test_assert( ecs_has(world, e, Position));

    ecs_entity_t child = ecs_lookup_child(world, e, "Child");
    test_assert(child != 0);

    test_assert( ecs_has(world, child, Velocity));
    test_assert( ecs_has_owned(world, child, Velocity, true));

    const Velocity *v = ecs_get(world, child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);

    ecs_fini(world);
}
