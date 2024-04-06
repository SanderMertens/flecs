#include <addons.h>

void Metrics_member_gauge_1_entity(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_member_gauge_2_entities(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 30);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_member_gauge_2_entities_1_existing(void) {
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

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 30);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_member_gauge_2_entities_update(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 25);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 35);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_gauge_w_remove(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_gauge_w_clear(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_gauge_w_delete(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_progress(world, 0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 30);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_gauge_1_entity(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w(world, Foo);

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 1);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_gauge_2_entities(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 1);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 1);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_gauge_2_entities_1_existing(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new_w(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e2 = ecs_new_w(world, Foo);

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 2);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 1);
    }
    {
        ecs_entity_t e = it.entities[1];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e2);
        test_int(inst->value, 1);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_gauge_w_remove(void) {
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

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);

    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 1);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_gauge_w_clear(void) {
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

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);

    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 1);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_gauge_w_delete(void) {
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

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);

    ecs_progress(world, 0);

    {    
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 1);
        }
        {
            ecs_entity_t e = it.entities[1];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e2);
            test_int(inst->value, 1);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_oneof_gauge_3_entities(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);
    ecs_entity_t e2 = ecs_new_w_pair(world, Color, Green);
    ecs_entity_t e3 = ecs_new_w_pair(world, Color, Blue);

    ecs_progress(world, 0);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetric, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_uint(it.count, 3);
    {
        ecs_entity_t e = it.entities[0];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e3);
        test_int(((double*)inst)[0], 0);
        test_int(((double*)inst)[1], 0);
        test_int(((double*)inst)[2], 1);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Metrics_oneof_gauge_3_entities_1_existing(void) {
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
        .targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Color, Green);
    ecs_entity_t e3 = ecs_new_w_pair(world, Color, Blue);

    ecs_progress(world, 0);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetric, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_uint(it.count, 3);
    {
        ecs_entity_t e = it.entities[0];
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
        const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e3);
        test_int(((double*)inst)[0], 0);
        test_int(((double*)inst)[1], 0);
        test_int(((double*)inst)[2], 1);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Metrics_oneof_gauge_w_remove(void) {
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
        .targets = true,
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

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetric, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 3);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e2, Color, EcsWildcard);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(f);

    ecs_fini(world);
}

void Metrics_oneof_gauge_w_clear(void) {
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
        .targets = true,
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

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetric, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 3);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_clear(world, e2);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(f);

    ecs_fini(world);
}

void Metrics_oneof_gauge_w_delete(void) {
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
        .targets = true,
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

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetric, EcsGauge) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 3);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e2);
    ecs_progress(world, 0);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 2);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
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
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e3);
            test_int(((double*)inst)[0], 0);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(f);

    ecs_fini(world);
}

void Metrics_oneof_to_snake_case(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, RedColor,    (ChildOf, Color));
    ECS_ENTITY(world, Green_Color, (ChildOf, Color));
    ECS_ENTITY(world, Blue,        (ChildOf, Color));
    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .targets = true,
        .kind = EcsGauge
    });
    test_assert(m != 0);

    const EcsStruct *s = ecs_get(world, m, EcsStruct);
    test_assert(s != NULL);
    test_int(s->members.count, 3);

    ecs_member_t *members = s->members.array;
    test_str(members[0].name, "red_color");
    test_str(members[1].name, "green_color");
    test_str(members[2].name, "blue");

    ecs_fini(world);
}

