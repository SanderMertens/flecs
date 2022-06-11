#include "private_api.h"

static
int32_t type_search(
    const ecs_table_t *table,
    ecs_id_record_t *idr,
    ecs_id_t *ids,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{    
    ecs_table_record_t *tr = ecs_table_cache_get(&idr->cache, table);
    if (tr) {
        int32_t r = tr->column;
        if (tr_out) tr_out[0] = tr;
        if (id_out) {
            id_out[0] = flecs_to_public_id(ids[r]);
        }
        return r;
    }

    return -1;
}

static
int32_t type_offset_search(
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
                id_out[0] = flecs_to_public_id(type_id);
            }
            return offset - 1;
        }
    }

    return -1;
}

static
bool type_can_inherit_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_id_record_t *idr,
    ecs_id_t id)
{
    if (idr->flags & EcsIdDontInherit) {
        return false;
    }
    if (idr->flags & EcsIdExclusive) {
        if (ECS_HAS_ROLE(id, PAIR)) {
            ecs_entity_t er = ECS_PAIR_FIRST(id);
            if (flecs_table_record_get(
                world, table, ecs_pair(er, EcsWildcard))) 
            {
                return false;
            }
        }
    }
    return true;
}

static
int32_t type_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_id_record_t *idr,
    ecs_id_t rel,
    ecs_id_record_t *idr_r,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    int32_t *depth_out,
    ecs_table_record_t **tr_out)
{
    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t count = type.count;

    if (min_depth <= 0) {
        if (offset) {
            int32_t r = type_offset_search(offset, id, ids, count, id_out);
            if (r != -1) {
                return r;
            }
        } else {
            int32_t r = type_search(table, idr, ids, id_out, tr_out);
            if (r != -1) {
                return r;
            }
        }
    }

    ecs_flags32_t flags = table->flags;
    if ((flags & EcsTableHasPairs) && max_depth && rel) {
        bool is_a = rel == ecs_pair(EcsIsA, EcsWildcard);
        if (is_a) {
            if (!(flags & EcsTableHasIsA)) {
                return -1;
            }
            if (!type_can_inherit_id(world, table, idr, id)) {
                return -1;
            }
            idr_r = world->idr_isa_wildcard;
        }

        if (!idr_r) {
            idr_r = flecs_id_record_get(world, rel);
            if (!idr_r) {
                return -1;
            }
        }

        ecs_id_t id_r;
        int32_t r, r_column;
        if (offset) {
            r_column = type_offset_search(offset, rel, ids, count, &id_r);
        } else {
            r_column = type_search(table, idr_r, ids, &id_r, 0);
        }
        while (r_column != -1) {
            ecs_entity_t obj = ECS_PAIR_SECOND(id_r);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_record_t *rec = flecs_entities_get_any(world, obj);
            ecs_assert(rec != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_table_t *obj_table = rec->table;
            if (obj_table) {
                ecs_assert(obj_table != table, ECS_CYCLE_DETECTED, NULL);
                
                r = type_search_relation(world, obj_table, 0, id, idr, 
                    rel, idr_r, min_depth - 1, max_depth - 1, subject_out, 
                    id_out, depth_out, tr_out);
                if (r != -1) {
                    if (subject_out && !subject_out[0]) {
                        subject_out[0] = ecs_get_alive(world, obj);
                    }
                    if (depth_out) {
                        depth_out[0] ++;
                    }
                    return r_column;
                }

                if (!is_a) {
                    r = type_search_relation(world, obj_table, 0, id, idr, 
                        ecs_pair(EcsIsA, EcsWildcard), world->idr_isa_wildcard, 
                            1, INT_MAX, subject_out, id_out, depth_out, tr_out);
                    if (r != -1) {
                        if (subject_out && !subject_out[0]) {
                            subject_out[0] = ecs_get_alive(world, obj);
                        }
                        if (depth_out) {
                            depth_out[0] ++;
                        }
                        return r_column;
                    }
                }
            }

            r_column = type_offset_search(r_column + 1, rel, ids, count, &id_r);
        }
    }

    return -1;
}

int32_t ecs_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    int32_t *depth_out,
    struct ecs_table_record_t **tr_out)
{
    if (!table) return -1;

    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_record_t *idr = flecs_query_id_record_get(world, id);
    if (!idr) {
        return -1;
    }

    max_depth = INT_MAX * !max_depth + max_depth * !!max_depth;

    int32_t result = type_search_relation(world, table, offset, id, idr, 
        ecs_pair(rel, EcsWildcard), NULL, min_depth, max_depth, subject_out, 
            id_out, depth_out, tr_out);

    return result;
}

int32_t ecs_search(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    if (!table) return -1;

    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_record_t *idr = flecs_query_id_record_get(world, id);
    if (!idr) {
        return -1;
    }

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    return type_search(table, idr, ids, id_out, NULL);
}

int32_t ecs_search_offset(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    if (!offset) {
        return ecs_search(world, table, id, id_out);
    }

    if (!table) return -1;

    ecs_poly_assert(world, ecs_world_t);

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t count = type.count;
    return type_offset_search(offset, id, ids, count, id_out);
}

static
int32_t flecs_relation_depth_walk(
    const ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_table_t *first,
    ecs_table_t *table)
{
    int32_t result = 0;

    const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (!tr) {
        return 0;
    }

    int32_t i = tr->column, end = i + tr->count;
    for (; i != end; i ++) {
        ecs_entity_t o = ecs_pair_second(world, table->type.array[i]);
        ecs_assert(o != 0, ECS_INTERNAL_ERROR, NULL);

        ecs_table_t *ot = ecs_get_table(world, o);
        if (!ot) {
            continue;
        }
        
        ecs_assert(ot != first, ECS_CYCLE_DETECTED, NULL);
        int32_t cur = flecs_relation_depth_walk(world, idr, first, ot);
        if (cur > result) {
            result = cur;
        }
    }
    
    return result + 1;
}

int32_t flecs_relation_depth(
    const ecs_world_t *world,
    ecs_entity_t r,
    ecs_table_t *table)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, ecs_pair(r, EcsWildcard));
    if (!idr) {
        return 0;
    }
    return flecs_relation_depth_walk(world, idr, table, table);
}
