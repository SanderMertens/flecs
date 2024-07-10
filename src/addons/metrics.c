/**
 * @file addons/metrics.c
 * @brief Metrics addon.
 */

#include "../private_api.h"

#ifdef FLECS_METRICS

/* Public components */
ECS_COMPONENT_DECLARE(FlecsMetrics);
ECS_TAG_DECLARE(EcsMetricInstance);
ECS_COMPONENT_DECLARE(EcsMetricValue);
ECS_COMPONENT_DECLARE(EcsMetricSource);
ECS_TAG_DECLARE(EcsMetric);
ECS_TAG_DECLARE(EcsCounter);
ECS_TAG_DECLARE(EcsCounterIncrement);
ECS_TAG_DECLARE(EcsCounterId);
ECS_TAG_DECLARE(EcsGauge);

/* Internal components */
static ECS_COMPONENT_DECLARE(EcsMetricMember);
static ECS_COMPONENT_DECLARE(EcsMetricId);
static ECS_COMPONENT_DECLARE(EcsMetricOneOf);
static ECS_COMPONENT_DECLARE(EcsMetricCountIds);
static ECS_COMPONENT_DECLARE(EcsMetricCountTargets);
static ECS_COMPONENT_DECLARE(EcsMetricMemberInstance);
static ECS_COMPONENT_DECLARE(EcsMetricIdInstance);
static ECS_COMPONENT_DECLARE(EcsMetricOneOfInstance);

/** Context for metric */
typedef struct {
    ecs_entity_t metric;              /**< Metric entity */
    ecs_entity_t kind;                /**< Metric kind (gauge, counter) */
} ecs_metric_ctx_t;

/** Context for metric that monitors member */
typedef struct {
    ecs_metric_ctx_t metric;
    ecs_primitive_kind_t type_kind;  /**< Primitive type kind of member */
    uint16_t offset;                 /**< Offset of member in component */
} ecs_member_metric_ctx_t;

/** Context for metric that monitors whether entity has id */
typedef struct {
    ecs_metric_ctx_t metric;
    ecs_id_record_t *idr;            /**< Id record for monitored component */
} ecs_id_metric_ctx_t;

/** Context for metric that monitors whether entity has pair target */
typedef struct {
    ecs_metric_ctx_t metric;
    ecs_id_record_t *idr;            /**< Id record for monitored component */
    ecs_size_t size;                 /**< Size of metric type */
    ecs_map_t target_offset;         /**< Pair target to metric type offset */
} ecs_oneof_metric_ctx_t;

/** Context for metric that monitors how many entities have a pair target */
typedef struct {
    ecs_metric_ctx_t metric;
    ecs_id_record_t *idr;            /**< Id record for monitored component */
    ecs_map_t targets;               /**< Map of counters for each target */
} ecs_count_targets_metric_ctx_t;

/** Stores context shared for all instances of member metric */
typedef struct {
    ecs_member_metric_ctx_t *ctx;
} EcsMetricMember;

/** Stores context shared for all instances of id metric */
typedef struct {
    ecs_id_metric_ctx_t *ctx;
} EcsMetricId;

/** Stores context shared for all instances of oneof metric */
typedef struct {
    ecs_oneof_metric_ctx_t *ctx;
} EcsMetricOneOf;

/** Stores context shared for all instances of id counter metric */
typedef struct {
    ecs_id_t id;
} EcsMetricCountIds;

/** Stores context shared for all instances of target counter metric */
typedef struct {
    ecs_count_targets_metric_ctx_t *ctx;
} EcsMetricCountTargets;

/** Instance of member metric */
typedef struct {
    ecs_ref_t ref;
    ecs_member_metric_ctx_t *ctx;
} EcsMetricMemberInstance;

/** Instance of id metric */
typedef struct {
    ecs_record_t *r;
    ecs_id_metric_ctx_t *ctx;
} EcsMetricIdInstance;

/** Instance of oneof metric */
typedef struct {
    ecs_record_t *r;
    ecs_oneof_metric_ctx_t *ctx;
} EcsMetricOneOfInstance;

