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
        ecs_check(ECS_IS_VALUE_PAIR(id) || id_second != 0,
            ECS_INVALID_PARAMETER,
            "second element of pair cannot be 0");

        ecs_check(pattern_first != 0, ECS_INVALID_PARAMETER,
            "first element of pair cannot be 0");
        ecs_check(ECS_IS_VALUE_PAIR(pattern) || pattern_second != 0,
            ECS_INVALID_PARAMETER,
            "second element of pair cannot be 0");

        if (ECS_IS_NESTED_PAIR(pattern) && !ECS_IS_NESTED_PAIR(id)) {
            return false;
        }

        bool pattern_first_wildcard = pattern_first == EcsWildcard;
        bool pattern_second_wc = pattern_second == EcsWildcard;
        if (ECS_IS_VALUE_PAIR(pattern)) {
            if (!ECS_IS_VALUE_PAIR(id)) {
                return false;
            }

            pattern_first_wildcard = false;   
            pattern_second_wc = false;
        }
        
        if (pattern_first_wildcard) {
            if (pattern_second_wc || pattern_second == id_second) {
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
    if (ECS_IS_VALUE_PAIR(id)) {
        return (first == EcsWildcard) || (first == EcsAny);
    }

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

const char* flecs_id_invalid_reason(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (!id) {
        return "components cannot be 0 (is the component registered?)";
    }

    if (ecs_id_is_wildcard(id)) {
        return "cannot add wildcards";
    }

    if (ECS_IS_VALUE_PAIR(id)) {
        if (!ECS_PAIR_FIRST(id)) {
            return "invalid value pair: first element is 0 (is the relationship registered?)";
        }
    } else if (ECS_HAS_ID_FLAG(id, PAIR)) {
        if (ECS_IS_NESTED_PAIR(id)) {
            if (!ECS_PAIR_FIRST(id)) {
                return "invalid nested pair: first element is 0";
            }

            if (!flecs_nested_pair_handle_id(world, ECS_PAIR_SECOND(id))) {
                return "invalid nested pair: target is not a nested pair "
                    "handle (use ecs_nested_pair())";
            }

            return NULL;
        }

        if (!ECS_PAIR_FIRST(id) && !ECS_PAIR_SECOND(id)) {
            return "invalid pair: both elements are 0";
        }

        if (!ECS_PAIR_FIRST(id)) {
            return "invalid pair: first element is 0 (is the relationship registered?)";
        }

        if (!ECS_PAIR_SECOND(id)) {
            return "invalid pair: second element is 0";
        }
    } else if (id & ECS_ID_FLAGS_MASK) {
        if (!ecs_is_valid(world, id & ECS_COMPONENT_MASK)) {
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
        }
    }

    return NULL;
}

bool ecs_id_is_valid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    return flecs_id_invalid_reason(world, id) == NULL;
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
    uint64_t entity)
{
    if (ECS_IS_VALUE_PAIR(entity)) {
        return "VALUE_PAIR";
    } else
    if (ECS_IS_NESTED_PAIR(entity)) {
        return "NESTED_PAIR";
    } else
    if (ECS_HAS_ID_FLAG(entity, PAIR)) {
        return "PAIR";
    } else
    if (ECS_HAS_ID_FLAG(entity, TOGGLE)) {
        return "TOGGLE";
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

    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        ecs_entity_t tgt = ECS_PAIR_SECOND(id);

        ecs_entity_t e;
        if ((e = ecs_get_alive(world, rel))) {
            rel = e;
        }

        ecs_strbuf_appendch(buf, '(');
        ecs_get_path_w_sep_buf(world, 0, rel, NULL, NULL, buf, false);
        ecs_strbuf_appendch(buf, ',');

        ecs_id_t nested;
        if (ECS_IS_VALUE_PAIR(id)) {
            ecs_strbuf_appendlit(buf, "@");
            ecs_strbuf_appendint(buf, (uint32_t)tgt);
        } else if (ECS_IS_NESTED_PAIR(id) && 
            (nested = flecs_nested_pair_handle_id(world, tgt))) 
        {
            ecs_id_str_buf(world, nested, buf);
        } else {
            if ((e = ecs_get_alive(world, tgt))) {
                tgt = e;
            }

            ecs_get_path_w_sep_buf(world, 0, tgt, NULL, NULL, buf, false);
        }

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
        ECS_INVALID_PARAMETER, 
        "cannot create nested pairs with ecs_pair() (use ecs_nested_pair())");
    return ecs_pair(relationship, target);
}

ecs_id_t flecs_nested_pair_handle_id(
    const ecs_world_t *world,
    ecs_entity_t handle)
{
    world = ecs_get_world(world);
    ecs_map_val_t *id = ecs_map_get(
        &world->nested_pair_ids, (uint32_t)handle);
    if (!id) {
        return 0;
    }

    return id[0];
}

static void flecs_nested_pair_handle_remove(
    ecs_world_t *world,
    ecs_entity_t handle)
{
    ecs_map_val_t *id = ecs_map_get(
        &world->nested_pair_ids, (uint32_t)handle);
    if (!id) {
        return;
    }

    ecs_map_val_t *h = ecs_map_get(&world->nested_pairs, id[0]);
    if (h && ((uint32_t)h[0] == (uint32_t)handle)) {
        ecs_map_remove(&world->nested_pairs, id[0]);
    }

    ecs_map_remove(&world->nested_pair_ids, (uint32_t)handle);
}

void flecs_on_remove_nested_pair_of(
    ecs_iter_t *it)
{
    ecs_world_t *world = ECS_CONST_CAST(ecs_world_t*, 
        ecs_get_world(it->world));
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        flecs_nested_pair_handle_remove(world, it->entities[i]);
    }
}

