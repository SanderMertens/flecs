/**
 * @file addons/json/deserialize.c
 * @brief Deserialize JSON strings into entities and worlds.
 */

#include "../../private_api.h"
#include "json.h"
#include "../query_dsl/query_dsl.h"

#ifdef FLECS_JSON

typedef struct {
    ecs_allocator_t *a;
    ecs_vec_t ids;
    ecs_map_t anonymous_ids;
    ecs_map_t missing_reflection;
    const char *expr;
} ecs_from_json_ctx_t;

static void flecs_from_json_ctx_init(
    ecs_allocator_t *a,
    ecs_from_json_ctx_t *ctx)
{
    ctx->a = a;
    ecs_vec_init_t(a, &ctx->ids, ecs_id_t, 0);
    ecs_map_init(&ctx->anonymous_ids, a);
    ecs_map_init(&ctx->missing_reflection, a);
}

static void flecs_from_json_ctx_fini(
    ecs_from_json_ctx_t *ctx)
{
    ecs_vec_fini_t(ctx->a, &ctx->ids, ecs_id_t);
    ecs_map_fini(&ctx->anonymous_ids);
    ecs_map_fini(&ctx->missing_reflection);
}

static ecs_entity_t flecs_json_new_id(
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

static void flecs_json_missing_reflection(
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

static ecs_entity_t flecs_json_lookup(
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

static ecs_entity_t flecs_json_lookup_id(
    ecs_world_t *world,
    ecs_entity_t id,
    const ecs_from_json_desc_t *desc)
{
    if (ecs_is_alive(world, id) && !ecs_get_name(world, id)) {
        return id;
    }
    char name[32];
    ecs_os_snprintf(name, 32, "#%u", (uint32_t)id);
    return flecs_json_lookup(world, 0, name, desc);
}

static void flecs_json_mark_reserved(
    ecs_map_t *anonymous_ids,
    ecs_entity_t e)
{
    ecs_entity_t *reserved = ecs_map_ensure(anonymous_ids, e);
    ecs_assert(reserved[0] == 0, ECS_INTERNAL_ERROR, NULL);
    reserved[0] = 0;
}

static ecs_entity_t flecs_json_ensure_entity(
    ecs_world_t *world,
    const char *name,
    void *ctx)
{
    ecs_entity_t e = 0;
    ecs_map_t *anonymous_ids = ctx;

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

static void flecs_json_track_id(
    ecs_from_json_ctx_t *ctx,
    ecs_id_t id)
{
    if (id != ecs_pair_t(EcsIdentifier, EcsName) &&
        !(ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsChildOf))
    {
        ecs_vec_append_t(ctx->a, &ctx->ids, ecs_id_t)[0] = id;
    }
}

static bool flecs_json_has_id(
    const ecs_from_json_ctx_t *ctx,
    ecs_id_t id)
{
    return bsearch(&id, ecs_vec_first(&ctx->ids),
        flecs_itosize(ecs_vec_count(&ctx->ids)), sizeof(ecs_id_t),
        flecs_id_qsort_cmp) != NULL;
}

static const char* flecs_json_deser_ids(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t rel,
    const char *json,
    const ecs_from_json_desc_t *desc,
    ecs_from_json_ctx_t *ctx)
{
    char token[ECS_MAX_TOKEN_SIZE];
    ecs_json_token_t kind;
    const char *next = flecs_json_parse(json, &kind, token);
    bool array = kind == JsonArrayOpen;
    if (array) {
        json = next;
        next = flecs_json_parse(json, &kind, token);
        if (kind == JsonArrayClose) {
            return next;
        }
    } else if (!rel) {
        return flecs_json_expect(json, JsonArrayOpen, token, desc);
    }
    do {
        char *str = NULL;
        json = flecs_json_expect_string(json, token, &str, desc);
        if (!json) {
            return NULL;
        }
        ecs_entity_t target = flecs_json_lookup(world, 0, str, desc);
        if (str != token) ecs_os_free(str);
        ecs_id_t id = rel ? ecs_pair(rel, target) : target;
        ecs_add_id(world, e, id);
        flecs_json_track_id(ctx, id);
        if (!array) {
            return json;
        }
        next = flecs_json_parse(json, &kind, token);
        if (kind != JsonComma) {
            return flecs_json_expect(json, JsonArrayClose, token, desc);
        }
        json = next;
    } while (json);
    return NULL;
}

static const char* flecs_json_deser_pairs(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc,
    ecs_from_json_ctx_t *ctx)
{
    char token[ECS_MAX_TOKEN_SIZE];
    ecs_json_token_t kind;
    json = flecs_json_expect(json, JsonObjectOpen, token, desc);
    if (!json) {
        return NULL;
    }
    const char *next = flecs_json_parse(json, &kind, token);
    if (kind == JsonObjectClose) {
        return next;
    }
    json = flecs_json_expect_member(json, token, desc);
    while (json) {
        ecs_entity_t rel = flecs_json_lookup(world, 0, token, desc);
        json = flecs_json_deser_ids(world, e, rel, json, desc, ctx);
        if (!json) {
            return NULL;
        }
        json = flecs_json_parse_next_member(json, token, &kind, desc);
        if (kind == JsonObjectClose) {
            return json;
        }
    }
    return NULL;
}

static const char* flecs_json_deser_components(
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
            if (!flecs_term_parse(world, NULL, token, token_buffer, &term)) {
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
        if (token_kind == JsonNull) {
            ecs_add_id(world, e, id);
            json = lah;
        } else {
            const ecs_type_info_t *ti = ecs_get_type_info(world, id);
            bool skip = ECS_IS_PAIR(id) &&
                ECS_PAIR_FIRST(id) == ecs_id(EcsIdentifier) &&
                ECS_PAIR_SECOND(id) != EcsAlias;
            const char *next = NULL;
            if (ti) {
                void *ptr = ecs_ensure_id(world, e, id, flecs_ito(size_t, ti->size));
                if (!skip) {
                    next = ecs_ptr_from_json(world, ti->component, ptr, json, desc);
                    if (next) {
                        ecs_modified_id(world, e, id);
                    }
                }
            }
            if (!next) {
                if (!skip || !ti) {
                    flecs_json_missing_reflection(world, id, json, ctx, desc);
                    if (desc->strict) {
                        goto error;
                    }
                }
                json = flecs_parse_ws_eol(json);
                next = flecs_json_skip_object(json + 1, token, desc);
                if (!next) {
                    goto error;
                }
                if (!ti) {
                    ecs_add_id(world, e, id);
                }
            }
            json = next;
        }

        /* Don't add ids that have their own fields in serialized data. */
        flecs_json_track_id(ctx, id);

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

static const char* flecs_entity_from_json(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc,
    ecs_from_json_ctx_t *ctx)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];
    const char *expr = ctx->expr;
    static const char *fields[] = {
        "parent", "name", "id", "version", "has_alerts", "tags", "pairs", "components"
    };
    enum { Parent, Name, Id, Version, Alerts, Tags, Pairs, Components };
    ecs_entity_t parent = 0, serialized_id = 0;
    bool has_id = false, replace_table = false;
    int32_t previous = -1;
    ecs_vec_clear(&ctx->ids);
    json = flecs_json_expect(json, JsonObjectOpen, token, desc);
    if (!json) {
        goto error;
    }
    const char *next = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonObjectClose) {
        json = next;
        goto end;
    }
    json = flecs_json_expect_member(json, token, desc);
    while (json) {
        int32_t field;
        for (field = previous + 1; field < 8; field ++) {
            if (!ecs_os_strcmp(token, fields[field])) {
                break;
            }
        }
        if (field == 8 ||
            (field == Version && previous != Id))
        {
            ecs_parser_error(desc->name, expr, json - expr,
                "unexpected entity member '%s'", token);
            goto error;
        }
        if (!e && has_id && field > Version) {
            e = flecs_json_lookup_id(world, serialized_id, desc);
        }
        if (!e && field > Version) {
            ecs_parser_error(desc->name, expr, json - expr, "failed to create entity");
            goto error;
        }
        switch (field) {
        case Parent:
        case Name: {
            char *str = NULL;
            json = flecs_json_expect_string(json, token, &str, desc);
            if (!json) goto error;
            if (field == Parent) {
                parent = flecs_json_lookup(world, 0, str, desc);
                if (e) ecs_add_pair(world, e, EcsChildOf, parent);
                ecs_vec_append_t(ctx->a, &ctx->ids, ecs_id_t)[0] = ecs_pair(EcsChildOf, parent);
            } else if (str[0] != '#') {
                if (!e) e = flecs_json_lookup(world, parent, str, desc);
                else ecs_set_name(world, e, str);
                ecs_vec_append_t(ctx->a, &ctx->ids, ecs_id_t)[0] = ecs_pair_t(EcsIdentifier, EcsName);
            }
            if (str != token) ecs_os_free(str);
            break;
        }
        case Id:
        case Version:
            json = flecs_json_parse(json, &token_kind, token);
            if (!json) goto error;
            if (token_kind != JsonNumber && token_kind != JsonLargeInt) {
                ecs_parser_error(desc->name, expr, json - expr, "expected %s", fields[field]);
                goto error;
            }
            serialized_id |= flecs_ito(uint64_t, atoll(token)) << (field == Version ? 32 : 0);
            has_id = true;
            break;
        case Alerts:
            json = flecs_json_expect(json, JsonBoolean, token, desc);
            break;
        case Tags:
            json = flecs_json_deser_ids(world, e, 0, json, desc, ctx);
            break;
        case Pairs:
            json = flecs_json_deser_pairs(world, e, json, desc, ctx);
            break;
        case Components:
            json = flecs_json_deser_components(world, e, json, desc, ctx);
            replace_table = true;
            break;
        }
        if (!json) goto error;
        previous = field;
        json = flecs_json_parse_next_member(json, token, &token_kind, desc);
        if (!json) goto error;
        if (token_kind == JsonObjectClose) {
            break;
        }
    }
    if (!e && has_id) {
        e = flecs_json_lookup_id(world, serialized_id, desc);
    }

end:
    if (e) {
        qsort(ecs_vec_first(&ctx->ids),
            flecs_itosize(ecs_vec_count(&ctx->ids)), sizeof(ecs_id_t),
            flecs_id_qsort_cmp);
        ecs_record_t *r = flecs_entities_get(world, e);
        ecs_defer_begin(world);
        if (replace_table && r && r->table) {
            const ecs_type_t *type = &r->table->type;
            for (int32_t i = 0; i < type->count; i ++) {
                ecs_id_t id = type->array[i];
                if (!flecs_json_has_id(ctx, id)) {
                    ecs_assert(ecs_get_target(world, e, EcsChildOf, 0)
                        != EcsFlecsCore, ECS_INVALID_OPERATION, NULL);
                    ecs_remove_id(world, e, id);
                }
            }
        }
        if (r && (r->row & EcsEntityHasDontFragment)) {
            ecs_component_record_t *cr = world->cr_non_fragmenting_head;
            for (; cr; cr = cr->non_fragmenting.next) {
                if (cr->sparse && !ecs_id_is_wildcard(cr->id) &&
                    flecs_sparse_has(cr->sparse, e) &&
                    !flecs_json_has_id(ctx, cr->id))
                {
                    ecs_remove_id(world, e, cr->id);
                }
            }
        }
        ecs_defer_end(world);
    }

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
    ecs_assert(!ecs_is_deferred(world), ECS_INVALID_OPERATION, 
        "cannot deserialize while world is deferred");

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
        desc.lookup_action = flecs_json_ensure_entity;
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
        desc.lookup_action = flecs_json_ensure_entity;
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
