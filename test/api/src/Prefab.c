#include <api.h>

static
void Iter(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Mass, m_ptr, 1);
    bool shared = ecs_is_shared(rows, 1);

    ECS_COLUMN(rows, Position, p, 2);

    Velocity *v = NULL;
    if (rows->column_count >= 3) {
        v = ecs_column(rows, Velocity, 3);
    }

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
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

    ecs_entity_t e_1 = ecs_new_instance(world, Prefab, 0);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    ecs_add(world, e_1, Velocity);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_1, Prefab));
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    ecs_entity_t e_2 = ecs_new_instance(world, Prefab, 0);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Prefab));
    test_assert( !ecs_has(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsId));
    test_assert( !ecs_get_ptr(world, e_2, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_2, EcsId));

    ecs_add(world, e_2, Velocity);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Prefab));
    test_assert( !ecs_has(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsId));
    test_assert( !ecs_get_ptr(world, e_2, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_2, EcsId));

    Position *p_1 = ecs_get_ptr(world, e_1, Position);
    Position *p_2 = ecs_get_ptr(world, e_2, Position);
    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_prefab != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_1 == p_prefab);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    Velocity *v_1 = ecs_get_ptr(world, e_1, Velocity);
    Velocity *v_2 = ecs_get_ptr(world, e_2, Velocity);
    
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

    ecs_entity_t e_start = ecs_new_instance_w_count(world, Prefab, 0, 10);

    int i;
    Position *p_prev = NULL;
    for (i = e_start; i < (e_start + 10); i ++) {
        Position *p = ecs_get_ptr(world, i, Position);
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
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));

    ecs_entity_t e_2 = ecs_new(world, Type);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Prefab));
    test_assert( !ecs_has(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsId));

    Position *p_1 = ecs_get_ptr(world, e_1, Position);
    Position *p_2 = ecs_get_ptr(world, e_2, Position);
    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_1 == p_prefab);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    Velocity *v_1 = ecs_get_ptr(world, e_1, Velocity);
    Velocity *v_2 = ecs_get_ptr(world, e_2, Velocity);
    
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

    ecs_inherit(world, e_1, Prefab);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));

    ecs_entity_t e_2 = ecs_new(world, Velocity);
    test_assert(e_2 != 0);

    ecs_inherit(world, e_2, Prefab);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Prefab));
    test_assert( !ecs_has(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsId));

    Position *p_1 = ecs_get_ptr(world, e_1, Position);
    Position *p_2 = ecs_get_ptr(world, e_2, Position);
    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_prefab == p_1);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    Velocity *v_1 = ecs_get_ptr(world, e_1, Velocity);
    Velocity *v_2 = ecs_get_ptr(world, e_2, Velocity);
    
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
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));

    ecs_entity_t e_2 = ecs_new(world, 0);

    ecs_add(world, e_2, Type);
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Prefab));
    test_assert( !ecs_has(world, e_2, EcsPrefab));
    test_assert( !ecs_has(world, e_2, EcsId));

    Position *p_1 = ecs_get_ptr(world, e_1, Position);
    Position *p_2 = ecs_get_ptr(world, e_2, Position);
    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);

    test_assert(p_1 != NULL);
    test_assert(p_2 != NULL);
    test_assert(p_1 == p_2);
    test_assert(p_prefab == p_1);

    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    Velocity *v_1 = ecs_get_ptr(world, e_1, Velocity);
    Velocity *v_2 = ecs_get_ptr(world, e_2, Velocity);
    
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

    ecs_entity_t e_1 = ecs_new_instance(world, Prefab, 0);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    ecs_disinherit(world, e_1, Prefab);
    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    ecs_fini(world);
}

void Prefab_remove_prefab_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new(world, 0);
    test_assert(e_1 != 0);

    ecs_inherit(world, e_1, Prefab);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    ecs_disinherit(world, e_1, Prefab);
    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    ecs_fini(world);
}

