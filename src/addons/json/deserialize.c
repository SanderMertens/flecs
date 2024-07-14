/**
 * @file addons/json/deserialize.c
 * @brief Deserialize JSON strings into (component) values.
 */

#include "../../private_api.h"
#include "json.h"
#include "../script/script.h"
#include <ctype.h>

#ifdef FLECS_JSON

typedef struct {
    ecs_allocator_t *a;
    ecs_vec_t table_type;
    ecs_vec_t remove_ids;
    ecs_map_t anonymous_ids;
    ecs_map_t missing_reflection;
    const char *expr;
} ecs_from_json_ctx_t;

static
void flecs_from_json_ctx_init(
    ecs_allocator_t *a,
    ecs_from_json_ctx_t *ctx)
{
    ctx->a = a;
    ecs_vec_init_t(a, &ctx->table_type, ecs_id_t, 0);
    ecs_vec_init_t(a, &ctx->remove_ids, ecs_id_t, 0);
    ecs_map_init(&ctx->anonymous_ids, a);
    ecs_map_init(&ctx->missing_reflection, a);
}

static
void flecs_from_json_ctx_fini(
    ecs_from_json_ctx_t *ctx)
{
    ecs_vec_fini_t(ctx->a, &ctx->table_type, ecs_record_t*);
    ecs_vec_fini_t(ctx->a, &ctx->remove_ids, ecs_record_t*);
    ecs_map_fini(&ctx->anonymous_ids);
    ecs_map_fini(&ctx->missing_reflection);
}

static
ecs_entity_t flecs_json_new_id(
    ecs_world_t *world,
    ecs_entity_t ser_id)
{
    /* Try to honor low id requirements */
    if (ser_id < FLECS_HI_COMPONENT_ID) {
        return ecs_new_low_id(world);
    } else {
        return ecs_new(world);
    }
}

static
void flecs_json_missing_reflection(
    ecs_world_t *world,
    ecs_id_t id,
    const char *json,
    ecs_from_json_ctx_t *ctx,
    const ecs_from_json_desc_t *desc)
{
    if (!desc->strict || ecs_map_get(&ctx->missing_reflection, id)) {
        return;
    }

    /* Don't spam log when multiple values of a type can't be deserialized */
    ecs_map_ensure(&ctx->missing_reflection, id);

    char *id_str = ecs_id_str(world, id);
    ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
        "missing reflection for '%s'", id_str);
    ecs_os_free(id_str);
}

static
ecs_entity_t flecs_json_lookup(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name,
    const ecs_from_json_desc_t *desc)
{
    ecs_entity_t scope = 0;
    if (parent) {
        scope = ecs_set_scope(world, parent);
    }

    ecs_entity_t result = desc->lookup_action(world, name, desc->lookup_ctx);
    if (parent) {
        ecs_set_scope(world, scope);
    }
    return result;
}

static
void flecs_json_mark_reserved(
    ecs_map_t *anonymous_ids,
    ecs_entity_t e)
{
    ecs_entity_t *reserved = ecs_map_ensure(anonymous_ids, e);
    ecs_assert(reserved[0] == 0, ECS_INTERNAL_ERROR, NULL);
    reserved[0] = 0;
}

