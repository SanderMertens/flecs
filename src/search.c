#include "private_api.h"

static
int32_t type_search(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    ecs_id_t *ids,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!ECS_HAS_ROLE(id, CASE), ECS_INVALID_PARAMETER, NULL);

    ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
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
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_id_t *ids,
    int32_t count,
    ecs_id_t *id_out,
    ecs_table_record_t **tr_out)
{
    ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(offset >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!ECS_HAS_ROLE(id, CASE), ECS_INVALID_PARAMETER, NULL);

    if (!offset) {
        return type_search(world, table, id, ids, id_out, tr_out);
    }

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
    int32_t offset,
    ecs_id_t id,
    uint32_t rel,
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
        int32_t r = type_offset_search(world, table, offset, id, ids, count, 
            id_out, tr_out);
        if (r != -1) {
            return r;
        }
    }

    ecs_flags32_t flags = table->flags;
    if ((flags & EcsTableHasPairs) && max_depth && rel && id != EcsPrefab && 
        id != EcsDisabled && ECS_PAIR_RELATION(id) != ecs_id(EcsIdentifier) &&
        ECS_PAIR_RELATION(id) != EcsChildOf)
    {
        bool is_a = rel == EcsIsA;
        if (is_a && !(flags & EcsTableHasIsA)) {
            return -1;
        }

        int32_t i, r;
        for (i = 0; i < count; i ++) {
            ecs_id_t tid = ids[i];
            if (ECS_PAIR_RELATION(tid) == rel) {
                do {
                    ecs_entity_t obj = ECS_PAIR_OBJECT(tid);
                    ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

                    ecs_record_t *rec = ecs_eis_get_any(world, obj);
                    ecs_assert(rec != NULL, ECS_INTERNAL_ERROR, NULL);

                    ecs_table_t *obj_table = rec->table;
                    if (obj_table) {
                        r = type_search_relation(world, obj_table, 0, id, rel, 
                            min_depth - 1, max_depth - 1, subject_out, id_out,
                            tr_out);
                        if (r != -1) {
                            if (subject_out && !subject_out[0]) {
                                subject_out[0] = ecs_get_alive(world, obj);
                            }
                            return r;
                        }

                        if (!is_a) {
                            r = type_search_relation(world, obj_table, 0, id,
                                (uint32_t)EcsIsA, 1, INT_MAX, subject_out, id_out, 
                                tr_out);
                            if (r != -1) {
                                if (subject_out && !subject_out[0]) {
                                    subject_out[0] = ecs_get_alive(world, obj);
                                }
                                return r;
                            }
                        }
                    }

                    i ++;
                    if (i >= count) {
                        break;
                    }

                    tid = ids[i];
                } while (ECS_PAIR_RELATION(tid) == rel);

                break;
            }
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

    max_depth = INT_MAX * !max_depth + max_depth * !!max_depth;

    bool is_case = ECS_HAS_ROLE(id, CASE);
    id = is_case * (ECS_SWITCH | ECS_PAIR_RELATION(id)) + !is_case * id;
    
    return type_search_relation(world, table, offset, id, (uint32_t)rel, 
        min_depth, max_depth, subject_out, id_out, tr_out);
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

    ecs_type_t type = table->type;
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    return type_search(world, table, id, ids, id_out, NULL);
}

int32_t ecs_search_offset(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    if (!table) return -1;

    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return type_search_relation(world, table, offset, id, 0, 
        0, 0, 0, id_out, NULL);
}
