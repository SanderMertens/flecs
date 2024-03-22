/**
 * @file json/serialize_query_info.c
 * @brief Serialize (component) values to JSON strings.
 */

#include "json.h"

#ifdef FLECS_JSON

static
const char* flecs_json_inout_str(
    ecs_inout_kind_t kind)
{
    switch(kind) {
    case EcsIn: return "in";
    case EcsOut: return "out";
    case EcsInOut: return "inout";
    case EcsInOutNone: return "none";
    case EcsInOutDefault: return "default";
    default: return "unknown";
    }
}

static
const char* flecs_json_oper_str(
    ecs_oper_kind_t kind)
{
    switch(kind) {
    case EcsAnd: return "and";
    case EcsNot: return "not";
    case EcsOr: return "or";
    case EcsOptional: return "optional";
    case EcsAndFrom: return "andfrom";
    case EcsNotFrom: return "notfrom";
    case EcsOrFrom: return "orfrom";
    default: return "unknown";
    }
}

static
void flecs_json_serialize_term_entity(
    const ecs_world_t *world,
    ecs_entity_t e,
    ecs_strbuf_t *buf)
{
    flecs_json_memberl(buf, "entity");
    flecs_json_path(buf, world, e);

    if (e) {
        const char *symbol = ecs_get_symbol(world, e);
        if (symbol) {
            flecs_json_memberl(buf, "symbol");
            flecs_json_string(buf, symbol);
        }

        if (ecs_has(world, e, EcsComponent)) {
            flecs_json_memberl(buf, "type");
            flecs_json_true(buf);
        }
    }
}

static
void flecs_json_serialize_term_ref(
    const ecs_world_t *world,
    const ecs_term_id_t *ref,
    ecs_strbuf_t *buf)
{
    flecs_json_object_push(buf);
    if (ref->flags & EcsIsEntity) {
        flecs_json_serialize_term_entity(world, ref->id, buf);
    } else if (ref->flags & EcsIsVariable) {
        flecs_json_memberl(buf, "var");
        if (ref->name) {
            flecs_json_string(buf, ref->name);
        } else if (ref->id) {
            if (ref->id == EcsThis) {
                flecs_json_string(buf, "this");
            } else {
                flecs_json_path(buf, world, ref->id);
            }
        }
    } else if (ref->flags & EcsIsName) {
        flecs_json_memberl(buf, "name");
        flecs_json_string(buf, ref->name);
    }
    flecs_json_object_pop(buf);
}

static
void flecs_json_serialize_term_trav(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_strbuf_t *buf)
{
    if (term->src.trav) {
        flecs_json_memberl(buf, "trav");
        flecs_json_object_push(buf);
        flecs_json_serialize_term_entity(world, term->src.trav, buf);
        flecs_json_object_pop(buf);
    }

    flecs_json_memberl(buf, "flags");
    flecs_json_array_push(buf);
    if (term->src.flags & EcsSelf) {
        flecs_json_next(buf);
        flecs_json_string(buf, "self");
    }
    if (term->src.flags & EcsCascade) {
        flecs_json_next(buf);
        flecs_json_string(buf, "cascade");
    } else
    if (term->src.flags & EcsUp) {
        flecs_json_next(buf);
        flecs_json_string(buf, "up");
    }
    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_term(
    const ecs_world_t *world,
    const ecs_filter_t *q,
    int t,
    ecs_strbuf_t *buf)
{
    ecs_term_t *term = &q->terms[t];

    flecs_json_object_push(buf);
    flecs_json_memberl(buf, "inout");
    flecs_json_string(buf, flecs_json_inout_str(term->inout));

    flecs_json_memberl(buf, "has_data");
    flecs_json_bool(buf, 0 == (term->flags & EcsTermNoData));

    if (term->first.flags & EcsIsEntity && term->first.id) {
        if (ecs_has_id(world, term->first.id, EcsDontInherit)) {
            flecs_json_memberl(buf, "dont_inherit");
            flecs_json_true(buf);
        }
    }

    flecs_json_memberl(buf, "oper");
    flecs_json_string(buf, flecs_json_oper_str(term->oper));

    flecs_json_memberl(buf, "src");
    flecs_json_serialize_term_ref(world, &term->src, buf);

    flecs_json_memberl(buf, "first");
    flecs_json_serialize_term_ref(world, &term->first, buf);
    
    if (term->second.id || term->second.name || term->second.flags & EcsIsEntity) {
        flecs_json_memberl(buf, "second");
        flecs_json_serialize_term_ref(world, &term->second, buf);
    }

    flecs_json_serialize_term_trav(world, term, buf);

    flecs_json_object_pop(buf);
}

void flecs_json_serialize_query(
    const ecs_world_t *world,
    const ecs_filter_t *q,
    ecs_strbuf_t *buf)
{
    flecs_json_object_push(buf);
    flecs_json_memberl(buf, "terms");
    flecs_json_array_push(buf);
    int t;
    for (t = 0; t < q->term_count; t ++) {
        flecs_json_next(buf);
        flecs_json_serialize_term(world, q, t, buf);
    }
    flecs_json_array_pop(buf);
    flecs_json_object_pop(buf);
}

#endif
