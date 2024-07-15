/**
 * @file query/engine/trav_down_cache.c
 * @brief Compile query term.
 */

#include "../../private_api.h"

static
void flecs_trav_entity_down_isa(
    ecs_world_t *world,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    ecs_trav_down_t *dst,
    ecs_entity_t trav,
    ecs_entity_t entity,
    ecs_id_record_t *idr_with,
    bool self,
    bool empty);

static
ecs_trav_down_t* flecs_trav_entity_down(
    ecs_world_t *world,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    ecs_trav_down_t *dst,
    ecs_entity_t trav,
    ecs_entity_t entity,
    ecs_id_record_t *idr_trav,
    ecs_id_record_t *idr_with,
    bool self,
    bool empty);

static
ecs_trav_down_t* flecs_trav_down_ensure(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_entity_t entity)
{
    ecs_trav_down_t **trav = ecs_map_ensure_ref(
        &cache->src, ecs_trav_down_t, entity);
    if (!trav[0]) {
        trav[0] = flecs_iter_calloc_t(ctx->it, ecs_trav_down_t);
        ecs_vec_init_t(NULL, &trav[0]->elems, ecs_trav_down_elem_t, 0);
    }

    return trav[0];
}

static
ecs_trav_down_t* flecs_trav_table_down(
    ecs_world_t *world,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    ecs_trav_down_t *dst,
    ecs_entity_t trav,
    const ecs_table_t *table,
    ecs_id_record_t *idr_with,
    bool self,
    bool empty)
{
    ecs_assert(table->id != 0, ECS_INTERNAL_ERROR, NULL);

    if (!table->_->traversable_count) {
        return dst;
    }

    ecs_assert(idr_with != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *entities = ecs_vec_first(&table->data.entities);
    int32_t i, count = ecs_table_count(table);
    for (i = 0; i < count; i ++) {
        ecs_entity_t entity = entities[i];
        ecs_record_t *record = flecs_entities_get(world, entity);
        if (!record) {
            continue;
        }

        uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
        if (flags & EcsEntityIsTraversable) {
            ecs_id_record_t *idr_trav = flecs_id_record_get(world, 
                ecs_pair(trav, entity));
            if (!idr_trav) {
                continue;
            }

            flecs_trav_entity_down(world, a, cache, dst, 
                trav, entity, idr_trav, idr_with, self, empty);
        }
    }

    return dst;
}

static
void flecs_trav_entity_down_isa(
    ecs_world_t *world,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    ecs_trav_down_t *dst,
    ecs_entity_t trav,
    ecs_entity_t entity,
    ecs_id_record_t *idr_with,
    bool self,
    bool empty)
{
    if (trav == EcsIsA || !world->idr_isa_wildcard) {
        return;
    }

    ecs_id_record_t *idr_isa = flecs_id_record_get(
        world, ecs_pair(EcsIsA, entity));
    if (!idr_isa) {
        return;
    }

    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&idr_isa->cache, &it)) {
        ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            if (!table->_->traversable_count) {
                continue;
            }

            ecs_entity_t *entities = ecs_vec_first(&table->data.entities);
            int32_t i, count = ecs_table_count(table);
            for (i = 0; i < count; i ++) {
                ecs_entity_t e = entities[i];
                ecs_record_t *record = flecs_entities_get(world, e);
                if (!record) {
                    continue;
                }

                uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
                if (flags & EcsEntityIsTraversable) {
                    ecs_id_record_t *idr_trav = flecs_id_record_get(world, 
                        ecs_pair(trav, e));
                    if (idr_trav) {
                        flecs_trav_entity_down(world, a, cache, dst, trav, e,
                            idr_trav, idr_with, self, empty);
                    }
                }
            }
        }
    }
}

static
ecs_trav_down_t* flecs_trav_entity_down(
    ecs_world_t *world,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    ecs_trav_down_t *dst,
    ecs_entity_t trav,
    ecs_entity_t entity,
    ecs_id_record_t *idr_trav,
    ecs_id_record_t *idr_with,
    bool self,
    bool empty)
{
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(idr_with != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(idr_trav != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_trav_entity_down_isa(
        world, a, cache, dst, trav, entity, idr_with, self, empty);

    int32_t first = ecs_vec_count(&dst->elems);

    ecs_table_cache_iter_t it;
    bool result;
    if (empty) {
        result = flecs_table_cache_all_iter(&idr_trav->cache, &it);
    } else {
        result = flecs_table_cache_iter(&idr_trav->cache, &it);
    }
    if (result) {
        ecs_table_record_t *tr; 
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);
            ecs_table_t *table = tr->hdr.table;
            bool leaf = false;

            if (flecs_id_record_get_table(idr_with, table) != NULL) {
                if (self) {
                    continue;
                }
                leaf = true;
            }

            /* If record is not the first instance of (trav, *), don't add it
             * to the cache. */
            int32_t index = tr->index;
            if (index) {
                ecs_id_t id = table->type.array[index - 1];
                if (ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == trav) {
                    int32_t col = ecs_search_relation(world, table, 0, 
                        idr_with->id, trav, EcsUp, NULL, NULL, &tr);
                    ecs_assert(col >= 0, ECS_INTERNAL_ERROR, NULL);

                    if (col != index) {
                        /* First relationship through which the id is 
                         * reachable is not the current one, so skip. */
                        continue;
                    }
                }
            }

            ecs_trav_down_elem_t *elem = ecs_vec_append_t(
                a, &dst->elems, ecs_trav_down_elem_t);
            elem->table = table;
            elem->leaf = leaf;
        }
    }

    /* Breadth first walk */
    int32_t t, last = ecs_vec_count(&dst->elems);
    for (t = first; t < last; t ++) {
        ecs_trav_down_elem_t *elem = ecs_vec_get_t(
            &dst->elems, ecs_trav_down_elem_t, t);
        if (!elem->leaf) {
            flecs_trav_table_down(world, a, cache, dst, trav,
                elem->table, idr_with, self, empty);
        }
    }

    return dst;
}

ecs_trav_down_t* flecs_query_get_down_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_entity_t trav,
    ecs_entity_t e,
    ecs_id_record_t *idr_with,
    bool self,
    bool empty)
{
    ecs_world_t *world = ctx->it->real_world;
    ecs_assert(cache->dir != EcsTravUp, ECS_INTERNAL_ERROR, NULL);
    cache->dir = EcsTravDown;

    ecs_allocator_t *a = flecs_query_get_allocator(ctx->it);
    ecs_map_init_if(&cache->src, a);

    ecs_trav_down_t *result = flecs_trav_down_ensure(ctx, cache, e);
    if (result->ready) {
        return result;
    }

    ecs_id_record_t *idr_trav = flecs_id_record_get(world, ecs_pair(trav, e));
    if (!idr_trav) {
        if (trav != EcsIsA) {
            flecs_trav_entity_down_isa(
                world, a, cache, result, trav, e, idr_with, self, empty);
        }
        result->ready = true;
        return result;
    }

    ecs_vec_init_t(a, &result->elems, ecs_trav_down_elem_t, 0);
    flecs_trav_entity_down(
        world, a, cache, result, trav, e, idr_trav, idr_with, self, empty);
    result->ready = true;

    return result;
}

void flecs_query_down_cache_fini(
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache)
{
    ecs_map_iter_t it = ecs_map_iter(&cache->src);
    while (ecs_map_next(&it)) {
        ecs_trav_down_t *t = ecs_map_ptr(&it);
        ecs_vec_fini_t(a, &t->elems, ecs_trav_down_elem_t);
    }
    ecs_map_fini(&cache->src);
}