/** Component lifecycle */

static ECS_DTOR(EcsMetricMember, ptr, {
    ecs_os_free(ptr->ctx);
})

static ECS_MOVE(EcsMetricMember, dst, src, {
    *dst = *src;
    src->ctx = NULL;
})

static ECS_DTOR(EcsMetricId, ptr, {
    ecs_os_free(ptr->ctx);
})

static ECS_MOVE(EcsMetricId, dst, src, {
    *dst = *src;
    src->ctx = NULL;
})

static ECS_DTOR(EcsMetricOneOf, ptr, {
    if (ptr->ctx) {
        ecs_map_fini(&ptr->ctx->target_offset);
        ecs_os_free(ptr->ctx);
    }
})

static ECS_MOVE(EcsMetricOneOf, dst, src, {
    *dst = *src;
    src->ctx = NULL;
})

static ECS_DTOR(EcsMetricCountTargets, ptr, {
    if (ptr->ctx) {
        ecs_map_fini(&ptr->ctx->targets);
        ecs_os_free(ptr->ctx);
    }
})

static ECS_MOVE(EcsMetricCountTargets, dst, src, {
    *dst = *src;
    src->ctx = NULL;
})

/** Observer used for creating new instances of member metric */
static void flecs_metrics_on_member_metric(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_member_metric_ctx_t *ctx = it->ctx;
    ecs_id_t id = ecs_field_id(it, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t m = ecs_new_w_pair(world, EcsChildOf, ctx->metric.metric);

        EcsMetricMemberInstance *src = ecs_emplace(
            world, m, EcsMetricMemberInstance, NULL);
        src->ref = ecs_ref_init_id(world, e, id);
        src->ctx = ctx;
        ecs_modified(world, m, EcsMetricMemberInstance);
        ecs_set(world, m, EcsMetricValue, { 0 });
        ecs_set(world, m, EcsMetricSource, { e });
        ecs_add(world, m, EcsMetricInstance);
        ecs_add_pair(world, m, EcsMetric, ctx->metric.kind);
    }
}

/** Observer used for creating new instances of id metric */
static void flecs_metrics_on_id_metric(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_id_metric_ctx_t *ctx = it->ctx;

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t m = ecs_new_w_pair(world, EcsChildOf, ctx->metric.metric);

        EcsMetricIdInstance *src = ecs_emplace(
            world, m, EcsMetricIdInstance, NULL);
        src->r = ecs_record_find(world, e);
        src->ctx = ctx;
        ecs_modified(world, m, EcsMetricIdInstance);
        ecs_set(world, m, EcsMetricValue, { 0 });
        ecs_set(world, m, EcsMetricSource, { e });
        ecs_add(world, m, EcsMetricInstance);
        ecs_add_pair(world, m, EcsMetric, ctx->metric.kind);
    }
}

/** Observer used for creating new instances of oneof metric */
static void flecs_metrics_on_oneof_metric(ecs_iter_t *it) {
    if (it->event == EcsOnRemove) {
        return;
    }

    ecs_world_t *world = it->world;
    ecs_oneof_metric_ctx_t *ctx = it->ctx;

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t m = ecs_new_w_pair(world, EcsChildOf, ctx->metric.metric);

        EcsMetricOneOfInstance *src = ecs_emplace(
            world, m, EcsMetricOneOfInstance, NULL);
        src->r = ecs_record_find(world, e);
        src->ctx = ctx;
        ecs_modified(world, m, EcsMetricOneOfInstance);
        ecs_add_pair(world, m, ctx->metric.metric, ecs_id(EcsMetricValue));
        ecs_set(world, m, EcsMetricSource, { e });
        ecs_add(world, m, EcsMetricInstance);
        ecs_add_pair(world, m, EcsMetric, ctx->metric.kind);
    }
}