static
ecs_entity_t flecs_json_ensure_entity(
    ecs_world_t *world,
    const char *name,
    ecs_map_t *anonymous_ids)
{
    ecs_entity_t e = 0;

    if (flecs_name_is_id(name)) {
        /* Anonymous entity, find or create mapping to new id */
        ecs_entity_t ser_id = flecs_ito(ecs_entity_t, atoll(&name[1]));
        ecs_entity_t *deser_id = ecs_map_get(anonymous_ids, ser_id);
        if (deser_id) {
            if (!deser_id[0]) {
                /* Id is already issued by deserializer, create new id */
                deser_id[0] = flecs_json_new_id(world, ser_id);

                /* Mark new id as reserved */
                flecs_json_mark_reserved(anonymous_ids, deser_id[0]);
            } else {
                /* Id mapping exists */
            }
        } else {
            /* Id has not yet been issued by deserializer, which means it's safe
             * to use. This allows the deserializer to bind to existing 
             * anonymous ids, as they will never be reissued. */
            deser_id = ecs_map_ensure(anonymous_ids, ser_id);
            if (!ecs_exists(world, ser_id) || 
               (ecs_is_alive(world, ser_id) && !ecs_get_name(world, ser_id))) 
            {
                /* Only use existing id if it's alive or doesn't exist yet. The 
                 * id could have been recycled for another entity 
                 * Also don't use existing id if the existing entity is not
                 * anonymous. */
                deser_id[0] = ser_id;
                ecs_make_alive(world, ser_id);
            } else {
                /* If id exists and is not alive, create a new id */
                deser_id[0] = flecs_json_new_id(world, ser_id);

                /* Mark new id as reserved */
                flecs_json_mark_reserved(anonymous_ids, deser_id[0]);
            }
        }

        e = deser_id[0];
    } else {
        e = ecs_lookup_path_w_sep(world, 0, name, ".", NULL, false);
        if (!e) {
            e = ecs_entity(world, { .name = name });
            flecs_json_mark_reserved(anonymous_ids, e);
        }
    }

    return e;
}

static
const char* flecs_json_deser_tags(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc,
    ecs_from_json_ctx_t *ctx)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    const char *expr = ctx->expr, *lah;

    json = flecs_json_expect(json, JsonArrayOpen, token, desc);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonArrayClose) {
        json = lah;
        goto end;
    }

    do {
        char *str = NULL;
        json = flecs_json_expect_string(json, token, &str, desc);
        if (!json) {
            goto error;
        }

        ecs_entity_t tag = flecs_json_lookup(world, 0, str, desc);
        ecs_vec_append_t(ctx->a, &ctx->table_type, ecs_id_t)[0] = tag;

        ecs_add_id(world, e, tag);

        if (str != token) {
            ecs_os_free(str);
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind != JsonComma) {
            break;
        }
    } while (true);

    if (token_kind != JsonArrayClose) {
        ecs_parser_error(NULL, expr, json - expr, "expected }");
        goto error;
    }


end:
    return json;
error:
    return NULL;
}

static
const char* flecs_json_deser_pairs(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc,
    ecs_from_json_ctx_t *ctx)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    const char *expr = ctx->expr, *lah;

    json = flecs_json_expect(json, JsonObjectOpen, token, desc);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonObjectClose) {
        json = lah;
        goto end;
    }

    do {
        json = flecs_json_expect_member(json, token, desc);
        if (!json) {
            goto error;
        }

        ecs_entity_t rel = flecs_json_lookup(world, 0, token, desc);

        bool multiple_targets = false;

        do {
            json = flecs_json_parse(json, &token_kind, token);
            
            if (token_kind == JsonString) {
                ecs_entity_t tgt = flecs_json_lookup(world, 0, token, desc);
                ecs_id_t id = ecs_pair(rel, tgt);
                ecs_add_id(world, e, id);
                ecs_vec_append_t(ctx->a, &ctx->table_type, ecs_id_t)[0] = id;
            } else if (token_kind == JsonLargeString) {
                ecs_strbuf_t large_token = ECS_STRBUF_INIT;
                json = flecs_json_parse_large_string(json, &large_token);
                if (!json) {
                    break;
                }

                char *str = ecs_strbuf_get(&large_token);
                ecs_entity_t tgt = flecs_json_lookup(world, 0, str, desc);
                ecs_os_free(str);
                ecs_id_t id = ecs_pair(rel, tgt);
                ecs_add_id(world, e, id);
                ecs_vec_append_t(ctx->a, &ctx->table_type, ecs_id_t)[0] = id;
            } else if (token_kind == JsonArrayOpen) {
                if (multiple_targets) {
                    ecs_parser_error(NULL, expr, json - expr, 
                        "expected string");
                    goto error;
                }

                multiple_targets = true;
            } else if (token_kind == JsonArrayClose) {
                if (!multiple_targets) {
                    ecs_parser_error(NULL, expr, json - expr, 
                        "unexpected ]");
                    goto error;
                }

                multiple_targets = false;
            } else if (token_kind == JsonComma) {
                if (!multiple_targets) {
                    ecs_parser_error(NULL, expr, json - expr, 
                        "unexpected ,");
                    goto error;
                }
            } else {
                ecs_parser_error(NULL, expr, json - expr, 
                    "expected array or string");
                goto error;
            }
        } while (multiple_targets);

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind != JsonComma) {
            break;
        }
    } while (true);

    if (token_kind != JsonObjectClose) {
        ecs_parser_error(NULL, expr, json - expr, "expected }");
        goto error;
    }

