/**
 * @file id.c
 * @brief Id utilities.
 */

#include "private_api.h"

#ifdef FLECS_QUERY_DSL
#include "addons/query_dsl/query_dsl.h"
#endif

bool ecs_id_match(
    ecs_id_t id,
    ecs_id_t pattern)
{
    if (id == pattern) {
        return true;
    }

    if (ECS_HAS_ID_FLAG(pattern, PAIR)) {
        if (!ECS_HAS_ID_FLAG(id, PAIR)) {
            return false;
        }

        ecs_entity_t id_first = ECS_PAIR_FIRST(id);
        ecs_entity_t id_second = ECS_PAIR_SECOND(id);
        ecs_entity_t pattern_first = ECS_PAIR_FIRST(pattern);
        ecs_entity_t pattern_second = ECS_PAIR_SECOND(pattern);

        ecs_check(id_first != 0, ECS_INVALID_PARAMETER, 
            "first element of pair cannot be 0");
        ecs_check(id_second != 0, ECS_INVALID_PARAMETER, 
            "second element of pair cannot be 0");

        ecs_check(pattern_first != 0, ECS_INVALID_PARAMETER,
            "first element of pair cannot be 0");
        ecs_check(pattern_second != 0, ECS_INVALID_PARAMETER,
            "second element of pair cannot be 0");
        
        if (pattern_first == EcsWildcard) {
            if (pattern_second == EcsWildcard || pattern_second == id_second) {
                return true;
            }
        } else if (pattern_first == EcsFlag) {
            /* Used for internals, helps to keep track of which ids are used in
             * pairs that have additional flags (like OVERRIDE and TOGGLE) */
            if (ECS_HAS_ID_FLAG(id, PAIR) && !ECS_IS_PAIR(id)) {
                if (ECS_PAIR_FIRST(id) == pattern_second) {
                    return true;
                }
                if (ECS_PAIR_SECOND(id) == pattern_second) {
                    return true;
                }
            }
        } else if (pattern_second == EcsWildcard) {
            if (pattern_first == id_first) {
                return true;
            }
        }
    } else {
        if ((id & ECS_ID_FLAGS_MASK) != (pattern & ECS_ID_FLAGS_MASK)) {
            return false;
        }

        if ((ECS_COMPONENT_MASK & pattern) == EcsWildcard) {
            return true;
        }
    }

error:
    return false;
}

bool ecs_id_is_pair(
    ecs_id_t id)
{
    return ECS_HAS_ID_FLAG(id, PAIR);
}

bool ecs_id_is_wildcard(
    ecs_id_t id)
{
    if ((id == EcsWildcard) || (id == EcsAny)) {
        return true;
    }

    bool is_pair = ECS_IS_PAIR(id);
    if (!is_pair) {
        return false;
    }

    ecs_entity_t first = ECS_PAIR_FIRST(id);
    ecs_entity_t second = ECS_PAIR_SECOND(id);

    return (first == EcsWildcard) || (second == EcsWildcard) ||
           (first == EcsAny) || (second == EcsAny);
}

bool ecs_id_is_any(
    ecs_id_t id)
{
    if (id == EcsAny) {
        return true;
    }

    bool is_pair = ECS_IS_PAIR(id);
    if (!is_pair) {
        return false;
    }

    ecs_entity_t first = ECS_PAIR_FIRST(id);
    ecs_entity_t second = ECS_PAIR_SECOND(id);

    return (first == EcsAny) || (second == EcsAny);
}

bool ecs_id_is_valid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (!id) {
        return false;
    }
    if (ecs_id_is_wildcard(id)) {
        return false;
    }

    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        if (!ECS_PAIR_FIRST(id)) {
            return false;
        }
        if (!ECS_PAIR_SECOND(id)) {
            return false;
        }
    } else if (id & ECS_ID_FLAGS_MASK) {
        if (!ecs_is_valid(world, id & ECS_COMPONENT_MASK)) {
            return false;
        }
    }

    return true;
}

ecs_flags32_t ecs_id_get_flags(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (cr) {
        return cr->flags;
    } else {
        return 0;
    }
}