/** Set doc name of metric instance to name of source entity */
#ifdef FLECS_DOC
static void SetMetricDocName(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMetricSource *src = ecs_field(it, EcsMetricSource, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t src_e = src[i].entity;
        const char *name = ecs_get_name(world, src_e);
        if (name) {
            ecs_doc_set_name(world, it->entities[i], name);
        }
    }
}
#endif

/** Delete metric instances for entities that are no longer alive */
static void ClearMetricInstance(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMetricSource *src = ecs_field(it, EcsMetricSource, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t src_e = src[i].entity;
        if (!ecs_is_alive(world, src_e)) {
            ecs_delete(world, it->entities[i]);
        }
    }
}

/** Update member metric */
static void UpdateMemberInstance(ecs_iter_t *it, bool counter) {
    ecs_world_t *world = it->real_world;
    EcsMetricValue *m = ecs_field(it, EcsMetricValue, 0);
    EcsMetricMemberInstance *mi = ecs_field(it, EcsMetricMemberInstance, 1);
    ecs_ftime_t dt = it->delta_time;

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_member_metric_ctx_t *ctx = mi[i].ctx;
        ecs_ref_t *ref = &mi[i].ref;
        if (!ref->entity) {
            continue;
        }

        const void *ptr = ecs_ref_get_id(world, ref, ref->id);
        if (ptr) {
            ptr = ECS_OFFSET(ptr, ctx->offset);
            if (!counter) {
                m[i].value = ecs_meta_ptr_to_float(ctx->type_kind, ptr);
            } else {
                m[i].value += 
                    ecs_meta_ptr_to_float(ctx->type_kind, ptr) * (double)dt;
            }
        } else {
            ecs_delete(it->world, it->entities[i]);
        }
    }
}

static void UpdateGaugeMemberInstance(ecs_iter_t *it) {
    UpdateMemberInstance(it, false);
}

static void UpdateCounterMemberInstance(ecs_iter_t *it) {
    UpdateMemberInstance(it, false);
}

static void UpdateCounterIncrementMemberInstance(ecs_iter_t *it) {
    UpdateMemberInstance(it, true);
}

/** Update id metric */
static void UpdateIdInstance(ecs_iter_t *it, bool counter) {
    ecs_world_t *world = it->real_world;
    EcsMetricValue *m = ecs_field(it, EcsMetricValue, 0);
    EcsMetricIdInstance *mi = ecs_field(it, EcsMetricIdInstance, 1);
    ecs_ftime_t dt = it->delta_time;

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_record_t *r = mi[i].r;
        if (!r) {
            continue;
        }

        ecs_table_t *table = r->table;
        if (!table) {
            ecs_delete(it->world, it->entities[i]);
            continue;
        }

        ecs_id_metric_ctx_t *ctx = mi[i].ctx;
        ecs_id_record_t *idr = ctx->idr;
        if (flecs_search_w_idr(world, table, NULL, idr) != -1) {
            if (!counter) {
                m[i].value = 1.0;
            } else {
                m[i].value += 1.0 * (double)dt;
            }
        } else {
            ecs_delete(it->world, it->entities[i]);
        }
    }
}

static void UpdateGaugeIdInstance(ecs_iter_t *it) {
    UpdateIdInstance(it, false);
}

static void UpdateCounterIdInstance(ecs_iter_t *it) {
    UpdateIdInstance(it, true);
}