void Prefab_override_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_PREFAB(world, Prefab, Position);

    ecs_set(world, Prefab, Position, {10, 20});

    ecs_entity_t e_1 = ecs_new_instance(world, Prefab, 0);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e_1, Position, {20, 30});
    p = ecs_get_ptr(world, e_1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);
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

    ecs_entity_t e_1 = ecs_new_instance(world, Prefab, 0);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e_1, Position, {20, 30});
    p = ecs_get_ptr(world, e_1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    ecs_remove(world, e_1, Position);
    p = ecs_get_ptr(world, e_1, Position);
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

    ecs_entity_t e_1 = ecs_new_instance(world, Prefab, 0);
    test_assert(e_1 != 0);

    ecs_set(world, e_1, Rotation, {60});

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_1, Mass));
    test_assert( ecs_has(world, e_1, Rotation));
    test_assert( ecs_has(world, e_1, Prefab));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e_1, Position, {20, 30});
    p = ecs_get_ptr(world, e_1, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_set(world, e_1, Velocity, {40, 50});
    v = ecs_get_ptr(world, e_1, Velocity);
    test_int(v->x, 40);
    test_int(v->y, 50);

    Velocity *v_prefab = ecs_get_ptr(world, Prefab, Velocity);
    test_assert(v != v_prefab);
    test_int(v_prefab->x, 30);
    test_int(v_prefab->y, 40);

    Mass *m = ecs_get_ptr(world, e_1, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    Mass *m_prefab = ecs_get_ptr(world, Prefab, Mass);
    test_assert(m_prefab != NULL);
    test_ptr(m_prefab, m);

    Rotation *r = ecs_get_ptr(world, e_1, Rotation);
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
    test_assert( ecs_has(world, e_1, Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    Velocity *v_prefab = ecs_get_ptr(world, Prefab, Velocity);
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
    test_assert( ecs_has(world, e_1, Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    Velocity *v_prefab = ecs_get_ptr(world, Prefab, Velocity);
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
    test_assert( ecs_has(world, e_1, Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    Velocity *v_prefab = ecs_get_ptr(world, Prefab, Velocity);
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
    test_assert( ecs_has(world, e_1, Prefab));
    test_assert( ecs_has(world, e_1, Type));
    /* These components should never be inherited from prefabs */
    test_assert( !ecs_has(world, e_1, EcsPrefab));
    test_assert( !ecs_has(world, e_1, EcsId));
    test_assert( !ecs_get_ptr(world, e_1, EcsPrefab));
    test_assert( !ecs_get_ptr(world, e_1, EcsId));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position *p_prefab = ecs_get_ptr(world, Prefab, Position);
    test_assert(p_prefab != NULL);
    test_assert(p != p_prefab);
    test_int(p_prefab->x, 10);
    test_int(p_prefab->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    Velocity *v_prefab = ecs_get_ptr(world, Prefab, Velocity);
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

    ecs_entity_t e_1 = ecs_new_instance(world, Prefab, 0);
    test_assert(e_1 != 0);
    test_assert( ecs_has(world, e_1, Prefab));
    test_assert( _ecs_get_ptr(world, e_1, ecs_type(Prefab)) == NULL);

    ecs_fini(world);
}

static
void Prefab_w_field(ecs_rows_t *rows) {
    ProbeSystem(rows);

    for (int i = 0; i < rows->count; i ++) {
        Position *p = ecs_field(rows, Position, 1, i);
        Velocity *v = ecs_field(rows, Velocity, 2, i);
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

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);    

    ecs_fini(world);
}

static
void Prefab_w_shared(ecs_rows_t *rows) {
    Velocity *v = NULL;
    if (rows->column_count >= 2) {
        v = ecs_column(rows, Velocity, 2);
        if (v) {
            test_assert(ecs_is_shared(rows, 2));
        }
    }
    
    Mass *m = NULL;
    if (rows->column_count >= 3) {
        m = ecs_column(rows, Mass, 3);
    }

    ProbeSystem(rows);

    for (int i = 0; i < rows->count; i ++) {
        Position *p = ecs_field(rows, Position, 1, i);
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

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e_1, Position);
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

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 5);

    ecs_fini(world);
}

void Prefab_prefab_in_system_expr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab1, Velocity);
    ECS_PREFAB(world, Prefab2, Velocity);
    ECS_SYSTEM(world, Prefab_w_shared, EcsOnUpdate, Position, Velocity, Prefab1);

    ecs_set(world, Prefab1, Velocity, {1, 2});
    ecs_set(world, Prefab2, Velocity, {1, 2});

    ecs_entity_t e_1 = ecs_new_instance(world, Prefab1, 0);
    ecs_entity_t e_2 = ecs_new_instance(world, Prefab1, 0);
    ecs_entity_t e_3 = ecs_new_instance(world, Prefab2, 0);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    ecs_set(world, e_1, Position, {0, 0});
    ecs_set(world, e_2, Position, {0, 0});
    ecs_set(world, e_3, Position, {0, 0});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Prefab_w_shared);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], Prefab1);
    test_int(ctx.c[0][2], Prefab1);
    test_int(ctx.s[0][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

static
void Dummy(ecs_rows_t *rows) {
    ProbeSystem(rows);
}

void Prefab_dont_match_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    SysTestData ctx = {0};
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

    ecs_entity_t e_1 = ecs_new_w_count(world, Type, 100);
    test_assert(e_1 != 0);

    Position *prefab_p = ecs_get_ptr(world, Prefab, Position);
    Velocity *prefab_v = ecs_get_ptr(world, Prefab, Velocity);

    ecs_entity_t e;
    for (e = e_1; e < (e_1 + 100); e ++) {
        test_assert( ecs_has(world, e, Position));
        test_assert( ecs_has(world, e, Velocity));
        test_assert( ecs_has(world, e, Prefab));

        Position *p = ecs_get_ptr(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_assert(p == prefab_p);

        Velocity *v = ecs_get_ptr(world, e, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 30);
        test_int(v->y, 40);
        test_assert(v != prefab_v);
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

    // TODO: investigate crash when replacing ecs_new_w_count with 
    // ecs_new_instance_w_count
    ecs_entity_t e_1 = ecs_new_w_count(world, Type, 100);
    test_assert(e_1 != 0);

    Position *prefab_p = ecs_get_ptr(world, Prefab, Position);
    Velocity *prefab_v = ecs_get_ptr(world, Prefab, Velocity);
    Color *prefab_c = ecs_get_ptr(world, Prefab, Color);

    test_assert(prefab_p != NULL);
    test_assert(prefab_v != NULL);
    test_assert(prefab_c != NULL);

    ecs_entity_t e;
    for (e = e_1; e < (e_1 + 100); e ++) {
        test_assert( ecs_has(world, e, Position));
        test_assert( ecs_has(world, e, Velocity));
        test_assert( ecs_has(world, e, Prefab));

        Position *p = ecs_get_ptr(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_assert(p == prefab_p);

        Velocity *v = ecs_get_ptr(world, e, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 30);
        test_int(v->y, 40);
        test_assert(v != prefab_v);

        Color *c = ecs_get_ptr(world, e, Color);
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
    ECS_ENTITY(world, child, EcsPrefab, Velocity);

    ecs_set(world, child, EcsPrefab, {.parent = parent});

    test_assert( ecs_contains(world, parent, child));
    test_assert( !ecs_has(world, child, Position));
    test_assert( ecs_has(world, child, Velocity));

    ecs_fini(world);
}

static
void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x += v[i].y;
        p[i].y += v[i].y;
    }
}

static
void AddVelocity(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], Velocity, {1, 1});
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

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 0);

    ecs_progress(world, 1);

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 1);

    ecs_fini(world);
}

void Prefab_prefab_w_1_child() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, Position);
            ecs_set(world, Child, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_instance(world, Parent, 0);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    Position *p_e = ecs_get_ptr(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);

    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_contains(world, e, e_child));

    Position *p_child = ecs_get_ptr(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    ecs_entity_t e2 = ecs_new_instance(world, Parent, 0);
    test_assert(e2 != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_get_ptr(world, e2, Position) == p_e);

    ecs_entity_t e_child2 = ecs_lookup_child(world, e2, "Child");
    test_assert(e_child2 != 0);
    test_assert(ecs_has(world, e_child2, Position));
    test_assert(ecs_contains(world, e2, e_child2));
    test_assert( ecs_get_ptr(world, e_child2, Position) == p_child);

    ecs_fini(world);
}

void Prefab_prefab_w_2_children() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child1, EcsPrefab, Position);
            ecs_set(world, Child1, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child1, Position, {2, 3});

        ECS_ENTITY(world, Child2, EcsPrefab, Position);
            ecs_set(world, Child2, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child2, Position, {3, 4});

    ecs_entity_t e = ecs_new_instance(world, Parent, 0);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child1");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_contains(world, e, e_child));

    p = ecs_get_ptr(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    e_child = ecs_lookup_child(world, e, "Child2");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_contains(world, e, e_child));

    p = ecs_get_ptr(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Prefab_prefab_w_grandchild() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, Position);
            ecs_set(world, Child, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child, Position, {2, 3});

            ECS_ENTITY(world, GrandChild, EcsPrefab, Position);
                ecs_set(world, GrandChild, EcsPrefab, {.parent = Child});
                ecs_set(world, GrandChild, Position, {3, 4});

    ecs_entity_t e = ecs_new_instance(world, Parent, 0);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_contains(world, e, e_child));

    p = ecs_get_ptr(world, e_child, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_entity_t e_grandchild = ecs_lookup_child(world, e_child, "GrandChild");
    test_assert(e_grandchild != 0);
    test_assert(ecs_has(world, e_grandchild, Position));
    test_assert(ecs_contains(world, e_child, e_grandchild));

    p = ecs_get_ptr(world, e_grandchild, Position);
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

        ECS_ENTITY(world, Child, EcsPrefab, Position);
            ecs_set(world, Child, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_instance(world, Parent, 0);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    Position *p_e = ecs_get_ptr(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);

    Velocity *v_e = ecs_get_ptr(world, e, Velocity);
    test_assert(v_e != NULL);
    test_int(v_e->x, 3);
    test_int(v_e->y, 4);    

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(!ecs_has(world, e_child, Velocity));
    test_assert(ecs_contains(world, e, e_child));

    Position *p_child = ecs_get_ptr(world, e_child, Position);
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

        ECS_ENTITY(world, Child, EcsPrefab, INSTANCEOF | Base, Position);
            ecs_set(world, Child, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_instance(world, Parent, 0);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    Position *p_e = ecs_get_ptr(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);   

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has(world, e_child, Velocity));
    test_assert(ecs_contains(world, e, e_child));

    Position *p_child = ecs_get_ptr(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    Velocity *v_child = ecs_get_ptr(world, e_child, Velocity);
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

        ECS_ENTITY(world, BaseChild, EcsPrefab, Position);
            ecs_set(world, BaseChild, EcsPrefab, {.parent = Base});
            ecs_set(world, BaseChild, Position, {4, 5});

    ECS_ENTITY(world, Parent, EcsPrefab, Position);
        ecs_set(world, Parent, Position, {1, 2});

        ECS_ENTITY(world, Child, EcsPrefab, INSTANCEOF | Base, Position);
            ecs_set(world, Child, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_instance(world, Parent, 0);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    Position *p_e = ecs_get_ptr(world, e, Position);
    test_assert(p_e != NULL);
    test_int(p_e->x, 1);
    test_int(p_e->y, 2);   

    ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
    test_assert(e_child != 0);
    test_assert(ecs_has(world, e_child, Position));
    test_assert(ecs_has(world, e_child, Velocity));
    test_assert(ecs_contains(world, e, e_child));

    Position *p_child = ecs_get_ptr(world, e_child, Position);
    test_assert(p_child != NULL);
    test_int(p_child->x, 2);
    test_int(p_child->y, 3);

    Velocity *v_child = ecs_get_ptr(world, e_child, Velocity);
    test_assert(v_child != NULL);
    test_int(v_child->x, 3);
    test_int(v_child->y, 4); 

    ecs_entity_t e_base_child = ecs_lookup_child(world, e_child, "BaseChild");
    test_assert(e_base_child != 0);
    test_assert(ecs_has(world, e_base_child, Position));
    test_assert(!ecs_has(world, e_base_child, Velocity));
    test_assert(ecs_contains(world, e_child, e_base_child)); 

    Position *p_base_child = ecs_get_ptr(world, e_base_child, Position);
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

        ECS_ENTITY(world, Child, EcsPrefab, Position);
            ecs_set(world, Child, EcsPrefab, {.parent = Parent});
            ecs_set(world, Child, Position, {2, 3});

    ecs_entity_t e = ecs_new_instance_w_count(world, Parent, 0, 3);
    test_assert(e != 0);

    int end = e + 3;
    for (; e < end; e ++) {
        test_assert( ecs_has(world, e, Position));

        Position *p_e = ecs_get_ptr(world, e, Position);
        test_assert(p_e != NULL);
        test_int(p_e->x, 1);
        test_int(p_e->y, 2);

        ecs_entity_t e_child = ecs_lookup_child(world, e, "Child");
        test_assert(e_child != 0);
        test_assert(ecs_has(world, e_child, Position));
        test_assert(ecs_contains(world, e, e_child));

        Position *p_child = ecs_get_ptr(world, e_child, Position);
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
        ECS_TYPE(world, Child, INSTANCEOF | ChildPrefab, Velocity);
            ecs_set(world, Child, EcsPrefab, {.parent = Parent});
            ecs_set(world, ChildPrefab, Position, {2, 3});
            ecs_set(world, ChildPrefab, Velocity, {4, 5});

    ecs_entity_t e1 = ecs_new_instance(world, Parent, 0);
    test_assert(e1 != 0);
    test_assert( ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_new_instance(world, Parent, 0);
    test_assert(e2 != 0);
    test_assert( ecs_has(world, e2, Position));

    Position *p1 = ecs_get_ptr(world, e1, Position);
    test_assert(p1 != NULL);
    test_int(p1->x, 1);
    test_int(p1->y, 2);

    Position *p2 = ecs_get_ptr(world, e2, Position);
    test_assert(p2 != NULL);
    test_int(p2->x, 1);
    test_int(p2->y, 2);    
    test_assert(p1 == p2);

    ecs_entity_t child_1 = ecs_lookup_child(world, e1, "Child");
    test_assert(child_1 != 0);

    test_assert( ecs_has(world, child_1, Position));
    test_assert( ecs_has(world, child_1, Velocity));
    test_assert( !ecs_has(world, child_1, EcsTypeComponent));

    ecs_entity_t child_2 = ecs_lookup_child(world, e2, "Child");
    test_assert(child_2 != 0);
    test_assert( ecs_has(world, child_2, Position));
    test_assert( ecs_has(world, child_2, Velocity));
    test_assert( !ecs_has(world, child_2, EcsTypeComponent));

    Position *child_p1 = ecs_get_ptr(world, child_1, Position);
    test_assert(child_p1 != NULL);
    test_int(child_p1->x, 2);
    test_int(child_p1->y, 3);

    Position *child_p2 = ecs_get_ptr(world, child_2, Position);
    test_assert(child_p2 != NULL);
    test_int(child_p2->x, 2);
    test_int(child_p2->y, 3);   
    test_assert(child_p1 == child_p2); 

    Velocity *child_v1 = ecs_get_ptr(world, child_1, Velocity);
    test_assert(child_v1 != NULL);
    test_int(child_v1->x, 4);
    test_int(child_v1->y, 5);

    Velocity *child_v2 = ecs_get_ptr(world, child_2, Velocity);
    test_assert(child_v2 != NULL);
    test_int(child_v2->x, 4);
    test_int(child_v2->y, 5); 
    test_assert(child_v1 != child_v2);

    ecs_fini(world);
}

static int invoked;

void PrefabReactiveTest(ecs_rows_t *rows) {
    invoked ++;
}

void Prefab_ignore_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, PrefabReactiveTest, EcsOnAdd, Position);

    ECS_PREFAB(world, Prefab, Position);

    test_int(invoked, 0);

    ecs_fini(world);
}

void Prefab_ignore_on_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, PrefabReactiveTest, EcsOnRemove, Position);

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

    ecs_entity_t e = ecs_new_instance(world, Prefab, 0);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_int(invoked, 1);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void InstantiateInProgress(ecs_rows_t *rows) {
    ECS_COLUMN_ENTITY(rows, Prefab, 2);

    ecs_entity_t *ids = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ids[i] = ecs_new_instance(rows->world, Prefab, 0);
    }
}

void Prefab_instantiate_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, InstantiateInProgress, EcsOnUpdate, Position, .Prefab);

    ecs_entity_t e = ecs_new_w_count(world, Position, 10);
    test_assert(e != 0);

    ecs_entity_t ids[10];
    ecs_set_context(world, ids);

    ecs_progress(world, 1);

    test_int(ecs_count(world, Prefab), 10);

    Velocity *v_prefab = ecs_get_ptr(world, Prefab, Velocity);

    int i;
    for (i = 0; i < 10; i ++) {
        Velocity *v = ecs_get_ptr(world, ids[i], Velocity);
        test_assert(v == v_prefab);
    }

    ecs_fini(world);
}

