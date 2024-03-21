/**
 * @file json/serialize_type_info.c
 * @brief Serialize type (reflection) information to JSON.
 */

#include "json.h"

#ifdef FLECS_JSON

void flecs_json_serialize_field(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    const ecs_filter_t *q,
    int field,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ctx)
{
    flecs_json_object_push(buf);
    flecs_json_memberl(buf, "id");

    flecs_json_serialize_get_field_ctx(world, it, field, ctx);
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
                if (term->first.flags & EcsIsEntity && term->first.id) {
                    if (ecs_has_id(world, term->first.id, EcsExclusive)) {
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