/** Update oneof metric */
static void UpdateOneOfInstance(ecs_iter_t *it, bool counter) {
    ecs_world_t *world = it->real_world;
    ecs_table_t *table = it->table;
    void *m = ecs_table_get_column(table, 
        ecs_table_type_to_column_index(table, it->columns[0]), it->offset);
    EcsMetricOneOfInstance *mi = ecs_field(it, EcsMetricOneOfInstance, 1);
    ecs_ftime_t dt = it->delta_time;

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_oneof_metric_ctx_t *ctx = mi[i].ctx;
        ecs_record_t *r = mi[i].r;
        if (!r) {
            continue;
        }

        ecs_table_t *mtable = r->table;

        double *value = ECS_ELEM(m, ctx->size, i);
        if (!counter) {
            ecs_os_memset(value, 0, ctx->size);
        }

        if (!mtable) {
            ecs_delete(it->world, it->entities[i]);
            continue;
        }

        ecs_id_record_t *idr = ctx->idr;
        ecs_id_t id;
        if (flecs_search_w_idr(world, mtable, &id, idr) == -1) {
            ecs_delete(it->world, it->entities[i]);
            continue;
        }

        ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        uint64_t *offset = ecs_map_get(&ctx->target_offset, tgt);
        if (!offset) {
            ecs_err("unexpected relationship target for metric");
            continue;
        }

        value = ECS_OFFSET(value, *offset);

        if (!counter) {
            *value = 1.0;
        } else {
            *value += 1.0 * (double)dt;
        }
    }
}

static void UpdateGaugeOneOfInstance(ecs_iter_t *it) {
    UpdateOneOfInstance(it, false);
}

static void UpdateCounterOneOfInstance(ecs_iter_t *it) {
    UpdateOneOfInstance(it, true);
}

static void UpdateCountTargets(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    EcsMetricCountTargets *m = ecs_field(it, EcsMetricCountTargets, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_count_targets_metric_ctx_t *ctx = m[i].ctx;
        ecs_id_record_t *cur = ctx->idr;
        while ((cur = cur->first.next)) {
            ecs_id_t id = cur->id;
            ecs_entity_t *mi = ecs_map_ensure(&ctx->targets, id);
            if (!mi[0]) {
                mi[0] = ecs_new_w_pair(world, EcsChildOf, ctx->metric.metric);
                ecs_entity_t tgt = ecs_pair_second(world, cur->id);
                const char *name = ecs_get_name(world, tgt);
                if (name) {
                    ecs_set_name(world, mi[0], name);
                }

                EcsMetricSource *source = ecs_ensure(
                    world, mi[0], EcsMetricSource);
                source->entity = tgt;
            }

            EcsMetricValue *value = ecs_ensure(world, mi[0], EcsMetricValue);
            value->value += (double)ecs_count_id(world, cur->id) * 
                (double)it->delta_system_time;
        }
    }
}

static void UpdateCountIds(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    EcsMetricCountIds *m = ecs_field(it, EcsMetricCountIds, 0);
    EcsMetricValue *v = ecs_field(it, EcsMetricValue, 1);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        v[i].value += (double)ecs_count_id(world, m[i].id) * 
            (double)it->delta_system_time;
    }
}

