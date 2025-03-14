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
int32_t flecs_type_search(
    const ecs_table_t *table,
    ecs_component_record_t *cdr,
    ecs_id_t *ids,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_table_record_t *tr = ecs_table_cache_get(&cdr->cache, table);
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
    const ecs_component_record_t *cdr,
    ecs_id_t id)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    if (cdr->flags & EcsIdOnInstantiateDontInherit) {
        return false;
    }

    if (cdr->flags & EcsIdExclusive) {
        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            ecs_entity_t er = ECS_PAIR_FIRST(id);
            ecs_component_record_t *cdr_wc = flecs_components_get(
                world, ecs_pair(er, EcsWildcard));
            if (cdr_wc && flecs_component_get_table(cdr_wc, table)) {
                return false;
            }
        }
    }

    return true;
}

static
int32_t flecs_type_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_component_record_t *cdr,
    ecs_id_t rel,
    ecs_component_record_t *idr_r,
    bool self,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t count = type.count;

    if (self) {
        if (offset) {
            int32_t r = flecs_type_offset_search(offset, id, ids, count, id_out);
            if (r != -1) {
                return r;
            }
        } else {
            int32_t r = flecs_type_search(table, cdr, ids, id_out, tr_out);
            if (r != -1) {
                return r;
            }
        }
    }

    ecs_flags32_t flags = table->flags;
    if ((flags & EcsTableHasPairs) && rel) {
        bool is_a = rel == ecs_pair(EcsIsA, EcsWildcard);
        if (is_a) {
            if (!(flags & EcsTableHasIsA)) {
                return -1;
            }
            idr_r = world->idr_isa_wildcard;

            if (!flecs_type_can_inherit_id(world, table, cdr, id)) {
                return -1;
            }
        }

        if (!idr_r) {
            idr_r = flecs_components_get(world, rel);
            if (!idr_r) {
                return -1;
            }
        }

        ecs_id_t id_r;
        int32_t r, r_column;
        if (offset) {
            r_column = flecs_type_offset_search(offset, rel, ids, count, &id_r);
        } else {
            r_column = flecs_type_search(table, idr_r, ids, &id_r, 0);
        }
        while (r_column != -1) {
            ecs_entity_t obj = ECS_PAIR_SECOND(id_r);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_record_t *rec = flecs_entities_get_any(world, obj);
            ecs_assert(rec != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_table_t *tgt_table = rec->table;
            ecs_assert(tgt_table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(tgt_table != table, ECS_CYCLE_DETECTED, NULL);
            
            r = flecs_type_search_relation(world, tgt_table, 0, id, cdr, 
                rel, idr_r, true, subject_out, id_out, tr_out);
            if (r != -1) {
                if (subject_out && !subject_out[0]) {
                    subject_out[0] = ecs_get_alive(world, obj);
                }
                return r_column;
            }

            if (!is_a) {
                r = flecs_type_search_relation(world, tgt_table, 0, id, cdr, 
                    ecs_pair(EcsIsA, EcsWildcard), world->idr_isa_wildcard, 
                        true, subject_out, id_out, tr_out);
                if (r != -1) {
                    if (subject_out && !subject_out[0]) {
                        subject_out[0] = ecs_get_alive(world, obj);
                    }
                    return r_column;
                }
            }

            r_column = flecs_type_offset_search(
                r_column + 1, rel, ids, count, &id_r);
        }
    }

    return -1;
}

int32_t flecs_search_relation_w_idr(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    ecs_flags64_t flags,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out,
    ecs_component_record_t *cdr)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    flags = flags ? flags : (EcsSelf|EcsUp);

    if (!cdr) {
        cdr = flecs_components_get(world, id);
        if (!cdr) {
            return -1;
        }
    }

    if (subject_out) subject_out[0] = 0;
    if (!(flags & EcsUp)) {
        if (offset) {
            return ecs_search_offset(world, table, offset, id, id_out);
        } else {
            return flecs_type_search(
                table, cdr, table->type.array, id_out, tr_out);
        }
    }

    int32_t result = flecs_type_search_relation(world, table, offset, id, cdr, 
        ecs_pair(rel, EcsWildcard), NULL, flags & EcsSelf, subject_out, 
            id_out, tr_out);

    return result;
}

int32_t ecs_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    ecs_flags64_t flags,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    flags = flags ? flags : (EcsSelf|EcsUp);

    if (subject_out) subject_out[0] = 0;
    if (!(flags & EcsUp)) {
        return ecs_search_offset(world, table, offset, id, id_out);
    }

    ecs_component_record_t *cdr = flecs_components_get(world, id);
    if (!cdr) {
        return -1;
    }

    int32_t result = flecs_type_search_relation(world, table, offset, id, cdr, 
        ecs_pair(rel, EcsWildcard), NULL, flags & EcsSelf, subject_out, 
            id_out, tr_out);

    return result;
}

int32_t flecs_search_w_idr(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t *id_out,
    ecs_component_record_t *cdr)
{
    flecs_poly_assert(world, ecs_world_t);
    (void)world;

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    return flecs_type_search(table, cdr, ids, id_out, 0);
}

int32_t ecs_search(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_component_record_t *cdr = flecs_components_get(world, id);
    if (!cdr) {
        return -1;
    }

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    return flecs_type_search(table, cdr, ids, id_out, 0);
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
    const ecs_component_record_t *cdr,
    const ecs_table_t *first,
    const ecs_table_t *table)
{
    int32_t result = 0;

    const ecs_table_record_t *tr = flecs_component_get_table(cdr, table);
    if (!tr) {
        return 0;
    }

    int32_t i = tr->index, end = i + tr->count;
    for (; i != end; i ++) {
        ecs_entity_t o = ecs_pair_second(world, table->type.array[i]);
        ecs_assert(o != 0, ECS_INTERNAL_ERROR, NULL);

        ecs_table_t *ot = ecs_get_table(world, o);
        if (!ot) {
            continue;
        }
        
        ecs_assert(ot != first, ECS_CYCLE_DETECTED, NULL);
        int32_t cur = flecs_relation_depth_walk(world, cdr, first, ot);
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
    ecs_component_record_t *cdr = flecs_components_get(world, ecs_pair(r, EcsWildcard));
    if (!cdr) {
        return 0;
    }

    return flecs_relation_depth_walk(world, cdr, table, table);
}