ecs_id_t ecs_id_from_str(
    const ecs_world_t *world,
    const char *expr)
{
#ifdef FLECS_QUERY_DSL
    ecs_id_t result;

    /* Temporarily disable parser logging */
    int prev_level = ecs_log_set_level(-3);
    if (!flecs_id_parse(world, NULL, expr, &result)) {
        /* Invalid expression */
        ecs_log_set_level(prev_level);
        return 0;
    }
    ecs_log_set_level(prev_level);
    return result;
#else
    (void)world;
    (void)expr;
    ecs_abort(ECS_UNSUPPORTED, 
        "ecs_id_from_str requires FLECS_QUERY_DSL addon");
#endif
}

const char* ecs_id_flag_str(
    ecs_entity_t entity)
{
    if (ECS_HAS_ID_FLAG(entity, PAIR)) {
        return "PAIR";
    } else
    if (ECS_HAS_ID_FLAG(entity, TOGGLE)) {
        return "TOGGLE";
    } else
    if (ECS_HAS_ID_FLAG(entity, AUTO_OVERRIDE)) {
        return "AUTO_OVERRIDE";
    } else {
        return "UNKNOWN";
    }
}

void ecs_id_str_buf(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_strbuf_t *buf)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    if (ECS_HAS_ID_FLAG(id, TOGGLE)) {
        ecs_strbuf_appendstr(buf, ecs_id_flag_str(ECS_TOGGLE));
        ecs_strbuf_appendch(buf, '|');
    }

    if (ECS_HAS_ID_FLAG(id, AUTO_OVERRIDE)) {
        ecs_strbuf_appendstr(buf, ecs_id_flag_str(ECS_AUTO_OVERRIDE));
        ecs_strbuf_appendch(buf, '|');
    }

    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        ecs_entity_t obj = ECS_PAIR_SECOND(id);

        ecs_entity_t e;
        if ((e = ecs_get_alive(world, rel))) {
            rel = e;
        }
        if ((e = ecs_get_alive(world, obj))) {
            obj = e;
        }

        ecs_strbuf_appendch(buf, '(');
        ecs_get_path_w_sep_buf(world, 0, rel, NULL, NULL, buf, false);
        ecs_strbuf_appendch(buf, ',');
        ecs_get_path_w_sep_buf(world, 0, obj, NULL, NULL, buf, false);
        ecs_strbuf_appendch(buf, ')');
    } else {
        ecs_entity_t e = id & ECS_COMPONENT_MASK;
        ecs_get_path_w_sep_buf(world, 0, e, NULL, NULL, buf, false);
    }

error:
    return;
}

char* ecs_id_str(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_id_str_buf(world, id, &buf);
    return ecs_strbuf_get(&buf);
}

ecs_id_t ecs_make_pair(
    ecs_entity_t relationship,
    ecs_entity_t target)
{
    ecs_assert(!ECS_IS_PAIR(relationship) && !ECS_IS_PAIR(target), 
        ECS_INVALID_PARAMETER, "cannot create nested pairs");
    return ecs_pair(relationship, target);
}

bool ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (ecs_id_is_wildcard(id)) {
        /* If id is a wildcard, we can't tell if it's a tag or not, except
         * when the relationship part of a pair has the Tag property */
        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            if (ECS_PAIR_FIRST(id) != EcsWildcard) {
                ecs_entity_t rel = ecs_pair_first(world, id);
                if (ecs_is_valid(world, rel)) {
                    if (ecs_has_id(world, rel, EcsPairIsTag)) {
                        return true;
                    }
                } else {
                    /* During bootstrap it's possible that not all ids are valid
                     * yet. Using ecs_get_typeid will ensure correct values are
                     * returned for only those components initialized during
                     * bootstrap, while still asserting if another invalid id
                     * is provided. */
                    if (ecs_get_typeid(world, id) == 0) {
                        return true;
                    }
                }
            } else {
                /* If relationship is wildcard id is not guaranteed to be a tag */
            }
        }
    } else {
        if (ecs_get_typeid(world, id) == 0) {
            return true;
        }
    }

    return false;
}


ecs_entity_t ecs_get_typeid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    if (ti) {
        ecs_assert(ti->component != 0, ECS_INTERNAL_ERROR, NULL);
        return ti->component;
    }
error:
    return 0;
}

bool ecs_id_in_use(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        return false;
    }

    return (flecs_table_cache_count(&cr->cache) != 0);
}