/** Initialize member metric */
static
int flecs_member_metric_init(
    ecs_world_t *world,
    ecs_entity_t metric,
    const ecs_metric_desc_t *desc)
{
    ecs_entity_t type = 0, member_type = 0, member = 0, id = 0;
    uintptr_t offset = 0;

    if (desc->dotmember) {
        if (!desc->id) {
            char *metric_name = ecs_get_path(world, metric);
            ecs_err("missing id for metric '%s' with member '%s",
                metric_name, desc->dotmember);
            ecs_os_free(metric_name);
            goto error;
        }

        if (desc->member) {
            char *metric_name = ecs_get_path(world, metric);
            ecs_err("cannot set both member and dotmember for metric '%s'",
                metric_name);
            ecs_os_free(metric_name);
            goto error;
        }

        type = ecs_get_typeid(world, desc->id);

        ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, NULL);
        if (ecs_meta_push(&cur)) {
            char *metric_name = ecs_get_path(world, metric);
            ecs_err("invalid type for metric '%s'", metric_name);
            ecs_os_free(metric_name);
            goto error;
        }
        if (ecs_meta_dotmember(&cur, desc->dotmember)) {
            char *metric_name = ecs_get_path(world, metric);
            ecs_err("invalid dotmember '%s' for metric '%s'",
                desc->dotmember, metric_name);
            ecs_os_free(metric_name);
            goto error;
        }

        id = desc->id;
        member_type = ecs_meta_get_type(&cur);
        offset = (uintptr_t)ecs_meta_get_ptr(&cur);
        member = ecs_meta_get_member_id(&cur);
    } else {    
        const EcsMember *m = ecs_get(world, desc->member, EcsMember);
        if (!m) {
            char *metric_name = ecs_get_path(world, metric);
            char *member_name = ecs_get_path(world, desc->member);
            ecs_err("entity '%s' provided for metric '%s' is not a member",
                member_name, metric_name);
            ecs_os_free(member_name);
            ecs_os_free(metric_name);
            goto error;
        }

        type = ecs_get_parent(world, desc->member);
        if (!type) {
            char *metric_name = ecs_get_path(world, metric);
            char *member_name = ecs_get_path(world, desc->member);
            ecs_err("member '%s' provided for metric '%s' is not part of a type",
                member_name, metric_name);
            ecs_os_free(member_name);
            ecs_os_free(metric_name);
            goto error;
        }
        
        id = type;
        if (desc->id) {
            if (type != ecs_get_typeid(world, desc->id)) {
                char *metric_name = ecs_get_path(world, metric);
                char *member_name = ecs_get_path(world, desc->member);
                char *id_name = ecs_get_path(world, desc->id);
                ecs_err("member '%s' for metric '%s' is not of type '%s'",
                    member_name, metric_name, id_name);
                ecs_os_free(id_name);
                ecs_os_free(member_name);
                ecs_os_free(metric_name);
                goto error;
            }
            id = desc->id;
        }

        member = desc->member;
        member_type = m->type;
        offset = flecs_ito(uintptr_t, m->offset);
    }

    const EcsPrimitive *p = ecs_get(world, member_type, EcsPrimitive);
    if (!p) {
        char *metric_name = ecs_get_path(world, metric);
        char *member_name = ecs_get_path(world, desc->member);
        ecs_err("member '%s' provided for metric '%s' must have primitive type",
            member_name, metric_name);
        ecs_os_free(member_name);
        ecs_os_free(metric_name);
        goto error;
    }

    const EcsType *mt = ecs_get(world, type, EcsType);
    if (!mt) {
        char *metric_name = ecs_get_path(world, metric);
        char *member_name = ecs_get_path(world, desc->member);
        ecs_err("parent of member '%s' for metric '%s' is not a type",
            member_name, metric_name);
        ecs_os_free(member_name);
        ecs_os_free(metric_name);
        goto error;
    }

    if (mt->kind != EcsStructType) {
        char *metric_name = ecs_get_path(world, metric);
        char *member_name = ecs_get_path(world, desc->member);
        ecs_err("parent of member '%s' for metric '%s' is not a struct",
            member_name, metric_name);
        ecs_os_free(member_name);
        ecs_os_free(metric_name);
        goto error;
    }

    ecs_member_metric_ctx_t *ctx = ecs_os_calloc_t(ecs_member_metric_ctx_t);
    ctx->metric.metric = metric;
    ctx->metric.kind = desc->kind;
    ctx->type_kind = p->kind;
    ctx->offset = flecs_uto(uint16_t, offset);

    ecs_observer(world, {
        .entity = metric,
        .events = { EcsOnAdd },
        .query.terms[0] = {
            .id = id,
            .src.id = EcsSelf,
            .inout = EcsInOutNone
        },
        .callback = flecs_metrics_on_member_metric,
        .yield_existing = true,
        .ctx = ctx
    });

    ecs_set_pair(world, metric, EcsMetricMember, member, { .ctx = ctx });
    ecs_add_pair(world, metric, EcsMetric, desc->kind);
    ecs_add_id(world, metric, EcsMetric);

    return 0;
error:
    return -1;
}