void NewInProgress(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Type, 2);

    ecs_entity_t *ids = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ids[i] = ecs_new(rows->world, Type);
    }
}

void Prefab_copy_from_prefab_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Velocity);
    ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Velocity);

    ECS_SYSTEM(world, NewInProgress, EcsOnUpdate, Position, .Type);

    ecs_entity_t ids[10];
    ecs_set_context(world, ids);

    ecs_entity_t e_dummy = ecs_new_w_count(world, Position, 10);
    test_assert(e_dummy != 0);

    /* Create one prefab instance so table is already created (case where table
     * is not created is tested in copy_from_prefab_first_instance_in_progress*/
    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    ecs_progress(world, 1);

    test_int(ecs_count(world, Prefab), 11);

    int i;
    for (i = 0; i < 10; i ++) {
        Velocity *v = ecs_get_ptr(world, ids[i], Velocity);
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

    ECS_SYSTEM(world, NewInProgress, EcsOnUpdate, Position, .Type);

    ecs_entity_t ids[10];
    ecs_set_context(world, ids);

    ecs_entity_t e = ecs_new_w_count(world, Position, 10);
    test_assert(e != 0);

    ecs_progress(world, 1);

    test_int(ecs_count(world, Prefab), 10);

    int i;
    for (i = 0; i < 10; i ++) {
        Velocity *v = ecs_get_ptr(world, ids[i], Velocity);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void Prefab_cached_ptr_after_realloc() {
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
    _ecs_new_w_count(world, prefab_type, 1000);
    
    ecs_set(world, Prefab, Mass, {2});
    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Prefab_revalidate_cached_ptr_w_mixed_table_refs() {
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
    _ecs_new_w_count(world, prefab_type, 1000);
    
    ecs_set(world, Prefab, Mass, {2});
    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get_ptr(world, e_1, Position);
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
    test_int( ecs_get(world, e_1, Mass), 1);

    ecs_set(world, e_1, Mass, {2});
    test_int( ecs_get(world, e_1, Mass), 2);

    ecs_add(world, e_1, Mass);
    test_int( ecs_get(world, e_1, Mass), 2);

    ecs_fini(world);
}

void AddPrefab(ecs_rows_t *rows) {
    ECS_COLUMN_ENTITY(rows, Prefab, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_inherit(rows->world, rows->entities[i], Prefab);
    }
}

void Prefab_no_overwrite_on_2nd_add_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Mass);
    ecs_set(world, Prefab, Mass, {1});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, .Prefab);

    ECS_ENTITY(world, e_1, Position, INSTANCEOF | Prefab);
    ecs_add(world, e_1, Mass);
    test_int( ecs_get(world, e_1, Mass), 1);

    ecs_set(world, e_1, Mass, {2});
    test_int( ecs_get(world, e_1, Mass), 2);

    ecs_progress(world, 1);

    test_int( ecs_get(world, e_1, Mass), 2);

    ecs_fini(world);
}

