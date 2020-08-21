#include "private_api.h"

/* Get entity */
ecs_record_t* ecs_ei_get(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    return ecs_sparse_get_sparse(
        entity_index->lo, ecs_record_t, entity);
}

/* Get or create entity */
ecs_record_t* ecs_ei_get_or_create(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    return ecs_sparse_get_or_create(
        entity_index->lo, ecs_record_t, entity);
}

/* Set entity */
ecs_record_t* ecs_ei_set(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *dst_record = ecs_sparse_get_or_create(
        entity_index->lo, ecs_record_t, entity);
    *dst_record = *record;

    /* Only return record ptrs of the sparse set, as these pointers are
        * stable. Tables store pointers to records only of they are stable */
    return dst_record;
}

/* Delete entity */
void ecs_ei_delete(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_sparse_remove(entity_index->lo, entity);
}

void ecs_ei_clear_entity(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    bool is_watched)
{
    ecs_ei_set(entity_index, entity, &(ecs_record_t){
        .table = NULL,
        .row = -is_watched
    });  
}

ecs_entity_t ecs_ei_recycle(
    ecs_ei_t *entity_index)
{
    return ecs_sparse_new_id(entity_index->lo);
}

/* Grow entity idex */
void ecs_ei_grow(
    ecs_ei_t *entity_index,
    int32_t count)
{
    ecs_sparse_grow(entity_index->lo, count);
}

/* Set size of entity index */
void ecs_ei_set_size(
    ecs_ei_t *entity_index,
    int32_t size)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_sparse_set_size(entity_index->lo, size);
}

/* Count number of entities in index */
int32_t ecs_ei_count(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_sparse_count(entity_index->lo);
}

/* Create new entity index */
void ecs_ei_new(
    ecs_world_t *world,
    ecs_ei_t *entity_index)
{
    entity_index->lo = ecs_sparse_new(ecs_record_t);
    ecs_sparse_set_id_source(entity_index->lo, &world->stats.last_id);
}

/* Clear entities from index */
void ecs_ei_clear(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_sparse_clear(entity_index->lo);
}

/* Free entity index */
void ecs_ei_free(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_sparse_free(entity_index->lo);
}

/* Copy entity index */
ecs_ei_t ecs_ei_copy(
    const ecs_ei_t *entity_index)
{
    return (ecs_ei_t){
        .lo = ecs_sparse_copy(entity_index->lo)
    };
}

/* Get memory occupied by entity index */
void ecs_ei_memory(
    ecs_ei_t *entity_index,
    int32_t *allocd,
    int32_t *used)
{
    ecs_sparse_memory(entity_index->lo, allocd, used);
}

ecs_ei_iter_t ecs_ei_iter(
    ecs_ei_t *entity_index)
{
    ecs_ei_iter_t result;
    result.index = 0;
    result.sparse_indices = ecs_sparse_ids(entity_index->lo);
    result.sparse_count = ecs_sparse_count(entity_index->lo);
    result.lo = entity_index->lo;
    return result;
}

/* Return next record for iterator (return NULL when end is reached) */
ecs_record_t *ecs_ei_next(
    ecs_ei_iter_t *iter,
    ecs_entity_t *entity_out)
{
    const uint64_t *sparse_indices = iter->sparse_indices;

    int32_t index = iter->index;
    if (iter->index < iter->sparse_count) {
        ecs_entity_t entity = (ecs_entity_t)sparse_indices[index];
        ecs_record_t *result = ecs_sparse_get_sparse(
                iter->lo, ecs_record_t, entity);
        *entity_out = entity;
        iter->index ++;
        return result;
    } else {
        return NULL;
    }
}