void Metrics_member_counter(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsCounter
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_progress(world, 1.0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_member_auto_counter(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsCounterIncrement
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_progress(world, 1.0);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 40);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_counter(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Foo);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.has_foo" }),
        .id = Foo,
        .kind = EcsCounter
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w(world, Foo);

    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
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
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounter));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 2);
        }
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_oneof_counter(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_ENTITY(world, Color, OneOf, Exclusive);
    ECS_ENTITY(world, Red,   (ChildOf, Color));
    ECS_ENTITY(world, Green, (ChildOf, Color));
    ECS_ENTITY(world, Blue,  (ChildOf, Color));

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .targets = true,
        .kind = EcsCounter
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Color, Red);

    ecs_progress(world, 1.0);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_childof(m) },
            { .id = ecs_pair(EcsMetric, EcsCounter) }
        }
    });
    test_assert(f != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 0);
            test_int(((double*)inst)[2], 0);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Color, Green);
    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 0);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Color, Blue);
    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_progress(world, 1.0);

    {
        ecs_iter_t it = ecs_query_iter(world, f);
        test_bool(true, ecs_query_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const void *inst = ecs_get_id(world, e, ecs_pair(m, ecs_id(EcsMetricValue)));
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(((double*)inst)[0], 1);
            test_int(((double*)inst)[1], 1);
            test_int(((double*)inst)[2], 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(f);

    ecs_fini(world);
}

void Metrics_metric_description(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge,
        .brief = "Position y"
    });
    test_assert(m != 0);

    test_str(ecs_doc_get_brief(world, m), "Position y");

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_count(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position" }),
        .id = ecs_id(Position),
        .kind = EcsCounterId
    });
    test_assert(m != 0);

    ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_progress(world, 1);

    {
        const EcsMetricValue *v = ecs_get(world, m, EcsMetricValue);
        test_assert(v != NULL);
        test_int(v->value, 1);
    }

    ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_progress(world, 1);

    {
        const EcsMetricValue *v = ecs_get(world, m, EcsMetricValue);
        test_assert(v != NULL);
        test_int(v->value, 4);
    }

    ecs_fini(world);
}

void Metrics_id_target_count(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsMetrics);

    ECS_TAG(world, Color);
    ECS_TAG(world, Red);
    ECS_TAG(world, Green);
    ECS_TAG(world, Blue);

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.color" }),
        .id = ecs_pair(Color, EcsWildcard),
        .targets = true,
        .kind = EcsCounterId
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, Color, Red);
    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, Color, Green);
    ecs_add_pair(world, e2, Color, Blue);
    ecs_entity_t e3 = ecs_new(world);
    ecs_add_pair(world, e3, Color, Blue);

    ecs_progress(world, 1);

    {
        ecs_entity_t red = ecs_lookup(world, "metrics.color.Red");
        test_assert(red != 0);
        const EcsMetricValue *v = ecs_get(world, red, EcsMetricValue);
        test_assert(v != NULL);
        test_int(v->value, 1);
    }

    {
        ecs_entity_t green = ecs_lookup(world, "metrics.color.Green");
        test_assert(green != 0);
        const EcsMetricValue *v = ecs_get(world, green, EcsMetricValue);
        test_assert(v != NULL);
        test_int(v->value, 1);
    }

    {
        ecs_entity_t blue = ecs_lookup(world, "metrics.color.Blue");
        test_assert(blue != 0);
        const EcsMetricValue *v = ecs_get(world, blue, EcsMetricValue);
        test_assert(v != NULL);
        test_int(v->value, 2);
    }

    ecs_fini(world);
}

void Metrics_metric_instance_has_doc_name(void) {
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
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, 20});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);

        test_str(ecs_doc_get_name(world, e), "e1");
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_metric_nested_member(void) {
    typedef struct {
        float x, y;
    } Point;

    typedef struct {
        float dummy;
        Point position;
    } Position;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Point);
    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "dummy", ecs_id(ecs_f32_t) },
            { "position", ecs_id(Point) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_id(Position),
        .dotmember = "position.y",
        .kind = EcsGauge
    });

    test_assert(m != 0);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, {20, 30}});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 30);

        test_str(ecs_doc_get_name(world, e), "e1");
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_metric_nested_member_counter(void) {
    typedef struct {
        float x, y;
    } Point;

    typedef struct {
        float dummy;
        Point position;
    } Position;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Point);
    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "dummy", ecs_id(ecs_f32_t) },
            { "position", ecs_id(Point) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_id(Position),
        .dotmember = "position.y",
        .kind = EcsCounter
    });

    test_assert(m != 0);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, {20, 30}});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounter));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 30);

        test_str(ecs_doc_get_name(world, e), "e1");
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_metric_nested_member_counter_increment(void) {
    typedef struct {
        float x, y;
    } Point;

    typedef struct {
        float dummy;
        Point position;
    } Position;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Point);
    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "dummy", ecs_id(ecs_f32_t) },
            { "position", ecs_id(Point) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_id(Position),
        .dotmember = "position.y",
        .kind = EcsCounterIncrement
    });

    test_assert(m != 0);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, {20, 30}});

    ecs_progress(world, 1);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 30);

            test_str(ecs_doc_get_name(world, e), "e1");
        }

        test_bool(false, ecs_children_next(&it));
    }

    ecs_progress(world, 1);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 60);

            test_str(ecs_doc_get_name(world, e), "e1");
        }

        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void Metrics_id_w_member_same_type(void) {
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
        .id = ecs_id(Position),
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_id_w_member_mismatching_type(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_log_set_level(-4);
    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_id(Velocity),
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m == 0);

    ecs_fini(world);
}