void Prefab_no_instantiate_on_2nd_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, EcsPrefab, {.parent = Prefab});
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, .Prefab);

    ecs_entity_t e = ecs_new_instance(world, Prefab, 0);
    test_assert( ecs_has(world, e, Prefab));
    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child != 0);
    Velocity *v = ecs_get_ptr(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    ecs_set(world, e_child, Velocity, {4, 5});
    v = ecs_get_ptr(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);

    ecs_inherit(world, e, Prefab);

    ecs_entity_t e_child_2 = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child == e_child_2);
    v = ecs_get_ptr(world, e_child, Velocity);
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

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, EcsPrefab, {.parent = Prefab});
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, AddPrefab, EcsOnUpdate, Position, .Prefab);

    ecs_entity_t e = ecs_new_instance(world, Prefab, 0);
    test_assert( ecs_has(world, e, Prefab));
    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t e_child = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child != 0);
    Velocity *v = ecs_get_ptr(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    ecs_set(world, e_child, Velocity, {4, 5});
    v = ecs_get_ptr(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);

    ecs_progress(world, 1);

    ecs_entity_t e_child_2 = ecs_lookup_child(world, e, "ChildPrefab");
    test_assert(e_child == e_child_2);
    v = ecs_get_ptr(world, e_child, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 4);
    test_int(v->y, 5);
    
    ecs_fini(world);
}

