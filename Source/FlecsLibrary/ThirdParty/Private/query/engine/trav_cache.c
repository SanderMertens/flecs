/**
 * @file query/engine/trav_cache.c
 * @brief Cache that stores the result of graph traversal.
 */

#include "../../private_api.h"

static
void flecs_query_build_down_cache(
    ecs_world_t *world,
    ecs_allocator_t *a,
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_entity_t entity)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, ecs_pair(trav, entity));
    if (!idr) {
        return;
    }

    ecs_trav_elem_t *elem = ecs_vec_append_t(a, &cache->entities, 
        ecs_trav_elem_t);
    elem->entity = entity;
    elem->idr = idr;

    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&idr->cache, &it)) {
        ecs_table_record_t *tr; 
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);
            ecs_table_t *table = tr->hdr.table;
            if (!table->_->traversable_count) {
                continue;
            }

            int32_t i, count = ecs_table_count(table);
            ecs_entity_t *entities = table->data.entities.array;
            for (i = 0; i < count; i ++) {
                ecs_record_t *r = flecs_entities_get(world, entities[i]);
                if (r->row & EcsEntityIsTraversable) {
                    flecs_query_build_down_cache(
                        world, a, ctx, cache, trav, entities[i]);
                }
            }
        }
    }
}

static
void flecs_query_build_up_cache(
    ecs_world_t *world,
    ecs_allocator_t *a,
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_table_t *table,
    const ecs_table_record_t *tr,
    int32_t root_column)
{
    ecs_id_t *ids = table->type.array;
    int32_t i = tr->index, end = i + tr->count;
    bool is_root = root_column == -1;

    for (; i < end; i ++) {
        ecs_entity_t second = ecs_pair_second(world, ids[i]);
        if (is_root) {
            root_column = i;
        }

        ecs_trav_elem_t *el = ecs_vec_append_t(a, &cache->entities, 
            ecs_trav_elem_t);
        el->entity = second;
        el->column = root_column;
        el->idr = NULL;

        ecs_record_t *r = flecs_entities_get_any(world, second);
        if (r->table) {
            ecs_table_record_t *r_tr = flecs_id_record_get_table(
                cache->idr, r->table);
            if (!r_tr) {
                return;
            }
            flecs_query_build_up_cache(world, a, ctx, cache, trav, r->table, 
                r_tr, root_column);
        }
    }
}

void flecs_query_trav_cache_fini(
    ecs_allocator_t *a,
    ecs_trav_cache_t *cache)
{
    ecs_vec_fini_t(a, &cache->entities, ecs_trav_elem_t);
}

void flecs_query_get_trav_down_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_entity_t entity)
{
    if (cache->id != ecs_pair(trav, entity) || cache->up) {
        ecs_world_t *world = ctx->it->real_world;
        ecs_allocator_t *a = flecs_query_get_allocator(ctx->it);
        ecs_vec_reset_t(a, &cache->entities, ecs_trav_elem_t);
        flecs_query_build_down_cache(world, a, ctx, cache, trav, entity);
        cache->id = ecs_pair(trav, entity);
        cache->up = false;
    }
}

void flecs_query_get_trav_up_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_world_t *world = ctx->it->real_world;
    ecs_allocator_t *a = flecs_query_get_allocator(ctx->it);

    ecs_id_record_t *idr = cache->idr;
    if (!idr || idr->id != ecs_pair(trav, EcsWildcard)) {
        idr = cache->idr = flecs_id_record_get(world, 
            ecs_pair(trav, EcsWildcard));
        if (!idr) {
            ecs_vec_reset_t(a, &cache->entities, ecs_trav_elem_t);
            return;
        }
    }

    ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (!tr) {
        ecs_vec_reset_t(a, &cache->entities, ecs_trav_elem_t);
        return;
    }

    ecs_id_t id = table->type.array[tr->index];

    if (cache->id != id || !cache->up) {
        ecs_vec_reset_t(a, &cache->entities, ecs_trav_elem_t);
        flecs_query_build_up_cache(world, a, ctx, cache, trav, table, tr, -1);
        cache->id = id;
        cache->up = true;
    }
}