static ecs_entity_t flecs_nested_pair_handle_ensure(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_world_t *real_world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    ecs_map_val_t *h = ecs_map_get(&real_world->nested_pairs, id);
    if (h) {
        if (ecs_is_alive(world, h[0])) {
            return h[0];
        }

        /* Handle was deleted without cleaning up the administration */
        flecs_nested_pair_handle_remove(real_world, h[0]);
    }

    ecs_check(!ecs_id_is_wildcard(id), ECS_INVALID_PARAMETER,
        "target of nested pair cannot be a wildcard");

    ecs_entity_t first = ecs_get_alive(world, ECS_PAIR_FIRST(id));
    ecs_check(first != 0, ECS_INVALID_PARAMETER,
        "first element of nested pair target is not alive");

    ecs_entity_t second = 0;
    if (!ECS_IS_VALUE_PAIR(id)) {
        second = ecs_get_alive(world, ECS_PAIR_SECOND(id));
        ecs_check(second != 0, ECS_INVALID_PARAMETER,
            "second element of nested pair target is not alive");
    }

    ecs_entity_t handle = ecs_new(world);
    ecs_map_insert(&real_world->nested_pairs, id, handle);
    ecs_map_insert(&real_world->nested_pair_ids, (uint32_t)handle, id);

    /* Delete the handle when one of the pair elements is deleted. This removes
     * the nested pair from all entities that have it. */
    ecs_add_pair(world, handle, EcsNestedPairOf, first);
    if (second && second != first) {
        ecs_add_pair(world, handle, EcsNestedPairOf, second);
    }

    return handle;
error:
    return 0;
}

ecs_id_t ecs_make_nested_pair(
    ecs_world_t *world,
    ecs_entity_t first,
    ecs_id_t second)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(first != 0, ECS_INVALID_PARAMETER,
        "first element of nested pair cannot be 0");
    ecs_check(!(first & ECS_ID_FLAGS_MASK), ECS_INVALID_PARAMETER,
        "first element of nested pair cannot be a pair or have flags");
    ecs_check(second != 0, ECS_INVALID_PARAMETER,
        "second element of nested pair cannot be 0");

    if (!ECS_IS_PAIR(second)) {
        ecs_check(!(second & ECS_ID_FLAGS_MASK), ECS_INVALID_PARAMETER,
            "target of nested pair cannot have flags");
        return ecs_pair(first, second);
    }

    ecs_entity_t handle = flecs_nested_pair_handle_ensure(world, second);
    if (!handle) {
        return 0;
    }

    return ECS_NESTED_PAIR | ecs_entity_t_comb(handle, first);
error:
    return 0;
}

ecs_id_t ecs_pair_second_id(
    const ecs_world_t *world,
    ecs_id_t pair)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ECS_IS_PAIR(pair), ECS_INVALID_PARAMETER, "id is not a pair");

    if (ECS_IS_NESTED_PAIR(pair)) {
        return flecs_nested_pair_handle_id(world, ECS_PAIR_SECOND(pair));
    }

    if (ECS_IS_VALUE_PAIR(pair)) {
        return ECS_PAIR_SECOND(pair);
    }

    return ecs_get_alive(world, ECS_PAIR_SECOND(pair));
error:
    return 0;
}

bool ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (ecs_id_is_wildcard(id)) {
        /* If id is a wildcard, we can't tell if it's a tag or not, except
         * when the relationship part of a pair has the PairIsTag property */
        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            ecs_entity_t first = ECS_PAIR_FIRST(id);
            if (first != EcsWildcard && first != EcsAny) {
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
                /* If relationship is wildcard, id is not guaranteed to be a tag */
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