void NewPrefab_w_count(ecs_rows_t *rows) {
    ecs_entity_t *result = ecs_get_context(rows->world);
    ECS_COLUMN_ENTITY(rows, Prefab, 1);
    *result = ecs_new_instance_w_count(rows->world, Prefab, 0, 3);
}

void Prefab_nested_prefab_in_progress_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, EcsPrefab, {.parent = Prefab});
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, NewPrefab_w_count, EcsOnUpdate, .Prefab);

    ecs_entity_t result = 0;
    ecs_set_context(world, &result);

    ecs_progress(world, 1);

    test_assert(result != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_has(world, result + i, Prefab);

        Position *p = ecs_get_ptr(world, result + i, Position);
        test_assert(p != NULL); 
        test_int(p->x, 1);
        test_int(p->y, 2);

        ecs_entity_t child = ecs_lookup_child(world, result + i, "ChildPrefab");
        test_assert(child != 0);

        Velocity *v = ecs_get_ptr(world, child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
    }
    
    ecs_fini(world);
}

static int on_set_velocity_invoked;

static
void OnSetVelocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 1);
    ECS_COLUMN_COMPONENT(rows, Velocity, 1);

    on_set_velocity_invoked ++;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], Velocity);
        Velocity *staged_v = ecs_get_ptr(rows->world, rows->entities[i], Velocity);
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

        ECS_PREFAB(world, ChildPrefab, Velocity);
            ecs_set(world, ChildPrefab, EcsPrefab, {.parent = Prefab});
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, NewPrefab_w_count, EcsOnUpdate, .Prefab);
    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    ecs_entity_t result = 0;
    ecs_set_context(world, &result);

    ecs_progress(world, 1);

    test_assert(result != 0);
    test_int(on_set_velocity_invoked, 1);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_has(world, result + i, Prefab);

        Position *p = ecs_get_ptr(world, result + i, Position);
        test_assert(p != NULL); 
        test_int(p->x, 1);
        test_int(p->y, 2);

        ecs_entity_t child = ecs_lookup_child(world, result + i, "ChildPrefab");
        test_assert(child != 0);

        Velocity *v = ecs_get_ptr(world, child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3 + 1);
        test_int(v->y, 4 + 1);
    }
    
    ecs_fini(world);
}

