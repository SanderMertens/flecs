/**
 * @file addons/json/serialize_world.c
 * @brief Serialize world to JSON.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON

int ecs_world_to_json_buf(
    ecs_world_t *world,
    ecs_strbuf_t *buf_out,
    const ecs_world_to_json_desc_t *desc)
{
    ecs_query_desc_t query_desc = {0};

    if (desc && desc->serialize_builtin && desc->serialize_modules) {
        query_desc.terms[0].id = EcsAny;
    } else {
        bool serialize_builtin = desc && desc->serialize_builtin;
        bool serialize_modules = desc && desc->serialize_modules;
        int32_t term_id = 0;

        if (!serialize_builtin) {
            query_desc.terms[term_id].id = ecs_pair(EcsChildOf, EcsFlecs);
            query_desc.terms[term_id].oper = EcsNot;
            query_desc.terms[term_id].src.id = EcsSelf | EcsUp;
            term_id ++;
        }
        if (!serialize_modules) {
            query_desc.terms[term_id].id = EcsModule;
            query_desc.terms[term_id].oper = EcsNot;
            query_desc.terms[term_id].src.id = EcsSelf | EcsUp;
        }
    }

    query_desc.flags = EcsQueryMatchDisabled|EcsQueryMatchPrefab;

    ecs_query_t *q = ecs_query_init(world, &query_desc);
    if (!q) {
        return -1;
    }

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t json_desc = { 
        .serialize_table = true,
        .serialize_full_paths = true,
        .serialize_entity_ids = true,
        .serialize_values = true
    };

    int ret = ecs_iter_to_json_buf(&it, buf_out, &json_desc);
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

