/**
 * @file search.c
 * @brief Search functions to find (component) ids in table types.
 * 
 * Search functions are used to find the column index of a (component) id in a
 * table. Additionally, search functions implement the logic for finding a
 * component id by following a relationship upwards.
 */

#include "private_api.h"

static
int32_t flecs_type_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_component_record_t *cr,
    ecs_id_t rel,
    ecs_component_record_t *cr_r,
    bool self,
    ecs_entity_t *tgt_out,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out);

static
int32_t flecs_type_search(
    const ecs_table_t *table,
    ecs_component_record_t *cr,
    ecs_id_t *ids,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_table_record_t *tr = ecs_table_cache_get(&cr->cache, table);
    if (tr) {
        int32_t r = tr->index;
        if (tr_out) tr_out[0] = tr;
        if (id_out) {
            id_out[0] = ids[r];
        }
        return r;
    }

    return -1;
}

static
int32_t flecs_type_offset_search(
    int32_t offset,
    ecs_id_t id,
    ecs_id_t *ids,
    int32_t count,
    ecs_id_t *id_out)
{
    ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(offset > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    while (offset < count) {
        ecs_id_t type_id = ids[offset ++];
        if (ecs_id_match(type_id, id)) {
            if (id_out) {
                id_out[0] = type_id;
            }
            return offset - 1;
        }
    }

    return -1;
}

bool flecs_type_can_inherit_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_component_record_t *cr,
    ecs_id_t id)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    if (cr->flags & EcsIdOnInstantiateDontInherit) {
        return false;
    }

    if (cr->flags & EcsIdExclusive) {
        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            ecs_entity_t er = ECS_PAIR_FIRST(id);
            ecs_component_record_t *cr_wc = flecs_components_get(
                world, ecs_pair(er, EcsWildcard));
            if (cr_wc && flecs_component_get_table(cr_wc, table)) {
                return false;
            }
        }
    }

    return true;
}

static
int32_t flecs_type_search_childof_for_tgt(
    const ecs_world_t *world,
    ecs_record_t *r,
    ecs_id_t id,
    ecs_component_record_t *cr,
    ecs_entity_t *tgt_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out)
{
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->flags & EcsTableHasParent, ECS_INTERNAL_ERROR, NULL);

    int32_t column = table->component_map[ecs_id(EcsParent)];
    ecs_assert(column > 0, ECS_INTERNAL_ERROR, NULL);

    EcsParent *parents = table->data.columns[column - 1].data;
    ecs_entity_t parent = parents[ECS_RECORD_TO_ROW(r->row)].value;

    return flecs_entity_search_relation(
        world, parent, id, EcsChildOf, true, cr, tgt_out, id_out, tr_out);
}

static
int32_t flecs_type_search_relation_for_tgt(
    const ecs_world_t *world,
    ecs_entity_t tgt,
    ecs_id_t id,
    ecs_component_record_t *cr,
    ecs_id_t rel,
    ecs_component_record_t *cr_r,
    ecs_entity_t *tgt_out,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_record_t *rec = flecs_entities_get_any(world, tgt);
    ecs_assert(rec != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *tgt_table = rec->table;
    ecs_assert(tgt_table != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t r = -1;

    if (rel == ecs_childof(EcsWildcard)) {
        if (tgt_table->flags & EcsTableHasParent) {
            return flecs_type_search_childof_for_tgt(
                world, rec, id, cr, tgt_out, id_out, tr_out);
        }
    }

    if (cr->flags & EcsIdDontFragment) {
        if (flecs_sparse_get(cr->sparse, 0, tgt) != NULL) {
            r = -2;
        }
    } else {
        r = flecs_type_search_relation(world, tgt_table, 0, id, cr, 
            rel, cr_r, true, tgt_out, id_out, tr_out);
    }

    if (r != -1) {
        return r;
    }

    if (rel != ecs_pair(EcsIsA, EcsWildcard)) {
        r = flecs_type_search_relation(world, tgt_table, 0, id, cr, 
            ecs_pair(EcsIsA, EcsWildcard), world->cr_isa_wildcard, 
                true, tgt_out, id_out, tr_out);
    }

    return r;
}

static
int32_t flecs_type_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_component_record_t *cr,
    ecs_id_t rel,
    ecs_component_record_t *cr_r,
    bool self,
    ecs_entity_t *tgt_out,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t count = type.count;
    bool dont_fragment = cr->flags & EcsIdDontFragment;

    if (self && !dont_fragment) {
        if (offset) {
            int32_t r = flecs_type_offset_search(offset, id, ids, count, id_out);
            if (r != -1) {
                return r;
            }
        } else {
            int32_t r = flecs_type_search(table, cr, ids, id_out, tr_out);
            if (r != -1) {
                return r;
            }
        }
    }

    ecs_flags32_t flags = table->flags;
    if (!(flags & EcsTableHasPairs) || !rel) {
        return -1;
    }

    if (rel == ecs_isa(EcsWildcard)) {
        if (!(flags & EcsTableHasIsA)) {
            return -1;
        }
        cr_r = world->cr_isa_wildcard;

        if (!flecs_type_can_inherit_id(world, table, cr, id)) {
            return -1;
        }
    }

    if (!cr_r) {
        cr_r = flecs_components_get(world, rel);
        if (!cr_r) {
            return -1;
        }
    }

    ecs_id_t id_r;
    int32_t r, r_column;
    if (offset) {
        r_column = flecs_type_offset_search(offset, rel, ids, count, &id_r);
    } else {
        r_column = flecs_type_search(table, cr_r, ids, &id_r, 0);
    }

    while (r_column != -1) {
        ecs_entity_t tgt = ECS_PAIR_SECOND(id_r);
        ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

        r = flecs_type_search_relation_for_tgt(
            world, tgt, id, cr, rel, cr_r, tgt_out, id_out, tr_out);
        if (r != -1) {
            if (tgt_out && !tgt_out[0]) {
                tgt_out[0] = ecs_get_alive(world, tgt);
            }
            return r_column;
        }

        r_column = flecs_type_offset_search(
            r_column + 1, rel, ids, count, &id_r);
    }

    return -1;
}

int32_t flecs_entity_search_relation(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_entity_t rel,
    bool self,
    ecs_component_record_t *cr,
    ecs_entity_t *tgt_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out)
{
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (self) {
        const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
        if (tr) {
            if (tgt_out) *tgt_out = entity;
            if (tr_out) *tr_out = ECS_CONST_CAST(ecs_table_record_t*, tr);
            if (id_out) *id_out = tr->hdr.cr->id;
            return tr->index;
        }
    }

    if (rel == EcsChildOf) {
        if (table->flags & EcsTableHasParent) {
            return flecs_type_search_childof_for_tgt(
                world, r, id, cr, tgt_out, id_out, tr_out);
        }
    }

    return flecs_type_search_relation(
        world, table, 0, id, cr, ecs_pair(rel, EcsWildcard), NULL, true, 
        tgt_out, id_out, tr_out);
}

int32_t ecs_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    ecs_flags64_t flags,
    ecs_entity_t *tgt_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    flags = flags ? flags : (EcsSelf|EcsUp);

    if (tgt_out) tgt_out[0] = 0;
    if (!(flags & EcsUp)) {
        return ecs_search_offset(world, table, offset, id, id_out);
    }

    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        return -1;
    }

    int32_t result = flecs_type_search_relation(world, table, offset, id, cr, 
        ecs_pair(rel, EcsWildcard), NULL, flags & EcsSelf, tgt_out, 
            id_out, tr_out);

    return result;
}