void AddPrefabInProgress(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Prefab, 2);
    ECS_COLUMN_COMPONENT(rows, Velocity, 3);

    ecs_entity_t Prefab = ecs_type_to_entity(rows->world, ecs_type(Prefab));

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_inherit(rows->world, rows->entities[i], Prefab);
        test_assert( ecs_has(rows->world, rows->entities[i], Prefab));
        test_assert( ecs_has(rows->world, rows->entities[i], Velocity));
        
        Velocity *v = ecs_get_ptr(rows->world, rows->entities[i], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);

        test_assert( ecs_get_ptr(rows->world, Prefab, Velocity) == v);
    }
}

void Prefab_get_ptr_from_prefab_from_new_table_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Prefab, Velocity);
        ecs_set(world, Prefab, Velocity, {1, 2});

    ECS_SYSTEM(world, AddPrefabInProgress, EcsOnUpdate, Position, .Prefab, .Velocity);

    ecs_entity_t e = ecs_new_instance(world, Prefab, 0);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Prefab));
    test_assert( ecs_has(world, e, Velocity));
    
    Velocity *v = ecs_get_ptr(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    test_assert( ecs_get_ptr(world, Prefab, Velocity) == v);

    ecs_fini(world);
}

void TestBase(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    test_assert(ecs_is_shared(rows, 2));

    test_assert(p != NULL);
    test_assert(v != NULL);
    test_int(rows->count, 1);
}

void Prefab_match_base() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Base, Velocity);
        ecs_set(world, Base, Velocity, {1, 2});

    ECS_PREFAB(world, Child, INSTANCEOF | Base);

    ecs_entity_t e = ecs_new_instance(world, Child, 0);
    ecs_add(world, e, Position);

    ECS_SYSTEM(world, TestBase, EcsOnUpdate, Position, Velocity);

    ecs_progress(world, 1);

    ecs_fini(world);
}

