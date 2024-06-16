/**
 * @file json/serialize_entity.c
 * @brief Serialize single entity.
 */

#include "json.h"
#include "../meta/meta.h"

int ecs_entity_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    const ecs_entity_to_json_desc_t *desc)
{
    if (!entity || !ecs_is_valid(world, entity)) {
        return -1;
    }

    /* Initialize iterator parameters */
    ecs_iter_to_json_desc_t iter_desc = {
        .serialize_table = true,
        .serialize_values = desc->serialize_values,
        .serialize_labels = desc->serialize_labels,
        .serialize_matches = desc->serialize_matches,
        .serialize_refs = desc->serialize_refs,
        .serialize_alerts = desc->serialize_alerts,
        .serialize_full_paths = desc->serialize_full_paths,
        .serialize_inherited = desc->serialize_inherited,
        .serialize_type_info = desc->serialize_type_info
    };

    /* Cache id record for flecs.doc ids */
    ecs_json_ser_ctx_t ser_ctx;
    ecs_os_zeromem(&ser_ctx);
#ifdef FLECS_DOC
    ser_ctx.idr_doc_name = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsName));
    ser_ctx.idr_doc_color = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsDocColor));
#endif

    /* Create iterator that's populated just with entity */
    ecs_record_t *r = ecs_record_find(world, entity);
    if (!r || !r->table) {
        flecs_json_object_push(buf);
        flecs_json_object_pop(buf);
        return 0;
    }

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