/** Update id metric */
static
int flecs_id_metric_init(
    ecs_world_t *world,
    ecs_entity_t metric,
    const ecs_metric_desc_t *desc)
{
    ecs_id_metric_ctx_t *ctx = ecs_os_calloc_t(ecs_id_metric_ctx_t);
    ctx->metric.metric = metric;
    ctx->metric.kind = desc->kind;
    ctx->idr = flecs_id_record_ensure(world, desc->id);
    ecs_check(ctx->idr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_observer(world, {
        .entity = metric,
        .events = { EcsOnAdd },
        .query.terms[0] = {
            .id = desc->id,
            .src.id = EcsSelf,
            .inout = EcsInOutNone
        },
        .callback = flecs_metrics_on_id_metric,
        .yield_existing = true,
        .ctx = ctx
    });

    ecs_set(world, metric, EcsMetricId, { .ctx = ctx });
    ecs_add_pair(world, metric, EcsMetric, desc->kind);
    ecs_add_id(world, metric, EcsMetric);

    return 0;
error:
    return -1;
}

/** Update oneof metric */
static
int flecs_oneof_metric_init(
    ecs_world_t *world,
    ecs_entity_t metric,
    ecs_entity_t scope,
    const ecs_metric_desc_t *desc)
{
    ecs_oneof_metric_ctx_t *ctx = ecs_os_calloc_t(ecs_oneof_metric_ctx_t);
    ctx->metric.metric = metric;
    ctx->metric.kind = desc->kind;
    ctx->idr = flecs_id_record_ensure(world, desc->id);
    ecs_check(ctx->idr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_map_init(&ctx->target_offset, NULL);

    /* Add member for each child of oneof to metric, so it can be used as metric
     * instance type that holds values for all targets */
    ecs_iter_t it = ecs_children(world, scope);
    uint64_t offset = 0;
    while (ecs_children_next(&it)) {
        int32_t i, count = it.count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t tgt = it.entities[i];
            const char *name = ecs_get_name(world, tgt);
            if (!name) {
                /* Member must have name */
                continue;
            }

            char *to_snake_case = flecs_to_snake_case(name);

            ecs_entity_t mbr = ecs_entity(world, {
                .name = to_snake_case,
                .parent = ecs_childof(metric)
            });

            ecs_os_free(to_snake_case);

            ecs_set(world, mbr, EcsMember, {
                .type = ecs_id(ecs_f64_t),
                .unit = EcsSeconds
            });

            /* Truncate upper 32 bits of target so we can lookup the offset
             * with the id we get from the pair */
            ecs_map_ensure(&ctx->target_offset, (uint32_t)tgt)[0] = offset;

            offset += sizeof(double);
        }
    }

    ctx->size = flecs_uto(ecs_size_t, offset);

    ecs_observer(world, {
        .entity = metric,
        .events = { EcsMonitor },
        .query.terms[0] = {
            .id = desc->id,
            .src.id = EcsSelf,
            .inout = EcsInOutNone
        },
        .callback = flecs_metrics_on_oneof_metric,
        .yield_existing = true,
        .ctx = ctx
    });

    ecs_set(world, metric, EcsMetricOneOf, { .ctx = ctx });
    ecs_add_pair(world, metric, EcsMetric, desc->kind);
    ecs_add_id(world, metric, EcsMetric);

    return 0;
error:
    return -1;
}

static
int flecs_count_id_targets_metric_init(
    ecs_world_t *world,
    ecs_entity_t metric,
    const ecs_metric_desc_t *desc)
{
    ecs_count_targets_metric_ctx_t *ctx = ecs_os_calloc_t(ecs_count_targets_metric_ctx_t);
    ctx->metric.metric = metric;
    ctx->metric.kind = desc->kind;
    ctx->idr = flecs_id_record_ensure(world, desc->id);
    ecs_check(ctx->idr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_map_init(&ctx->targets, NULL);

    ecs_set(world, metric, EcsMetricCountTargets, { .ctx = ctx });
    ecs_add_pair(world, metric, EcsMetric, desc->kind);
    ecs_add_id(world, metric, EcsMetric); 

    return 0;
error:
    return -1;
}

static
int flecs_count_ids_metric_init(
    ecs_world_t *world,
    ecs_entity_t metric,
    const ecs_metric_desc_t *desc)
{
    ecs_set(world, metric, EcsMetricCountIds, { .id = desc->id });
    ecs_set(world, metric, EcsMetricValue, { .value = 0 });
    return 0;
}

ecs_entity_t ecs_metric_init(
    ecs_world_t *world,
    const ecs_metric_desc_t *desc)
{
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_metric_desc_t was not initialized to zero");
    flecs_poly_assert(world, ecs_world_t);

    ecs_entity_t result = desc->entity;
    if (!result) {
        result = ecs_new(world);
    }

    ecs_entity_t kind = desc->kind;
    if (!kind) {
        ecs_err("missing metric kind");
        goto error;
    }

    if (kind != EcsGauge && 
        kind != EcsCounter && 
        kind != EcsCounterId &&
        kind != EcsCounterIncrement) 
    {
        ecs_err("invalid metric kind %s", ecs_get_path(world, kind));
        goto error;
    }

    if (kind == EcsCounterIncrement && !desc->member && !desc->dotmember) {
        ecs_err("CounterIncrement can only be used in combination with member");
        goto error;
    }

    if (kind == EcsCounterId && (desc->member || desc->dotmember)) {
        ecs_err("CounterId cannot be used in combination with member");
        goto error;
    }

    if (desc->brief) {
#ifdef FLECS_DOC
        ecs_doc_set_brief(world, result, desc->brief);
#else
        ecs_warn("FLECS_DOC is not enabled, ignoring metrics brief");
#endif        
    }

    if (desc->member || desc->dotmember) {
        if (flecs_member_metric_init(world, result, desc)) {
            goto error;
        }
    } else if (desc->id) {
        if (desc->targets) {
            if (!ecs_id_is_pair(desc->id)) {
                ecs_err("cannot specify targets for id that is not a pair");
                goto error;
            }
            if (ECS_PAIR_FIRST(desc->id) == EcsWildcard) {
                ecs_err("first element of pair cannot be wildcard with "
                    " targets enabled");
                goto error;
            }
            if (ECS_PAIR_SECOND(desc->id) != EcsWildcard) {
                ecs_err("second element of pair must be wildcard with "
                    " targets enabled");
                goto error;
            }

            if (kind == EcsCounterId) {
                if (flecs_count_id_targets_metric_init(world, result, desc)) {
                    goto error;
                }
            } else {
                ecs_entity_t first = ecs_pair_first(world, desc->id);
                ecs_entity_t scope = flecs_get_oneof(world, first);
                if (!scope) {
                    ecs_err("first element of pair must have OneOf with "
                        " targets enabled");
                    goto error;
                }

                if (flecs_oneof_metric_init(world, result, scope, desc)) {
                    goto error;
                }
            }
        } else {
            if (kind == EcsCounterId) {
                if (flecs_count_ids_metric_init(world, result, desc)) {
                    goto error;
                }
            } else {
                if (flecs_id_metric_init(world, result, desc)) {
                    goto error;
                }
            }
        }
    } else {
        ecs_err("missing source specified for metric");
        goto error;
    }

    return result;
error:
    if (result && result != desc->entity) {
        ecs_delete(world, result);
    }
    return 0;
}

void FlecsMetricsImport(ecs_world_t *world) {
    ECS_MODULE_DEFINE(world, FlecsMetrics);

    ECS_IMPORT(world, FlecsPipeline);
    ECS_IMPORT(world, FlecsMeta);
    ECS_IMPORT(world, FlecsUnits);

    ecs_set_name_prefix(world, "Ecs");
    ECS_TAG_DEFINE(world, EcsMetric);
    ecs_entity_t old_scope = ecs_set_scope(world, EcsMetric);
    ECS_TAG_DEFINE(world, EcsCounter);
    ECS_TAG_DEFINE(world, EcsCounterIncrement);
    ECS_TAG_DEFINE(world, EcsCounterId);
    ECS_TAG_DEFINE(world, EcsGauge);
    ecs_set_scope(world, old_scope);

    ecs_set_name_prefix(world, "EcsMetric");
    ECS_TAG_DEFINE(world, EcsMetricInstance);
    ECS_COMPONENT_DEFINE(world, EcsMetricValue);
    ECS_COMPONENT_DEFINE(world, EcsMetricSource);
    ECS_COMPONENT_DEFINE(world, EcsMetricMemberInstance);
    ECS_COMPONENT_DEFINE(world, EcsMetricIdInstance);
    ECS_COMPONENT_DEFINE(world, EcsMetricOneOfInstance);
    ECS_COMPONENT_DEFINE(world, EcsMetricMember);
    ECS_COMPONENT_DEFINE(world, EcsMetricId);
    ECS_COMPONENT_DEFINE(world, EcsMetricOneOf);
    ECS_COMPONENT_DEFINE(world, EcsMetricCountIds);
    ECS_COMPONENT_DEFINE(world, EcsMetricCountTargets);

    ecs_add_id(world, ecs_id(EcsMetricMemberInstance), EcsPrivate);
    ecs_add_id(world, ecs_id(EcsMetricIdInstance), EcsPrivate);
    ecs_add_id(world, ecs_id(EcsMetricOneOfInstance), EcsPrivate);

    ecs_struct(world, {
        .entity = ecs_id(EcsMetricValue),
        .members = {
            { .name = "value", .type = ecs_id(ecs_f64_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsMetricSource),
        .members = {
            { .name = "entity", .type = ecs_id(ecs_entity_t) }
        }
    });

    ecs_set_hooks(world, EcsMetricMember, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsMetricMember),
        .move = ecs_move(EcsMetricMember)
    });

    ecs_set_hooks(world, EcsMetricId, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsMetricId),
        .move = ecs_move(EcsMetricId)
    });

    ecs_set_hooks(world, EcsMetricOneOf, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsMetricOneOf),
        .move = ecs_move(EcsMetricOneOf)
    });

    ecs_set_hooks(world, EcsMetricCountTargets, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsMetricCountTargets),
        .move = ecs_move(EcsMetricCountTargets)
    });

    ecs_add_id(world, EcsMetric, EcsOneOf);