end:
    return json;
error:
    return NULL;
}

static
const char* flecs_json_deser_components(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc,
    ecs_from_json_ctx_t *ctx)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    const char *expr = ctx->expr, *lah;

    json = flecs_json_expect(json, JsonObjectOpen, token, desc);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonObjectClose) {
        json = lah;
        goto end;
    }

    do {
        json = flecs_json_expect_member(json, token, desc);
        if (!json) {
            goto error;
        }

        ecs_id_t id = 0;

        if (token[0] != '(') {
            id = flecs_json_lookup(world, 0, token, desc);
        } else {
            char token_buffer[256];
            ecs_term_t term = {0};
            if (!flecs_term_parse(world, NULL, token, &term, token_buffer)) {
                goto error;
            }

            ecs_assert(term.first.name != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(term.second.name != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_entity_t rel = flecs_json_lookup(
                world, 0, term.first.name, desc);
            ecs_entity_t tgt = flecs_json_lookup(
                world, 0, term.second.name, desc);
            
            id = ecs_pair(rel, tgt);
        }

        lah = flecs_json_parse(json, &token_kind, token);
        if (token_kind != JsonNull) {
            ecs_entity_t type = ecs_get_typeid(world, id);
            if (!type) {
                flecs_json_missing_reflection(world, id, json, ctx, desc);
                if (desc->strict) {
                    goto error;
                }

                json = flecs_json_skip_object(json + 1, token, desc);
                if (!json) {
                    goto error;
                }
            } else {
                void *ptr = ecs_ensure_id(world, e, id);

                lah = flecs_json_parse(json, &token_kind, token);
                if (token_kind != JsonNull) {
                    const char *next = ecs_ptr_from_json(
                        world, type, ptr, json, desc);
                    if (!next) {
                        flecs_json_missing_reflection(
                            world, id, json, ctx, desc);
                        if (desc->strict) {
                            goto error;
                        }

                        json = flecs_json_skip_object(json + 1, token, desc);
                        if (!json) {
                            goto error;
                        }
                    } else {
                        json = next;
                        ecs_modified_id(world, e, id);
                    }
                } else {
                    json = lah;
                }
            }
        } else {
            ecs_add_id(world, e, id);
            json = lah;
        }

        ecs_vec_append_t(ctx->a, &ctx->table_type, ecs_id_t)[0] = id;

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind != JsonComma) {
            break;
        }
    } while (true);

    if (token_kind != JsonObjectClose) {
        ecs_parser_error(NULL, expr, json - expr, "expected }");
        goto error;
    }

end:
    return json;
error:
    return NULL;
}

