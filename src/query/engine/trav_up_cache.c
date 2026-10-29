/**
 * @file query/engine/trav_up_cache.c
 * @brief Up traversal cache.
 */

#include "../../private_api.h"

#ifdef FLECS_QUERY_PLANS

static ecs_trav_up_t* flecs_trav_up_ensure(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_entity_t tgt)
{
    ecs_trav_up_t **trav = ecs_map_ensure_ref(
        &cache->src, ecs_trav_up_t, tgt);
    if (!trav[0]) {
        trav[0] = flecs_iter_calloc_t(ctx->it, ecs_trav_up_t);
    }

    return trav[0];
}

static int32_t flecs_trav_type_search(
    ecs_trav_up_t *up,
    const ecs_table_t *table,
    ecs_component_record_t *cr_with,
    ecs_type_t *type)
{    
    ecs_table_record_t *tr = ecs_table_cache_get(&cr_with->cache, table);
    if (tr) {
        up->id = type->array[tr->index];
        up->tr = tr;
        return tr->index;
    }

    return -1;
}

static int32_t flecs_trav_type_offset_search(
    ecs_trav_up_t *up,
    const ecs_table_t *table,
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
            up->tr = &table->_->records[offset - 1];
            return offset - 1;
        }
    }

    return -1;
}

static ecs_trav_up_t* flecs_trav_table_up(
    const ecs_query_run_ctx_t *ctx,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    const ecs_world_t *world,
    ecs_entity_t src,
    ecs_id_t with,
    ecs_id_t rel,
    ecs_component_record_t *cr_with,
    ecs_component_record_t *cr_trav);

