#include "private_api.h"

static
int32_t type_search(
    const ecs_table_t *table,
    ecs_id_record_t *idr,
    ecs_id_t *ids,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{    
    ecs_table_record_t *tr = ecs_table_cache_get(
        &idr->cache, ecs_table_record_t, table);

    if (tr) {
        int32_t r = tr->column;
        if (tr_out) tr_out[0] = tr;
        if (id_out) id_out[0] = ids[r];
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
    ecs_assert(!ECS_HAS_ROLE(id, CASE), ECS_INVALID_PARAMETER, NULL);

    while (offset < count) {
        ecs_id_t type_id = ids[offset ++];
        if (ecs_id_match(type_id, id)) {
            if (id_out) id_out[0] = type_id;
            return offset - 1;
        }
    }

    return -1;
}

static
int32_t type_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    ecs_id_record_t *idr,
    ecs_id_t rel,
    ecs_id_record_t *idr_r,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_type_t type = table->type;
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t count = ecs_vector_count(type);

    if (min_depth <= 0) {
        int32_t r = type_search(table, idr, ids, id_out, tr_out);
        if (r != -1) {
            return r;
        }
    }

    ecs_flags32_t flags = table->flags;
    if ((flags & EcsTableHasPairs) && max_depth && rel) {
        bool is_a = rel == ecs_pair(EcsIsA, EcsWildcard);
        if (is_a) {
            if (!(flags & EcsTableHasIsA)) {
                return -1;
            }
            if (idr->flags & ECS_ID_DONT_INHERIT) {
                return -1;
            }
            idr_r = world->idr_isa_wildcard;
        }

        if (!idr_r) {
            idr_r = flecs_get_id_record(world, rel);
            if (!idr_r) {
                return -1;
            }
        }

        ecs_id_t id_r;
        ecs_table_record_t *tr_r;
        int32_t r, r_column = type_search(table, idr_r, ids, &id_r, &tr_r);
        while (r_column != -1) {
            ecs_entity_t obj = ECS_PAIR_OBJECT(id_r);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_record_t *rec = ecs_eis_get_any(world, obj);
            ecs_assert(rec != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_table_t *obj_table = rec->table;
            if (obj_table) {
                r = type_search_relation(world, obj_table, id, idr, 
                    rel, idr_r, min_depth - 1, max_depth - 1, subject_out, 
                    id_out, tr_out);
                if (r != -1) {
                    if (subject_out && !subject_out[0]) {
                        subject_out[0] = ecs_get_alive(world, obj);
                    }
                    return r;
                }

                if (!is_a) {
                    r = type_search_relation(world, obj_table, id, idr, 
                        ecs_pair(EcsIsA, EcsWildcard), world->idr_isa_wildcard, 
                            1, INT_MAX, subject_out, id_out, tr_out);
                    if (r != -1) {
                        if (subject_out && !subject_out[0]) {
                            subject_out[0] = ecs_get_alive(world, obj);
                        }
                        return r;
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
    struct ecs_table_record_t **tr_out)
{
    if (!table) return -1;

    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    bool is_case = ECS_HAS_ROLE(id, CASE);
    id = is_case * (ECS_SWITCH | ECS_PAIR_RELATION(id)) + !is_case * id;
    
    if (offset) {
        ecs_assert(min_depth == 0, ECS_INVALID_PARAMETER, NULL);
        ecs_type_t type = table->type;
        ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
        int32_t count = ecs_vector_count(type);
        return type_offset_search(offset, id, ids, count, id_out);
    }

    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return -1;
    }

    max_depth = INT_MAX * !max_depth + max_depth * !!max_depth;

    return type_search_relation(world, table, id, idr, 
        ecs_pair(rel, EcsWildcard), NULL, min_depth, max_depth, subject_out, 
            id_out, tr_out);
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

    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return -1;
    }

    ecs_type_t type = table->type;
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
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
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t count = ecs_vector_count(type);
    return type_offset_search(offset, id, ids, count, id_out);
}
