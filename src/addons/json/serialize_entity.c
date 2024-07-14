/**
 * @file addons/json/serialize_entity.c
 * @brief Serialize single entity.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON

int ecs_entity_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    const ecs_entity_to_json_desc_t *desc)
{
    if (!entity || !ecs_is_valid(world, entity)) {
        return -1;
    }

    /* Cache id record for flecs.doc ids */
    ecs_json_ser_ctx_t ser_ctx;
    ecs_os_zeromem(&ser_ctx);
#ifdef FLECS_DOC
    ser_ctx.idr_doc_name = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsName));
    ser_ctx.idr_doc_color = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsDocColor));
#endif

    ecs_record_t *r = ecs_record_find(world, entity);
    if (!r || !r->table) {
        flecs_json_object_push(buf);
        flecs_json_member(buf, "name");
        ecs_strbuf_appendch(buf, '"');
        ecs_strbuf_appendch(buf, '#');
        ecs_strbuf_appendint(buf, (uint32_t)entity);
        ecs_strbuf_appendch(buf, '"');
        flecs_json_object_pop(buf);
        return 0;
    }

    /* Create iterator that's populated just with entity */
    int32_t row = ECS_RECORD_TO_ROW(r->row);
    ecs_iter_t it = {
        .world = ECS_CONST_CAST(ecs_world_t*, world),
        .real_world = ECS_CONST_CAST(ecs_world_t*, world),
        .table = r->table,
        .offset = row,
        .count = 1,
        .entities = &flecs_table_entities_array(r->table)[row],
        .field_count = 0
    };

    /* Initialize iterator parameters */
    ecs_iter_to_json_desc_t iter_desc = {
        .serialize_table = true,
        .serialize_entity_ids =   desc ? desc->serialize_entity_id : false,
        .serialize_values =       desc ? desc->serialize_values : true,
        .serialize_doc =          desc ? desc->serialize_doc : false,
        .serialize_matches =      desc ? desc->serialize_matches : false,
        .serialize_refs =         desc ? desc->serialize_refs : 0,
        .serialize_alerts =       desc ? desc->serialize_alerts : false,
        .serialize_full_paths =   desc ? desc->serialize_full_paths : false,
        .serialize_inherited =    desc ? desc->serialize_inherited : false,
        .serialize_type_info =    desc ? desc->serialize_type_info : false
    };

    if (flecs_json_serialize_iter_result(
        world, &it, buf, &iter_desc, &ser_ctx))
    {
        return -1;
    }

    return 0;
}

char* ecs_entity_to_json(
    const ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_entity_to_json_desc_t *desc)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ecs_entity_to_json_buf(world, entity, &buf, desc) != 0) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

#endif
