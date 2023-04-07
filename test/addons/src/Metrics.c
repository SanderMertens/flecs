#include <addons.h>

void Metrics_member_gauge_1_entity() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_member_gauge_2_entities() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 30);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_member_gauge_2_entities_1_existing() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 30);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_member_gauge_2_entities_update() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_set(world, e1, Position, {15, 25});
    ecs_set(world, e2, Position, {25, 35});

    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 25);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 35);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_gauge_w_remove() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove(world, e1, Position);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_gauge_w_clear() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e1);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_gauge_w_delete() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e1);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_gauge_1_entity() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new(world, Foo);

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 1);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_gauge_2_entities() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_entity_t e2 = ecs_new(world, Foo);

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 1);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 1);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_gauge_2_entities_1_existing() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e2 = ecs_new(world, Foo);

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 1);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 1);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_gauge_w_remove() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_entity_t e2 = ecs_new(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);

    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 1);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove(world, e1, Foo);
    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_gauge_w_clear() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_entity_t e2 = ecs_new(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);

    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 1);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e1);
    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_gauge_w_delete() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_entity_t e2 = ecs_new(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);

    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 1);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e1);
    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_oneof_gauge_3_entities() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .id_targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);
    ecs_entity_t e2 = ecs_new_w_pair(world, Color, Green);
    ecs_entity_t e3 = ecs_new_w_pair(world, Color, Blue);

    ecs_progress(world, 0);

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetricKind, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    ecs_iter_t it = ecs_filter_iter(world, f);
    test_bool(true, ecs_filter_next(&it));
    test_uint(it.count, 3);
    {
        ecs_entity_t e = it.entities[0];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(((double*)inst)[0], 1);
        test_int(((double*)inst)[1], 0);
        test_int(((double*)inst)[2], 0);
    }
    {
        ecs_entity_t e = it.entities[1];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(((double*)inst)[0], 0);
        test_int(((double*)inst)[1], 1);
        test_int(((double*)inst)[2], 0);
    }
    {
        ecs_entity_t e = it.entities[2];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e3);
        test_int(((double*)inst)[0], 0);
        test_int(((double*)inst)[1], 0);
        test_int(((double*)inst)[2], 1);
    }
    test_bool(false, ecs_filter_next(&it));

    ecs_fini(world);
}

void Metrics_oneof_gauge_3_entities_1_existing() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .id_targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Color, Green);
    ecs_entity_t e3 = ecs_new_w_pair(world, Color, Blue);

    ecs_progress(world, 0);

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetricKind, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    ecs_iter_t it = ecs_filter_iter(world, f);
    test_bool(true, ecs_filter_next(&it));
    test_uint(it.count, 3);
    {
        ecs_entity_t e = it.entities[0];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(((double*)inst)[0], 1);
        test_int(((double*)inst)[1], 0);
        test_int(((double*)inst)[2], 0);
    }
    {
        ecs_entity_t e = it.entities[1];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(((double*)inst)[0], 0);
        test_int(((double*)inst)[1], 1);
        test_int(((double*)inst)[2], 0);
    }
    {
        ecs_entity_t e = it.entities[2];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e3);
        test_int(((double*)inst)[0], 0);
        test_int(((double*)inst)[1], 0);
        test_int(((double*)inst)[2], 1);
    }
    test_bool(false, ecs_filter_next(&it));

    ecs_fini(world);
}

void Metrics_oneof_gauge_w_remove() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));
    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .id_targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);
    ecs_entity_t e2 = ecs_new_w_pair(world, Color, Green);
    ecs_entity_t e3 = ecs_new_w_pair(world, Color, Blue);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    
    ecs_progress(world, 0);

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetricKind, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 3);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[1];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[2];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_remove_pair(world, e2, Color, EcsWildcard);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[1];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_fini(world);
}

void Metrics_oneof_gauge_w_clear() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));
    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .id_targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);
    ecs_entity_t e2 = ecs_new_w_pair(world, Color, Green);
    ecs_entity_t e3 = ecs_new_w_pair(world, Color, Blue);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    
    ecs_progress(world, 0);

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetricKind, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 3);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[1];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[2];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_clear(world, e2);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[1];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_fini(world);
}

void Metrics_oneof_gauge_w_delete() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));
    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .id_targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);
    ecs_entity_t e2 = ecs_new_w_pair(world, Color, Green);
    ecs_entity_t e3 = ecs_new_w_pair(world, Color, Blue);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    
    ecs_progress(world, 0);

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetricKind, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 3);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[1];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[2];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_delete(world, e2);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        {
            ecs_entity_t e = it.entities[1];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_counter() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsCounter
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_progress(world, 1.0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_auto_counter() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup_fullpath(world, "Position.y"),
        .kind = EcsCounterIncrement
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_progress(world, 1.0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 40);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_counter() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsCounter
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new(world, Foo);

    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }
    
    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetricKind, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricInstance *inst = ecs_get(world, e, EcsMetricInstance);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 2);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_oneof_counter() {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .id_targets = true,
        .kind = EcsCounter
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);

    ecs_progress(world, 1.0);

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetricKind, EcsCounter) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_add_pair(world, e1, Color, Green);
    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 0);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_add_pair(world, e1, Color, Blue);
    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_filter_iter(world, f);
        test_bool(true, ecs_filter_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricInstance)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 2);
        }
        test_bool(false, ecs_filter_next(&it));
    }

    ecs_fini(world);
}