static void flecs_trav_table_up_w(
    const ecs_query_run_ctx_t *ctx,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    const ecs_world_t *world,
    ecs_entity_t src,
    ecs_id_t with,
    ecs_id_t rel,
    ecs_component_record_t *cr_with,
    ecs_component_record_t *cr_trav,
    ecs_trav_up_t *up)
{
    ecs_record_t *src_record = flecs_entities_get_any(world, src);
    ecs_table_t *table = src_record->table;
    if (!table) {
        goto not_found;
    }

    ecs_type_t type = table->type;
    if (cr_with->flags & EcsIdDontFragment) {
        if (flecs_sparse_has(cr_with->sparse, src)) {
            up->src = src;
            up->tr = NULL;
            up->id = cr_with->id;
            goto found;
        }
    } else {
        if (flecs_trav_type_search(up, table, cr_with, &type) >= 0) {
            up->src = src;
            goto found;
        } else if (ECS_PAIR_FIRST(with) == EcsChildOf) {
            if (table->flags & EcsTableHasParent) {
                const EcsParent *p = ecs_table_get_id(
                    world, table, ecs_id(EcsParent), 
                    ECS_RECORD_TO_ROW(src_record->row));
                ecs_assert(p != NULL, ECS_INTERNAL_ERROR, NULL);
                
                if ((uint32_t)p->value == ECS_PAIR_SECOND(with)) {
                    up->src = src;
                    up->tr = NULL;
                    up->id = cr_with->id;
                    goto found;
                }
            }
        }
    }

    ecs_flags32_t flags = table->flags;
    if ((flags & EcsTableHasPairs) && rel) {
        bool is_a = cr_trav == world->cr_isa_wildcard;
        if (is_a) {
            if (!(flags & EcsTableHasIsA)) {
                goto not_found;
            }

            if (!flecs_type_can_inherit_id(world, table, cr_with, with)) {
                goto not_found;
            }
        }

        if ((rel == ecs_pair(EcsChildOf, EcsWildcard) && 
            (flags & EcsTableHasParent))) 
        {
            const EcsParent *p = ecs_table_get_id(
                world, table, ecs_id(EcsParent), 
                ECS_RECORD_TO_ROW(src_record->row));
            ecs_assert(p != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_trav_up_t *up_parent = flecs_trav_table_up(ctx, a, cache,
                world, p->value, with, rel, cr_with, cr_trav);
            if (up_parent->src) {
                up->src = up_parent->src;
                up->tr = up_parent->tr;
                up->id = up_parent->id;
                goto found;
            }   
        }

        ecs_trav_up_t up_pair = {0};
        int32_t r_column = flecs_trav_type_search(
            &up_pair, table, cr_trav, &type);

        while (r_column != -1) {
            ecs_entity_t tgt = ECS_PAIR_SECOND(up_pair.id);
            ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_trav_up_t *up_parent = flecs_trav_table_up(ctx, a, cache,
                world, tgt, with, rel, cr_with, cr_trav);
            if (up_parent->src) {
                up->src = up_parent->src;
                up->tr = up_parent->tr;
                up->id = up_parent->id;
                goto found;
            }

            r_column = flecs_trav_type_offset_search(
                &up_pair, table, r_column + 1, rel, &type);
        }

        if (!is_a && (cr_with->flags & EcsIdOnInstantiateInherit)) {
            cr_trav = world->cr_isa_wildcard;
            r_column = flecs_trav_type_search(
                    &up_pair, table, cr_trav, &type);

            while (r_column != -1) {
                ecs_entity_t tgt = ECS_PAIR_SECOND(up_pair.id);
                ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

                ecs_trav_up_t *up_parent = flecs_trav_table_up(ctx, a, cache,
                    world, tgt, with, rel, cr_with, cr_trav);
                if (up_parent->src) {
                    up->src = up_parent->src;
                    up->tr = up_parent->tr;
                    up->id = up_parent->id;
                    goto found;
                }

                r_column = flecs_trav_type_offset_search(
                    &up_pair, table, r_column + 1, rel, &type);
            }
        }
    }

not_found:
    up->tr = NULL;
found:
    return;
}

static ecs_trav_up_t* flecs_trav_table_up(
    const ecs_query_run_ctx_t *ctx,
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache,
    const ecs_world_t *world,
    ecs_entity_t src,
    ecs_id_t with,
    ecs_id_t rel,
    ecs_component_record_t *cr_with,
    ecs_component_record_t *cr_trav)
{
    ecs_trav_up_t *up = flecs_trav_up_ensure(ctx, cache, src);
    if (up->ready) {
        return up;
    }

    flecs_trav_table_up_w(ctx, a, cache, world, src, with, rel, cr_with,
        cr_trav, up);
    up->ready = true;
    return up;
}

int flecs_query_up_reachable(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_id_t with,
    ecs_entity_t trav,
    ecs_component_record_t *cr_with,
    bool match_inherited,
    ecs_trav_up_t *out)
{
    if (trav != EcsChildOf) {
        return -1;
    }

    if (cr_with->flags & EcsIdDontFragment) {
        return -1;
    }

    if (ECS_IS_PAIR(with) && ECS_PAIR_FIRST(with) == EcsChildOf) {
        return -1;
    }

    if (match_inherited) {
        return -1;
    }

    ecs_entity_t parent;
    ecs_component_record_t *cr_rel = NULL;
    ecs_record_t *r;
    if (table->flags & EcsTableHasParent) {
        ecs_assert(row >= 0, ECS_INTERNAL_ERROR, NULL);
        const EcsParent *p = flecs_query_tree_get_parents((ecs_table_range_t){
            .table = table,
            .offset = row,
            .count = 1
        });

        parent = p->value;
        r = flecs_entities_get_any(world, parent);
    } else {
        int16_t index = table->childof_index;
        if (index == -1) {
            goto not_found;
        }

        cr_rel = table->_->records[index].hdr.cr;
        ecs_entity_index_t *entity_index = ecs_eis(world);
        r = flecs_entity_index_get_any(entity_index, 
            ECS_PAIR_SECOND(cr_rel->id));
        ecs_assert(r->dense < entity_index->alive_count, 
            ECS_INTERNAL_ERROR, NULL);
        parent = ecs_vec_get_t(&entity_index->dense, uint64_t, r->dense)[0];
    }

    ecs_table_t *parent_table = r ? r->table : NULL;
    if (parent_table) {
        const ecs_table_record_t *tr = NULL;
        if (with < FLECS_HI_COMPONENT_ID) {
            int16_t res = parent_table->component_map[with];
            if (res) {
                int32_t type_index = res > 0 ?
                    parent_table->column_map[parent_table->type.count + (res - 1)] :
                    (-res - 1);
                tr = &parent_table->_->records[type_index];
            }
        } else {
            tr = flecs_component_get_table(cr_with, parent_table);
        }

        if (tr) {
            out->src = parent;
            out->tr = ECS_CONST_CAST(ecs_table_record_t*, tr);
            out->id = with;
            return 1;
        }
    }

    if (!cr_rel) {
        cr_rel = flecs_components_get(world, ecs_childof(parent));
        if (!cr_rel) {
            goto not_found;
        }
    }

    const ecs_vec_t *ids = flecs_reachable_cache_get(world, cr_rel);
    if (!ids) {
        return -1;
    }

    const ecs_reachable_elem_t *elems = ecs_vec_first(ids);
    int32_t i, count = ecs_vec_count(ids);

    for (i = 0; i < count; i ++) {
        if (elems[i].id == with) {
            goto found;
        }
    }

not_found:
    out->src = 0;
    out->tr = NULL;
    out->id = 0;
    return 0;

found:
    out->src = elems[i].src;
    out->tr = ECS_CONST_CAST(ecs_table_record_t*, elems[i].tr);
    out->id = elems[i].id;
    return 1;
}

ecs_trav_up_t* flecs_query_get_up_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_table_t *table,
    int32_t row,
    ecs_id_t with,
    ecs_entity_t trav,
    ecs_component_record_t *cr_with,
    ecs_component_record_t *cr_trav)
{
    if (cache->with && cache->with != with) {
        flecs_query_up_cache_fini(cache);
    }

    ecs_world_t *world = ctx->it->world;
    ecs_allocator_t *a = flecs_query_get_allocator(ctx->it);
    ecs_map_init_if(&cache->src, a);

    ecs_assert(cache->dir != EcsTravDown, ECS_INTERNAL_ERROR, NULL);
    cache->dir = EcsTravUp;
    cache->with = with;

    ecs_assert(cr_with != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr_trav != NULL, ECS_INTERNAL_ERROR, NULL);

    if (trav == EcsChildOf) {
        if (table->flags & EcsTableHasParent) {
            ecs_assert(row != -1, ECS_INTERNAL_ERROR, NULL);

            const EcsParent *p = flecs_query_tree_get_parents((ecs_table_range_t){
                .table = table,
                .offset = row,
                .count = 1
            });
            ecs_assert(p != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_entity_t tgt = (uint32_t)p->value;
            ecs_trav_up_t *result = &cache->up;
            *result = (ecs_trav_up_t){0};
            flecs_trav_table_up_w(ctx, a, cache, world, tgt,
                with, ecs_pair(trav, EcsWildcard), cr_with, cr_trav, result);
            if (result->src != 0) {
                return result;
            }

            return NULL;
        }
    }

    ecs_table_record_t *tr = ecs_table_cache_get(&cr_trav->cache, table);
    if (!tr) {
        return NULL; /* Table doesn't have the relationship */
    }

    int32_t i = tr->index, remaining = tr->count;
    for (; remaining; i ++) {
        i = flecs_table_offset_search_w_inherited(
            world, table, i, cr_trav->id, NULL);
        if (i == -1) {
            break;
        }

        remaining --;

        ecs_id_t id = table->type.array[i];
        ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        ecs_trav_up_t *result = &cache->up;
        *result = (ecs_trav_up_t){0};
        flecs_trav_table_up_w(ctx, a, cache, world, tgt,
            with, ecs_pair(trav, EcsWildcard), cr_with, cr_trav, result);
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

#endif // FLECS_QUERY_PLANS