int32_t ecs_search(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        return -1;
    }

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    return flecs_type_search(table, cr, ids, id_out, 0);
}

int32_t ecs_search_offset(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    if (!offset) {
        flecs_poly_assert(world, ecs_world_t);
        return ecs_search(world, table, id, id_out);
    }

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t count = type.count;
    return flecs_type_offset_search(offset, id, ids, count, id_out);
}

static
int32_t flecs_relation_depth_walk(
    const ecs_world_t *world,
    const ecs_component_record_t *cr,
    const ecs_table_t *first,
    const ecs_table_t *table)
{
    int32_t result = 0;

    const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
    if (!tr) {
        return 0;
    }

    int32_t i = tr->index, end = i + tr->count;
    for (; i != end; i ++) {
        ecs_entity_t o = ecs_pair_second(world, table->type.array[i]);
        if (!o) {
            /* Rare, but can happen during cleanup when an intermediate table is
             * created that contains a pair that is about to be removed but 
             * hasn't yet, where the target is not alive. 
             * Would be better if this intermediate table wouldn't get created,
             * but that requires a refactor of the cleanup logic. */
            return 0;
        }

        ecs_table_t *ot = ecs_get_table(world, o);
        if (!ot) {
            continue;
        }
        
        ecs_assert(ot != first, ECS_CYCLE_DETECTED, NULL);
        int32_t cur = flecs_relation_depth_walk(world, cr, first, ot);
        if (cur > result) {
            result = cur;
        }
    }
    
    return result + 1;
}

int32_t flecs_relation_depth(
    const ecs_world_t *world,
    ecs_entity_t r,
    const ecs_table_t *table)
{
    if (r == EcsChildOf) {
        if (table->flags & EcsTableHasChildOf) {
            ecs_component_record_t *cr_wc = world->cr_childof_wildcard;
            const ecs_table_record_t *tr_wc = 
                flecs_component_get_table(cr_wc, table);
            ecs_assert(tr_wc != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_table_record_t *tr = &table->_->records[tr_wc->index];
            ecs_component_record_t *cr = tr->hdr.cr;
            ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
            return cr->pair->depth;
        } else if (table->flags & EcsTableHasParent) {
            ecs_component_record_t *cr_wc = flecs_components_get(
                world, ecs_pair(EcsParentDepth, EcsWildcard));
            ecs_assert(cr_wc != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_table_record_t *tr_wc = flecs_component_get_table(
                cr_wc, table);
            if (!tr_wc) {
                return 0;
            }

            ecs_id_t depth_pair = table->type.array[tr_wc->index];
            ecs_assert(ECS_PAIR_FIRST(depth_pair) == EcsParentDepth, 
                ECS_INTERNAL_ERROR, NULL);
            return flecs_uto(int32_t, ECS_PAIR_SECOND(depth_pair));
        } else {
            return 0;
        }
    }

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_pair(r, EcsWildcard));
    if (!cr) {
        return 0;
    }

    return flecs_relation_depth_walk(world, cr, table, table);
}