static
void AddMass(ecs_rows_t *rows) {
    ECS_COLUMN_ENTITY(rows, Mass, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], Mass);
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

    ecs_entity_t e = ecs_new_instance(world, Child, 0);
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

    Position *p1 = ecs_get_ptr(world, Prefab, Position);
    Position *p2 = ecs_get_ptr(world, Entity1, Position);
    Position *p3 = ecs_get_ptr(world, Entity2, Position);

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

    SysTestData ctx = {0};
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
void Inherit(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    ecs_entity_t *param = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        ecs_entity_t e_backup = ecs_new(rows->world, Position);
        test_assert(e_backup != 0);
        test_assert( ecs_has(rows->world, e_backup, Position));

        ecs_inherit(rows->world, e, e_backup);
        test_assert( ecs_has_entity(rows->world, e, e_backup | ECS_INSTANCEOF));

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
void AddPosition(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);
    
    ecs_entity_t *base = ecs_get_context(rows->world);

    ecs_add(rows->world, *base, Position);
}

void Prefab_add_to_empty_base_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new(world, 0);
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_inherit(world, e1, base);

    ECS_SYSTEM(world, AddPosition, EcsOnUpdate, .Position);

    ecs_set_context(world, &base);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, base, Position));

    ecs_fini(world);
}

void Prefab_cyclic_inheritance() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e_1, Velocity, {1, 2});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {30, 40});
    ecs_set(world, e_2, Velocity, {3, 4});

    ecs_inherit(world, e_1, e_2);
    ecs_inherit(world, e_2, e_1);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has_entity(world, e_1, e_2 | ECS_INSTANCEOF));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has_entity(world, e_2, e_1 | ECS_INSTANCEOF));

    /* Override Position from e_2 */
    ecs_remove(world, e_1, Position);
    ecs_add(world, e_1, Position);
    Position *p = ecs_get_ptr(world, e_1, Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    /* Override Velocity from e_1 */
    ecs_remove(world, e_2, Velocity);
    ecs_add(world, e_2, Velocity);
    Velocity *v = ecs_get_ptr(world, e_2, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

typedef struct Backup {
    ecs_entity_t backup;
} Backup;

void CloneInProgress(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Backup, 2);
    ECS_COLUMN_COMPONENT(rows, Position, 3);
    ECS_COLUMN_COMPONENT(rows, Velocity, 4);

    ecs_world_t *world = rows->world;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];

        ecs_entity_t backup = ecs_clone(world, e, true);
        test_assert( ecs_has(world, backup, Position));
        test_assert( ecs_has(world, backup, Velocity));

        ecs_inherit(world, e, backup);
        ecs_inherit(world, backup, e);

        test_assert( ecs_has_entity(world, e, backup));
        test_assert( ecs_has_entity(world, backup, e));

        ecs_set(world, e, Backup, { backup });
    }
}

void Prefab_cyclic_inheritance_w_clone_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Backup);
    ECS_TAG(world, NeedsBackup);

    ECS_SYSTEM(world, CloneInProgress, EcsOnUpdate, NeedsBackup, !Backup, .Position, .Velocity);

    ecs_entity_t e = ecs_new(world, NeedsBackup);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Backup));
    Backup *b = ecs_get_ptr(world, e, Backup);
    test_assert(b != NULL);

    ecs_entity_t backup = b->backup;
    test_assert(backup != 0);
    test_assert( ecs_has_owned(world, backup, Position));
    test_assert( ecs_has_owned(world, backup, Velocity));
    test_assert( ecs_has_entity(world, backup, e));

    ecs_fini(world);
}

void Prefab_dont_inherit_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Velocity);
    
    ecs_add(world, e_2, EcsDisabled);
    ecs_inherit(world, e_1, e_2);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( !ecs_has(world, e_1, EcsDisabled));
    test_assert( ecs_has_entity(world, e_1, e_2 | ECS_INSTANCEOF));
    test_assert( !ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, EcsDisabled));

    ecs_fini(world);
}

void Prefab_cyclic_inheritance_get_unavailable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);

    ecs_inherit(world, e1, e2);
    ecs_inherit(world, e2, e1);

    Velocity *v = ecs_get_ptr(world, e1, Velocity);
    test_assert(v == NULL);

    ecs_fini(world);
}

void Prefab_cyclic_inheritance_has_unavailable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);

    ecs_inherit(world, e1, e2);
    ecs_inherit(world, e2, e1);

    test_assert(!ecs_has(world, e1, Velocity));

    ecs_fini(world);
}

static bool has_cloned = false;

static
void CloneInOnAdd(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        bool has_cloned_test = has_cloned;

        /* Prevent recursive OnAdd calls */
        has_cloned = true;

        p[i].x = 10;
        p[i].y = 20;

        if (!has_cloned_test) {
            ecs_entity_t clone = ecs_clone(rows->world, rows->entities[i], true);
            test_assert( ecs_has(rows->world, clone, Position));

            Position *p_clone = ecs_get_ptr(rows->world, clone, Position);
            test_int(p_clone->x, 10);
            test_int(p_clone->y, 20);

            ecs_inherit(rows->world, clone, rows->entities[i]);
        }
    }
}