static
const char* flecs_entity_from_json(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc,
    ecs_from_json_ctx_t *ctx)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    const char *expr = ctx->expr, *lah;

    ecs_vec_clear(&ctx->table_type);

    ecs_entity_t parent = 0;

    json = flecs_json_expect(json, JsonObjectOpen, token, desc);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (!lah) {
        goto error;
    }

    if (token_kind == JsonObjectClose) {
        json = lah;
        goto end;
    }

    json = flecs_json_expect_member(json, token, desc);
    if (!json) {
        goto error;
    }

    if (!ecs_os_strcmp(token, "parent")) {
        char *str = NULL;
        json = flecs_json_expect_string(json, token, &str, desc);
        if (!json) {
            goto error;
        }

        parent = flecs_json_lookup(world, 0, str, desc);
        if (e) {
            ecs_add_pair(world, e, EcsChildOf, parent);
            ecs_vec_append_t(ctx->a, &ctx->table_type, ecs_id_t)[0] =
                ecs_pair(EcsChildOf, parent);
        }

        if (str != token) ecs_os_free(str);

        json = flecs_json_parse_next_member(json, token, &token_kind, desc);
        if (!json) {
            goto error;
        }
        if (token_kind == JsonObjectClose) {
            goto end;
        }
    }

    if (!ecs_os_strcmp(token, "name")) {
        char *str = NULL;
        json = flecs_json_expect_string(json, token, &str, desc);
        if (!json) {
            goto error;
        }

        if (!e) {
            e = flecs_json_lookup(world, parent, str, desc);
        } else {
            ecs_set_name(world, e, str);
            ecs_vec_append_t(ctx->a, &ctx->table_type, ecs_id_t)[0] =
                ecs_pair_t(EcsIdentifier, EcsName);
        }

        if (str != token) ecs_os_free(str);

        json = flecs_json_parse_next_member(json, token, &token_kind, desc);
        if (!json) {
            goto error;
        }
        if (token_kind == JsonObjectClose) {
            goto end;
        }
    }

    if (!ecs_os_strcmp(token, "id")) {
        json = flecs_json_parse(json, &token_kind, token);
        if (!json) {
            goto error;
        }

        uint64_t id;
        if (token_kind == JsonNumber || token_kind == JsonLargeInt) {
            id = flecs_ito(uint64_t, atoll(token));
        } else {
            ecs_parser_error(NULL, expr, json - expr, "expected entity id");
            goto error;
        }

        if (!e) {
            char name[32];
            ecs_os_snprintf(name, 32, "#%u", (uint32_t)id);
            e = flecs_json_lookup(world, 0, name, desc);
        } else {
            /* If we already have an id, ignore explicit id */
        }

        json = flecs_json_parse_next_member(json, token, &token_kind, desc);
        if (!json) {
            goto error;
        }
        if (token_kind == JsonObjectClose) {
            goto end;
        }
    }

    if (!e) {
        ecs_parser_error(NULL, expr, json - expr, "failed to create entity");
        return NULL;
    }

    if (!ecs_os_strcmp(token, "alerts")) {
        json = flecs_json_expect(json, JsonBoolean, token, desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_parse_next_member(json, token, &token_kind, desc);
        if (!json) {
            goto error;
        }
        if (token_kind == JsonObjectClose) {
            goto end;
        }
    }

    if (!ecs_os_strcmp(token, "tags")) {
        json = flecs_json_deser_tags(world, e, json, desc, ctx);
        if (!json) {
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonObjectClose) {
            goto end;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(NULL, expr, json - expr, "expected ','");
            goto error;
        }

        json = flecs_json_expect_member(json, token, desc);
        if (!json) {
            goto error;
        }
    }

    if (!ecs_os_strcmp(token, "pairs")) {
        json = flecs_json_deser_pairs(world, e, json, desc, ctx);
        if (!json) {
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonObjectClose) {
            goto end;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(NULL, expr, json - expr, "expected ','");
            goto error;
        }

        json = flecs_json_expect_member(json, token, desc);
        if (!json) {
            goto error;
        }
    }

    if (!ecs_os_strcmp(token, "components")) {
        json = flecs_json_deser_components(world, e, json, desc, ctx);
        if (!json) {
            goto error;
        }
    }

    json = flecs_json_expect(json, JsonObjectClose, token, desc);
    if (!json) {
        goto error;
    }

    ecs_record_t *r = flecs_entities_get(world, e);
    ecs_table_t *table = r ? r->table : NULL;
    if (table) {
        ecs_id_t *ids = ecs_vec_first(&ctx->table_type);
        int32_t ids_count = ecs_vec_count(&ctx->table_type);
        qsort(ids, flecs_itosize(ids_count), sizeof(ecs_id_t), flecs_id_qsort_cmp);

        ecs_table_t *dst_table = ecs_table_find(world, 
            ecs_vec_first(&ctx->table_type), ecs_vec_count(&ctx->table_type));

        /* Entity had existing components that weren't in the serialized data */
        if (table != dst_table) {
            if (!dst_table) {
                ecs_clear(world, e);
            } else {
                ecs_vec_clear(&ctx->remove_ids);

                ecs_type_t *type = &table->type, *dst_type = &dst_table->type;
                int32_t i = 0, i_dst = 0;
                for (; (i_dst < dst_type->count) && (i < type->count); ) {
                    ecs_id_t id = type->array[i], dst_id = dst_type->array[i_dst];

                    if (dst_id > id) {
                        ecs_vec_append_t(
                            ctx->a, &ctx->remove_ids, ecs_id_t)[0] = id;
                    }

                    i_dst += dst_id <= id;
                    i += dst_id >= id;
                }

                ecs_type_t removed = {
                    .array = ecs_vec_first(&ctx->remove_ids),
                    .count = ecs_vec_count(&ctx->remove_ids)
                };

                ecs_commit(world, e, r, dst_table, NULL, &removed);
            }

            ecs_assert(ecs_get_table(world, e) == dst_table, 
                ECS_INTERNAL_ERROR, NULL);
        }
    }

end:
    return json;
error:
    return NULL;
}