#ifdef FLECS_DOC
    ECS_OBSERVER(world, SetMetricDocName, EcsOnSet, 
        Source);
#endif

    ECS_SYSTEM(world, ClearMetricInstance, EcsPreStore,
        [in] Source);

    ECS_SYSTEM(world, UpdateGaugeMemberInstance, EcsPreStore, 
        [out]  Value, 
        [in]   MemberInstance,
        [none] (Metric, Gauge));

    ECS_SYSTEM(world, UpdateCounterMemberInstance, EcsPreStore, 
        [out]  Value, 
        [in]   MemberInstance,
        [none] (Metric, Counter));

    ECS_SYSTEM(world, UpdateCounterIncrementMemberInstance, EcsPreStore, 
        [out]  Value, 
        [in]   MemberInstance,
        [none] (Metric, CounterIncrement));

    ECS_SYSTEM(world, UpdateGaugeIdInstance, EcsPreStore, 
        [out]  Value, 
        [in]   IdInstance,
        [none] (Metric, Gauge));

    ECS_SYSTEM(world, UpdateCounterIdInstance, EcsPreStore, 
        [inout] Value, 
        [in]    IdInstance,
        [none]  (Metric, Counter));

    ECS_SYSTEM(world, UpdateGaugeOneOfInstance, EcsPreStore, 
        [none] (_, Value), 
        [in]   OneOfInstance,
        [none] (Metric, Gauge));

    ECS_SYSTEM(world, UpdateCounterOneOfInstance, EcsPreStore, 
        [none] (_, Value), 
        [in]   OneOfInstance,
        [none] (Metric, Counter));

    ECS_SYSTEM(world, UpdateCountIds, EcsPreStore, 
        [inout] CountIds, Value);

    ECS_SYSTEM(world, UpdateCountTargets, EcsPreStore, 
        [inout] CountTargets);
}

#endif
