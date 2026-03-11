/**
 * @file addons/json/serialize_world.c
 * @brief Serialize world to JSON.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON

static
void flecs_world_to_json_init_query(
    ecs_query_desc_t *query_desc,
    const ecs_world_to_json_desc_t *desc,
    bool singleton_query)
{
    bool serialize_builtin = desc && desc->serialize_builtin;
    bool serialize_modules = desc && desc->serialize_modules;
    int32_t term_id = 0;

    if (singleton_query) {
        query_desc->terms[term_id].id = EcsSingleton;
        term_id ++;
    } else if (serialize_builtin && serialize_modules) {
        query_desc->terms[term_id].id = EcsAny;
        term_id ++;
    }

    if (!serialize_builtin) {
        query_desc->terms[term_id].id = ecs_pair(EcsChildOf, EcsFlecs);
        query_desc->terms[term_id].oper = EcsNot;
        query_desc->terms[term_id].src.id = EcsSelf | EcsUp;
        term_id ++;
    }

    if (!singleton_query && !serialize_modules) {
        query_desc->terms[term_id].id = EcsModule;
        query_desc->terms[term_id].oper = EcsNot;
        query_desc->terms[term_id].src.id = EcsSelf | EcsUp;
    }

    query_desc->flags = EcsQueryMatchDisabled|EcsQueryMatchPrefab;
}

static
void flecs_world_to_json_fini_ser_ctx(
    ecs_json_ser_ctx_t *ser_ctx)
{
    int32_t i;
    for (i = 0; i < 64; i ++) {
        ecs_os_free(ser_ctx->value_ctx[i].id_label);
    }
}

static
void flecs_world_to_json_init_ser_ctx(
    ecs_world_t *world,
    ecs_json_ser_ctx_t *ser_ctx)
{
    ecs_os_zeromem(ser_ctx);
#ifdef FLECS_DOC
    ser_ctx->cr_doc_name = flecs_components_get(world,
        ecs_pair_t(EcsDocDescription, EcsName));
    ser_ctx->cr_doc_color = flecs_components_get(world,
        ecs_pair_t(EcsDocDescription, EcsDocColor));
#endif
}

static
int flecs_world_to_json_serialize_query(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx)
{
    ecs_iter_t it = ecs_query_iter(world, query);

    if (desc && desc->serialize_table) {
        ECS_BIT_SET(it.flags, EcsIterNoData);
    }

    ecs_iter_next_action_t next = it.next;
    while (next(&it)) {
        if (flecs_json_serialize_iter_result(world, &it, buf, desc, ser_ctx)) {
            ecs_iter_fini(&it);
            return -1;
        }
    }

    return 0;
}

int ecs_world_to_json_buf(
    ecs_world_t *world,
    ecs_strbuf_t *buf_out,
    const ecs_world_to_json_desc_t *desc)
{
    ecs_query_desc_t query_desc = {0};
    flecs_world_to_json_init_query(&query_desc, desc, false);

    ecs_query_t *q = ecs_query_init(world, &query_desc);
    if (!q) {
        return -1;
    }

    ecs_query_t *sq = NULL;
    if (desc && desc->serialize_singletons) {
        ecs_query_desc_t singleton_query_desc = {0};
        flecs_world_to_json_init_query(&singleton_query_desc, desc, true);

        sq = ecs_query_init(world, &singleton_query_desc);
        if (!sq) {
            ecs_query_fini(q);
            return -1;
        }
    }

    ecs_iter_to_json_desc_t json_desc = { 
        .serialize_table = true,
        .serialize_full_paths = true,
        .serialize_entity_ids = true,
        .serialize_values = true,
        .serialize_parents_before_children = true
    };

    ecs_json_ser_ctx_t ser_ctx;
    flecs_world_to_json_init_ser_ctx(world, &ser_ctx);

    flecs_json_object_push(buf_out);
    flecs_json_memberl(buf_out, "results");
    flecs_json_array_push(buf_out);

    ecs_map_init(&ser_ctx.serialized, &world->allocator);

    int ret = flecs_world_to_json_serialize_query(
        world, q, buf_out, &json_desc, &ser_ctx);
    if (!ret && sq) {
        ret = flecs_world_to_json_serialize_query(
            world, sq, buf_out, &json_desc, &ser_ctx);
    }

    ecs_map_fini(&ser_ctx.serialized);
    flecs_world_to_json_fini_ser_ctx(&ser_ctx);

    if (ret) {
        ecs_strbuf_reset(buf_out);
    } else {
        flecs_json_array_pop(buf_out);
        flecs_json_object_pop(buf_out);
    }

    if (sq) {
        ecs_query_fini(sq);
    }
    ecs_query_fini(q);
    return ret;
}

char* ecs_world_to_json(
    ecs_world_t *world,
    const ecs_world_to_json_desc_t *desc)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ecs_world_to_json_buf(world, &buf, desc)) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

#endif
