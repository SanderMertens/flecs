/**
 * @file id.c
 * @brief Id utilities.
 */

#include "private_api.h"

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
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (idr) {
        return idr->flags;
    } else {
        return 0;
    }
}