void Metrics_pair_member_rel_type(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_pair_t(Position, Foo),
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value_pair(Position, Foo, {10, 20}));
    ecs_set_pair(world, e1, Position, Bar, {1, 2});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_pair_member_tgt_type(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_pair(Foo, ecs_id(Position)),
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, 
        ecs_value_pair_2nd(Foo, Position, {10, 20}),
        ecs_value_pair_2nd(Bar, Position, {1, 2}));

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_pair_dotmember_rel_type(void) {
    typedef struct {
        float x, y;
    } Point;

    typedef struct {
        float dummy;
        Point position;
    } Position;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Point);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "dummy", ecs_id(ecs_f32_t) },
            { "position", ecs_id(Point) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_pair_t(Position, Foo),
        .dotmember = "position.y",
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value_pair(Position, Foo, {0, {10, 20}}));
    ecs_set_pair(world, e1, Position, Bar, {0, {1, 2}});

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_pair_dotmember_tgt_type(void) {
    typedef struct {
        float x, y;
    } Point;

    typedef struct {
        float dummy;
        Point position;
    } Position;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Point);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "dummy", ecs_id(ecs_f32_t) },
            { "position", ecs_id(Point) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_pair(Foo, ecs_id(Position)),
        .dotmember = "position.y",
        .kind = EcsGauge
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, 
        ecs_value_pair_2nd(Foo, Position, {0, {10, 20}}),
        ecs_value_pair_2nd(Bar, Position, {0, {1, 2}}));

    ecs_progress(world, 0);
    
    ecs_iter_t it = ecs_children(world, m);
    test_bool(true, ecs_children_next(&it));
    test_uint(it.count, 1);
    {
        ecs_entity_t e = it.entities[0];
        test_assert(ecs_has_pair(world, e, EcsMetric, EcsGauge));
        const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
        const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
        test_assert(src != NULL);
        test_assert(inst != NULL);
        test_uint(src->entity, e1);
        test_int(inst->value, 20);
    }

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void Metrics_pair_member_counter_increment(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        }
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .id = ecs_pair_t(Position, Foo),
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsCounterIncrement
    });
    test_assert(m != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value_pair(Position, Foo, {10, 20}));
    ecs_set_pair(world, e1, Position, Bar, {1, 2});

    ecs_progress(world, 1);
    
    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 20);
        }

        test_bool(false, ecs_children_next(&it));
    }

    ecs_progress(world, 1);

    {
        ecs_iter_t it = ecs_children(world, m);
        test_bool(true, ecs_children_next(&it));
        test_uint(it.count, 1);
        {
            ecs_entity_t e = it.entities[0];
            test_assert(ecs_has_pair(world, e, EcsMetric, EcsCounterIncrement));
            const EcsMetricSource *src = ecs_get(world, e, EcsMetricSource);
            const EcsMetricValue *inst = ecs_get(world, e, EcsMetricValue);
            test_assert(src != NULL);
            test_assert(inst != NULL);
            test_uint(src->entity, e1);
            test_int(inst->value, 40);
        }

        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}
