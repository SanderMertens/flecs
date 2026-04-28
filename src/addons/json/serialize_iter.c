/**
 * @file addons/json/serialize_iter.c
 * @brief Serialize iterator to JSON.
 */

#include "json.h"

#ifdef FLECS_JSON

static
void flecs_json_serialize_field_info(
    const ecs_world_t *world,
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ctx)
{
    int32_t field_count = it->field_count;
    if (!field_count || !it->query) {
        return;
    }

    const ecs_query_t *q = it->query;

    flecs_json_memberl(buf, "field_info");
    flecs_json_array_push(buf);

    int f;
    for (f = 0; f < field_count; f ++) {
        flecs_json_next(buf);
        flecs_json_serialize_field(world, it, q, f, buf, ctx);
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_query_info(
    const ecs_world_t *world,
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf)
{
    if (!it->query) {
        return;
    }

    const ecs_query_t *q = it->query;
    flecs_json_memberl(buf, "query_info");
    flecs_json_serialize_query(world, q, buf);
}

static
void flecs_json_serialize_query_plan(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    (void)world;
    (void)buf;
    (void)desc;

    if (!desc->query) {
        return;
    }

    const ecs_query_t *q = desc->query;
    flecs_poly_assert(q, ecs_query_t);
    const ecs_query_t *cq = ecs_query_get_cache_query(q);
    
    flecs_json_memberl(buf, "query_plan");

    bool prev_color = ecs_log_enable_colors(true);
    char *plan = ecs_query_plan(q);
    char *cache_plan = NULL;
    if (cq) {
        flecs_poly_assert(cq, ecs_query_t);
        cache_plan = ecs_query_plan(cq);
    }

    ecs_strbuf_t plan_buf = ECS_STRBUF_INIT;
    if (plan) {
        ecs_strbuf_appendstr(&plan_buf, plan);
    } else {
        if (q->term_count) {
            ecs_strbuf_append(&plan_buf, "   %sOptimized out (trivial query)\n", ECS_GREY);
        }
    }

    if (cq) {
        ecs_strbuf_appendstr(&plan_buf, "\n\n");
        ecs_strbuf_appendstr(&plan_buf, "   Cache plan\n");
        ecs_strbuf_appendstr(&plan_buf, "   ---\n");
        
        if (cache_plan) {
            ecs_strbuf_appendstr(&plan_buf, cache_plan);
        } else {
            ecs_strbuf_append(&plan_buf, "   %sOptimized out (trivial query)\n", ECS_GREY);
        }
    }

    char *plan_str = ecs_strbuf_get(&plan_buf);
    if (plan_str) {
        flecs_json_string_escape(buf, plan_str);
        ecs_os_free(plan_str);
    } else {
        flecs_json_null(buf);
    }

    ecs_os_free(plan);
    ecs_os_free(cache_plan);
    ecs_log_enable_colors(prev_color);
}

static
void flecs_json_serialize_query_profile(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    const ecs_iter_t *it,
    const ecs_iter_to_json_desc_t *desc)
{
    if (!desc->query) {
        return;
    }
    
    ecs_time_t t = {0};
    int32_t result_count = 0, entity_count = 0, i, sample_count = 100;
    ecs_size_t component_bytes = 0, shared_component_bytes = 0;
    double eval_time = 0, eval_min = 0, eval_max = 0;
    ecs_time_measure(&t);

    for (i = 0; i < sample_count; i ++) {
        result_count = 0; 
        entity_count = 0;
        component_bytes = 0;
        shared_component_bytes = 0;

        ecs_iter_t qit = ecs_query_iter(world, desc->query);    
        while (ecs_query_next(&qit)) {
            result_count ++;
            entity_count += qit.count;

            int8_t f, field_count = qit.field_count;
            for (f = 0; f < field_count; f ++) {
                size_t size = ecs_field_size(&qit, f);
                if (ecs_field_is_set(&qit, f) && size) {
                    if (ecs_field_is_self(&qit, f)) {
                        component_bytes += 
                            flecs_uto(ecs_size_t, size) * qit.count;
                    } else {
                        shared_component_bytes += flecs_uto(ecs_size_t, size);
                    }
                }
            }
        }

        double time_measure = ecs_time_measure(&t);
        if (!i) {
            eval_min = time_measure;
        } else if (time_measure < eval_min) {
            eval_min = time_measure;
        }

        if (time_measure > eval_max) {
            eval_max = time_measure;
        }

        eval_time += time_measure;

        /* Don't profile for too long */
        if (eval_time > 0.001) {
            i ++;
            break;
        }
    }

    eval_time /= i;

    flecs_json_memberl(buf, "query_profile");
    flecs_json_object_push(buf);
    if (it->query) {
        /* Correct for profiler */
        ECS_CONST_CAST(ecs_query_t*, it->query)->eval_count -= i;
        flecs_json_memberl(buf, "eval_count");
        flecs_json_number(buf, it->query->eval_count);
    }
    flecs_json_memberl(buf, "result_count");
    flecs_json_number(buf, result_count);
    flecs_json_memberl(buf, "entity_count");
    flecs_json_number(buf, entity_count);

    flecs_json_memberl(buf, "eval_time_avg_us");
    flecs_json_number(buf, eval_time * 1000.0 * 1000.0);
    flecs_json_memberl(buf, "eval_time_min_us");
    flecs_json_number(buf, eval_min * 1000.0 * 1000.0);
    flecs_json_memberl(buf, "eval_time_max_us");
    flecs_json_number(buf, eval_max * 1000.0 * 1000.0);

    flecs_json_memberl(buf, "component_bytes");
    flecs_json_number(buf, component_bytes);
    flecs_json_memberl(buf, "shared_component_bytes");
    flecs_json_number(buf, shared_component_bytes);

    flecs_json_object_pop(buf);
}

static
void flecs_iter_free_ser_ctx(
    ecs_iter_t *it,
    ecs_json_ser_ctx_t *ser_ctx)
{
    int32_t f, field_count = it->field_count;
    for (f = 0; f < field_count; f ++) {
        ecs_os_free(ser_ctx->value_ctx[f].id_label);
    }
}

static
void flecs_json_accum_query_term_type_info(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_json_ser_ctx_t *ser_ctx)
{
    if (!ser_ctx->type_info_buf || !it->query) {
        return;
    }

    const ecs_query_t *q = it->query;
    int32_t i, term_count = q->term_count;
    for (i = 0; i < term_count; i ++) {
        const ecs_term_t *term = &q->terms[i];
        if (term->inout == EcsInOutNone) {
            continue;
        }
        ecs_entity_t typeid = ecs_get_typeid(world, term->id);
        if (typeid) {
            flecs_json_accum_type_info(world, typeid, ser_ctx);
        }
    }
}

int ecs_iter_to_json_buf(
    ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_world_t *world = it->real_world;

    /* Cache component record for flecs.doc ids */
    ecs_json_ser_ctx_t ser_ctx;
    ecs_os_zeromem(&ser_ctx);
#ifdef FLECS_DOC
    ser_ctx.cr_doc_name = flecs_components_get(world,
        ecs_pair_t(EcsDocDescription, EcsName));
    ser_ctx.cr_doc_color = flecs_components_get(world,
        ecs_pair_t(EcsDocDescription, EcsDocColor));
#endif

    ecs_strbuf_t type_info_buf = ECS_STRBUF_INIT;
    if (desc && desc->serialize_type_info) {
        flecs_json_type_info_accum_init(&ser_ctx, &type_info_buf, world);
        flecs_json_accum_query_term_type_info(world, it, &ser_ctx);
    }

    ecs_strbuf_t body_buf = ECS_STRBUF_INIT;
    flecs_json_object_push(&body_buf);

    /* Serialize field info if enabled */
    if (desc && desc->serialize_field_info) {
        flecs_json_serialize_field_info(world, it, &body_buf, &ser_ctx);
    }

    /* Serialize query info if enabled */
    if (desc && desc->serialize_query_info) {
        flecs_json_serialize_query_info(world, it, &body_buf);
    }

    /* Serialize query plan if enabled */
    if (desc && desc->serialize_query_plan) {
        flecs_json_serialize_query_plan(world, &body_buf, desc);
    }

    /* Profile query */
    if (desc && desc->serialize_query_profile) {
        flecs_json_serialize_query_profile(world, &body_buf, it, desc);
    }

    bool failed = false;

    /* Serialize results */
    if (!desc || !desc->dont_serialize_results) {
        flecs_json_memberl(&body_buf, "results");
        flecs_json_array_push(&body_buf);

        /* If serializing entire table, don't bother letting the iterator populate
         * data fields as we'll be iterating all columns. */
        if (desc && desc->serialize_table) {
            ECS_BIT_SET(it->flags, EcsIterNoData);
        }

        /* Keep track of serialized entities. This allows entities to be
         * serialized depth first, which avoids weird side effects from children
         * being created before parents. */
        if (desc && desc->serialize_parents_before_children) {
            ecs_map_init(&ser_ctx.serialized, &world->allocator);
        }

        ecs_iter_next_action_t next = it->next;
        while (next(it)) {
            if (flecs_json_serialize_iter_result(
                world, it, &body_buf, desc, &ser_ctx))
            {
                ecs_iter_fini(it);
                failed = true;
                break;
            }
        }

        if (desc && desc->serialize_parents_before_children) {
            ecs_map_fini(&ser_ctx.serialized);
        }

        flecs_json_array_pop(&body_buf);
    } else {
        ecs_iter_fini(it);
    }

    flecs_iter_free_ser_ctx(it, &ser_ctx);
    flecs_json_object_pop(&body_buf);

    flecs_json_type_info_accum_fini(&ser_ctx, &type_info_buf);

    if (failed) {
        ecs_strbuf_reset(&body_buf);
        ecs_strbuf_reset(&type_info_buf);
        ecs_strbuf_reset(buf);
        return -1;
    }

    flecs_json_assemble_output(buf, ser_ctx.type_info_buf, &body_buf);

    return 0;
}

char* ecs_iter_to_json(
    ecs_iter_t *it,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ecs_iter_to_json_buf(it, &buf, desc)) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

#endif
