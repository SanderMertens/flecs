/**
 * @file query/engine/trav_up_cache.c
 * @brief Compile query term.
 */

#include "../../private_api.h"

static
ecs_trav_up_t* flecs_trav_up_ensure(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    uint64_t table_id)
{
    ecs_trav_up_t **trav = ecs_map_ensure_ref(
        &cache->src, ecs_trav_up_t, table_id);
    if (!trav[0]) {
        trav[0] = flecs_iter_calloc_t(ctx->it, ecs_trav_up_t);
    }

    return trav[0];
}

static
int32_t flecs_trav_type_search(
    ecs_trav_up_t *up,
    const ecs_table_t *table,
    ecs_id_record_t *idr_with,
    ecs_type_t *type)
{    
    ecs_table_record_t *tr = ecs_table_cache_get(&idr_with->cache, table);
    if (tr) {
        up->id = type->array[tr->index];
        return up->column = tr->index;
    }

    return -1;
}

static
int32_t flecs_trav_type_offset_search(
    ecs_trav_up_t *up,
    int32_t offset,
    ecs_id_t with,
    ecs_type_t *type)
{
    ecs_assert(offset > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(with != 0, ECS_INVALID_PARAMETER, NULL);

    while (offset < type->count) {
        ecs_id_t type_id = type->array[offset ++];
        if (ecs_id_match(type_id, with)) {
            up->id = type_id;
            return up->column = offset - 1;
        }
    }

    return -1;
}

static
ecs_trav_up_t* flecs_trav_table_up(
    const ecs_query_run_ctx_t *ctx,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    const ecs_world_t *world,
    ecs_entity_t src,
    ecs_id_t with,
    ecs_id_t rel,
    ecs_id_record_t *idr_with,
    ecs_id_record_t *idr_trav)
{
    ecs_trav_up_t *up = flecs_trav_up_ensure(ctx, cache, src);
    if (up->ready) {
        return up;
    }

    ecs_record_t *src_record = flecs_entities_get_any(world, src);
    ecs_table_t *table = src_record->table;
    if (!table) {
        goto not_found;
    }

    ecs_type_t type = table->type;
    if (flecs_trav_type_search(up, table, idr_with, &type) >= 0) {
        up->src = src;
        goto found;
    }

    ecs_flags32_t flags = table->flags;
    if ((flags & EcsTableHasPairs) && rel) {
        bool is_a = idr_trav == world->idr_isa_wildcard;
        if (is_a) {
            if (!(flags & EcsTableHasIsA)) {
                goto not_found;
            }

            if (!flecs_type_can_inherit_id(world, table, idr_with, with)) {
                goto not_found;
            }
        }

        ecs_trav_up_t up_pair = {0};
        int32_t r_column = flecs_trav_type_search(
                &up_pair, table, idr_trav, &type);

        while (r_column != -1) {
            ecs_entity_t tgt = ECS_PAIR_SECOND(up_pair.id);
            ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_trav_up_t *up_parent = flecs_trav_table_up(ctx, a, cache,
                world, tgt, with, rel, idr_with, idr_trav);
            if (up_parent->column != -1) {
                up->src = up_parent->src;
                up->column = up_parent->column;
                up->id = up_parent->id;
                goto found;
            }

            r_column = flecs_trav_type_offset_search(
                &up_pair, r_column + 1, rel, &type);
        }

        if (!is_a) {
            idr_trav = world->idr_isa_wildcard;
            r_column = flecs_trav_type_search(
                    &up_pair, table, idr_trav, &type);

            while (r_column != -1) {
                ecs_entity_t tgt = ECS_PAIR_SECOND(up_pair.id);
                ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

                ecs_trav_up_t *up_parent = flecs_trav_table_up(ctx, a, cache,
                    world, tgt, with, rel, idr_with, idr_trav);
                if (up_parent->column != -1) {
                    up->src = up_parent->src;
                    up->column = up_parent->column;
                    up->id = up_parent->id;
                    goto found;
                }

                r_column = flecs_trav_type_offset_search(
                    &up_pair, r_column + 1, rel, &type);
            }
        }
    }

not_found:
    up->column = -1;
found:
    up->ready = true;
    return up;
}

ecs_trav_up_t* flecs_query_get_up_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_table_t *table,
    ecs_id_t with,
    ecs_entity_t trav,
    ecs_id_record_t *idr_with,
    ecs_id_record_t *idr_trav)
{
    if (cache->with && cache->with != with) {
        flecs_query_up_cache_fini(cache);
    }

    ecs_world_t *world = ctx->it->real_world;
    ecs_allocator_t *a = flecs_query_get_allocator(ctx->it);
    ecs_map_init_if(&cache->src, a);

    ecs_assert(cache->dir != EcsTravDown, ECS_INTERNAL_ERROR, NULL);
    cache->dir = EcsTravUp;
    cache->with = with;

    ecs_assert(idr_with != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(idr_trav != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_record_t *tr = ecs_table_cache_get(&idr_trav->cache, table);
    if (!tr) {
        return NULL; /* Table doesn't have the relationship */
    }

    int32_t i = tr->index, end = i + tr->count;
    for (; i < end; i ++) {
        ecs_id_t id = table->type.array[i];
        ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        ecs_trav_up_t *result = flecs_trav_table_up(ctx, a, cache, world, tgt,
            with, ecs_pair(trav, EcsWildcard), idr_with, idr_trav);
        ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
        if (result->src != 0) {
            return result;
        }
    }

    return NULL;
}

void flecs_query_up_cache_fini(
    ecs_trav_up_cache_t *cache)
{
    ecs_map_fini(&cache->src);
}
