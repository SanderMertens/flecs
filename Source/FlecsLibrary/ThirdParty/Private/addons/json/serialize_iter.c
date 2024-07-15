/**
 * @file addons/json/serialize_iter.c
 * @brief Serialize iterator to JSON.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON

static
void flecs_json_serialize_id_str(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_strbuf_t *buf)
{
    ecs_strbuf_appendch(buf, '"');
    if (ECS_IS_PAIR(id)) {
        ecs_entity_t first = ecs_pair_first(world, id);
        ecs_entity_t second = ecs_pair_first(world, id);
        ecs_strbuf_appendch(buf, '(');
        ecs_get_path_w_sep_buf(world, 0, first, ".", "", buf);
        ecs_strbuf_appendch(buf, ',');
        ecs_get_path_w_sep_buf(world, 0, second, ".", "", buf);
        ecs_strbuf_appendch(buf, ')');
    } else {
        ecs_get_path_w_sep_buf(
            world, 0, id & ECS_COMPONENT_MASK, ".", "", buf);
    }
    ecs_strbuf_appendch(buf, '"');
}

static
void flecs_json_serialize_type_info(
    const ecs_world_t *world,
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf) 
{
    flecs_json_memberl(buf, "type_info");
    flecs_json_object_push(buf);

    int32_t field_count = it->field_count;
    if (!field_count) {
        goto done;
    }

    if (it->flags & EcsIterNoData) {
        goto done;
    }

    for (int i = 0; i < field_count; i ++) {
        flecs_json_next(buf);
        ecs_entity_t typeid = 0;
        if (it->query->terms[i].inout != EcsInOutNone) {
            typeid = ecs_get_typeid(world, it->query->terms[i].id);
        }
        if (typeid) {
            flecs_json_serialize_id_str(world, typeid, buf);
            ecs_strbuf_appendch(buf, ':');
            ecs_type_info_to_json_buf(world, typeid, buf);
        } else {
            flecs_json_serialize_id_str(world, it->query->terms[i].id, buf);
            ecs_strbuf_appendlit(buf, ":0");
        }
    }

done:
    flecs_json_object_pop(buf);
}

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

    flecs_json_memberl(buf, "query_plan");

    bool prev_color = ecs_log_enable_colors(true);
    char *plan = ecs_query_plan(q);
    flecs_json_string_escape(buf, plan);
    ecs_os_free(plan);
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
        qit.flags |= EcsIterIsInstanced;
    
        while (ecs_iter_next(&qit)) {
            result_count ++;
            entity_count += qit.count;

            int32_t f, field_count = qit.field_count;
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

int ecs_iter_to_json_buf(
    ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_world_t *world = it->real_world;

    /* Cache id record for flecs.doc ids */
    ecs_json_ser_ctx_t ser_ctx;
    ecs_os_zeromem(&ser_ctx);
#ifdef FLECS_DOC
    ser_ctx.idr_doc_name = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsName));
    ser_ctx.idr_doc_color = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsDocColor));
#endif

    flecs_json_object_push(buf);

    /* Serialize type info if enabled */
    if (desc && desc->serialize_type_info) {
        flecs_json_serialize_type_info(world, it, buf);
    }

    /* Serialize field info if enabled */
    if (desc && desc->serialize_field_info) {
        flecs_json_serialize_field_info(world, it, buf, &ser_ctx);
    }

    /* Serialize query info if enabled */
    if (desc && desc->serialize_query_info) {
        flecs_json_serialize_query_info(world, it, buf);
    }

    /* Serialize query plan if enabled */
    if (desc && desc->serialize_query_plan) {
        flecs_json_serialize_query_plan(world, buf, desc);
    }

    /* Profile query */
    if (desc && desc->serialize_query_profile) {
        flecs_json_serialize_query_profile(world, buf, it, desc);
    }

    /* Serialize results */
    if (!desc || !desc->dont_serialize_results) {
        flecs_json_memberl(buf, "results");
        flecs_json_array_push(buf);

        /* Use instancing for improved performance */
        ECS_BIT_SET(it->flags, EcsIterIsInstanced);

        /* If serializing entire table, don't bother letting the iterator populate
         * data fields as we'll be iterating all columns. */
        if (desc && desc->serialize_table) {
            ECS_BIT_SET(it->flags, EcsIterNoData);
        }

        ecs_iter_next_action_t next = it->next;
        while (next(it)) {
            if (flecs_json_serialize_iter_result(world, it, buf, desc, &ser_ctx)) {
                ecs_strbuf_reset(buf);
                flecs_iter_free_ser_ctx(it, &ser_ctx);
                ecs_iter_fini(it);
                return -1;
            }
        }

        flecs_json_array_pop(buf);
    } else {
        ecs_iter_fini(it);
    }

    flecs_iter_free_ser_ctx(it, &ser_ctx);

    flecs_json_object_pop(buf);

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