const char* ecs_entity_from_json(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc_arg)
{
    ecs_from_json_desc_t desc = {0};
    if (desc_arg) {
        desc = *desc_arg;
    }

    desc.expr = json;

    ecs_allocator_t *a = &world->allocator;
    ecs_from_json_ctx_t ctx;
    flecs_from_json_ctx_init(a, &ctx);
    ctx.expr = json;

    if (!desc.lookup_action) {
        desc.lookup_action = (ecs_entity_t(*)(
            const ecs_world_t*, const char*, void*))flecs_json_ensure_entity;
        desc.lookup_ctx = &ctx.anonymous_ids;
    }

    json = flecs_entity_from_json(world, e, json, &desc, &ctx);

    flecs_from_json_ctx_fini(&ctx);
    return json;
}

const char* ecs_world_from_json(
    ecs_world_t *world,
    const char *json,
    const ecs_from_json_desc_t *desc_arg)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    ecs_from_json_desc_t desc = {0};
    if (desc_arg) {
        desc = *desc_arg;
    }

    desc.expr = json;

    ecs_allocator_t *a = &world->allocator;
    ecs_from_json_ctx_t ctx;
    flecs_from_json_ctx_init(a, &ctx);

    const char *expr = json, *lah;
    ctx.expr = expr;

    if (!desc.lookup_action) {
        desc.lookup_action = (ecs_entity_t(*)(
            const ecs_world_t*, const char*, void*))flecs_json_ensure_entity;
        desc.lookup_ctx = &ctx.anonymous_ids;
    }

    json = flecs_json_expect(json, JsonObjectOpen, token, &desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect_member_name(json, token, "results", &desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(json, JsonArrayOpen, token, &desc);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonArrayClose) {
        json = lah;
        goto end;
    }

    do {
        json = flecs_entity_from_json(world, 0, json, &desc, &ctx);
        if (!json) {
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind != JsonComma) {
            if (token_kind != JsonArrayClose) {
                ecs_parser_error(NULL, expr, json - expr, "expected ']'");
                goto error;
            }
            break;
        }
    } while (true);

end:
    json = flecs_json_expect(json, JsonObjectClose, token, &desc);

    flecs_from_json_ctx_fini(&ctx);
    return json;
error:
    flecs_from_json_ctx_fini(&ctx);
    return NULL;
}

const char* ecs_world_from_json_file(
    ecs_world_t *world,
    const char *filename,
    const ecs_from_json_desc_t *desc)
{
    char *json = flecs_load_from_file(filename);
    if (!json) {
        ecs_err("file not found: %s", filename);
        return NULL;
    }

    const char *result = ecs_world_from_json(world, json, desc);
    ecs_os_free(json);
    return result;
}

#endif