void Prefab_clone_after_inherit_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, CloneInOnAdd, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    Position *p = ecs_get_ptr(world, e, Position);
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
    test_assert( ecs_has_owned(world, e_1, Position));
    test_assert( ecs_has_owned(world, e_1, Velocity));
    test_assert( ecs_has_owned(world, e_1, Position));
    test_assert( ecs_has_owned(world, e_1, Velocity));
    test_assert( ecs_has(world, e_1, SubPrefab));
    test_assert( ecs_has(world, e_1, BasePrefab));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

static
void OnAddEntity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, ecs_entity_t, e, 1);
    ECS_COLUMN_COMPONENT(rows, ecs_entity_t, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        e[i] = rows->entities[i];
    }
}

void Prefab_create_multiple_nested_w_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, ecs_entity_t);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
        ECS_TYPE(world, Child, INSTANCEOF | ChildPrefab, ecs_entity_t);
            ecs_set(world, Child, EcsPrefab, {.parent = Prefab});
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, OnAddEntity, EcsOnAdd, ecs_entity_t);
    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    /* Create two entities with separate calls */ 
    ecs_entity_t e1 = ecs_new_instance(world, Prefab, 0);
    ecs_entity_t e2 = ecs_new_instance(world, Prefab, 0);

    test_int(on_set_velocity_invoked, 2);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));

    ecs_entity_t child1 = ecs_lookup_child(world, e1, "Child");
    test_assert(child1 != 0);

    ecs_entity_t child2 = ecs_lookup_child(world, e2, "Child");
    test_assert(child2 != 0);
    test_assert(child1 != child2);

    ecs_entity_t *self1 = ecs_get_ptr(world, child1, ecs_entity_t);
    test_assert(self1 != NULL);
    test_assert(*self1 == child1);

    ecs_entity_t *self2 = ecs_get_ptr(world, child2, ecs_entity_t);
    test_assert(self2 != NULL);
    test_assert(self1 != self2);
    test_assert(*self2 == child2);

    test_assert( ecs_has(world, child1, Velocity));
    test_assert( ecs_has(world, child2, Velocity));
    test_assert( ecs_has_owned(world, child1, Velocity));
    test_assert( ecs_has_owned(world, child2, Velocity));

    Velocity *v1 = ecs_get_ptr(world, child1, Velocity);
    test_assert(v1 != NULL);
    test_int(v1->x, 4);
    test_int(v1->y, 5);

    Velocity *v2 = ecs_get_ptr(world, child2, Velocity);
    test_assert(v2 != NULL);
    test_assert(v1 != v2);
    test_int(v2->x, 4);
    test_int(v2->y, 5);

    ecs_fini(world);
}

static ecs_entity_t new_instance_1, new_instance_2;

static
void CreateInstances(ecs_rows_t *rows) {
    ECS_COLUMN_ENTITY(rows, Prefab, 1);

    new_instance_1 = ecs_new_instance(rows->world, Prefab, 0);
    new_instance_2 = ecs_new_instance(rows->world, Prefab, 0);
}

void Prefab_create_multiple_nested_w_on_add_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, ecs_entity_t);

    ECS_PREFAB(world, Prefab, Position);
        ecs_set(world, Prefab, Position, {1, 2});

        ECS_PREFAB(world, ChildPrefab, Velocity);
        ECS_TYPE(world, Child, INSTANCEOF | ChildPrefab, ecs_entity_t);
            ecs_set(world, Child, EcsPrefab, {.parent = Prefab});
            ecs_set(world, ChildPrefab, Velocity, {3, 4});

    ECS_SYSTEM(world, CreateInstances, EcsOnUpdate, .Prefab);
    ECS_SYSTEM(world, OnAddEntity, EcsOnAdd, ecs_entity_t);
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

    ecs_entity_t *self1 = ecs_get_ptr(world, child1, ecs_entity_t);
    test_assert(self1 != NULL);
    test_assert(*self1 == child1);

    ecs_entity_t *self2 = ecs_get_ptr(world, child2, ecs_entity_t);
    test_assert(self2 != NULL);
    test_assert(self1 != self2);
    test_assert(*self2 == child2);

    test_assert( ecs_has(world, child1, Velocity));
    test_assert( ecs_has(world, child2, Velocity));
    test_assert( ecs_has_owned(world, child1, Velocity));
    test_assert( ecs_has_owned(world, child2, Velocity));

    Velocity *v1 = ecs_get_ptr(world, child1, Velocity);
    test_assert(v1 != NULL);
    test_int(v1->x, 4);
    test_int(v1->y, 5);

    Velocity *v2 = ecs_get_ptr(world, child2, Velocity);
    test_assert(v2 != NULL);
    test_assert(v1 != v2);
    test_int(v2->x, 4);
    test_int(v2->y, 5);

    ecs_fini(world);
}
