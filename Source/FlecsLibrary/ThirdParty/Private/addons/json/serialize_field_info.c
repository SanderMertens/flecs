/**
 * @file addons/json/serialize_field_info.c
 * @brief Serialize query field information to JSON.
 */

#include "json.h"

#ifdef FLECS_JSON

static
bool flecs_json_serialize_get_field_ctx(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    int32_t f,
    ecs_json_ser_ctx_t *ser_ctx,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
    if (it->query) {
        return flecs_json_serialize_get_value_ctx(
            world, it->query->ids[f], value_ctx, desc);
    } else if (it->ids[f]) {
        return flecs_json_serialize_get_value_ctx(
            world, it->ids[f], value_ctx, desc);
    } else {
        return false;
    }
}


void flecs_json_serialize_field(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    const ecs_query_t *q,
    int field,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ctx)
{
    flecs_json_object_push(buf);
    flecs_json_memberl(buf, "id");

    flecs_json_serialize_get_field_ctx(world, it, field, ctx, NULL);
    ecs_json_value_ser_ctx_t *value_ctx = &ctx->value_ctx[field];

    if (value_ctx->id_label) {
        flecs_json_string(buf, value_ctx->id_label);

        const ecs_term_t *term = &q->terms[0];
        int t;
        for (t = 0; t < q->term_count; t ++) {
            if (q->terms[t].field_index == field) {
                term = &q->terms[t];
                break;
            }
        }

        if (term->oper != EcsNot) {
            if (term->oper == EcsOptional) {
                flecs_json_memberl(buf, "optional");
                flecs_json_bool(buf, true);
            }

            if (ECS_IS_PAIR(term->id)) {
                if ((term->first.id & EcsIsEntity) && ECS_TERM_REF_ID(&term->first)) {
                    if (ecs_has_id(world, ECS_TERM_REF_ID(&term->first), EcsExclusive)) {
                        flecs_json_memberl(buf, "exclusive");
                        flecs_json_bool(buf, true);
                    }
                }
            }

            if (value_ctx->type) {
                flecs_json_memberl(buf, "type");
                flecs_json_label(buf, world, value_ctx->type);

                const char *symbol = ecs_get_symbol(world, value_ctx->type);
                if (symbol) {
                    flecs_json_memberl(buf, "symbol");
                    flecs_json_string(buf, symbol);
                }
            }

            if (value_ctx->ser) {
                flecs_json_memberl(buf, "schema");
                ecs_type_info_to_json_buf(world, value_ctx->type, buf);
            }
        } else {
            flecs_json_memberl(buf, "not");
            flecs_json_bool(buf, true);
        }
    } else {
        ecs_strbuf_appendlit(buf, "0");
    }

    flecs_json_object_pop(buf);
}

#endif
