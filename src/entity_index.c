#include "private_api.h"

/* Get entity */
ecs_record_t* ecs_ei_get(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    if (entity > ECS_HI_ENTITY_ID) {
        return ecs_map_get(
            entity_index->hi, ecs_record_t, entity);
    } else {
        return ecs_sparse_get_sparse(
            entity_index->lo, ecs_record_t, (int32_t)entity);
    }
}

/* Get or create entity */
ecs_record_t* ecs_ei_get_or_create(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    if (entity > ECS_HI_ENTITY_ID) {
        ecs_record_t *record =  ecs_map_get(
            entity_index->hi, ecs_record_t, entity);

        if (!record) {
            ecs_record_t new_record = { 0 };
            ecs_map_set(
                entity_index->hi, entity, &new_record);

            record = ecs_map_get(
                entity_index->hi, ecs_record_t, entity);  

            record->table = NULL;  
            record->row = 0;            
        }

        return record;
    } else {
        bool is_new = false;

        ecs_record_t *record = ecs_sparse_get_or_set_sparse(
            entity_index->lo, ecs_record_t, (int32_t)entity, &is_new);

        if (is_new) {
            record->table = NULL;
            record->row = 0;
        }
        
        return record;
    }
}

/* Set entity */
ecs_record_t* ecs_ei_set(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (entity > ECS_HI_ENTITY_ID) {
        ecs_map_set(entity_index->hi, entity, record);
    } else {
        bool is_new;
        ecs_record_t *dst_record = ecs_sparse_get_or_set_sparse(
            entity_index->lo, ecs_record_t, (int32_t)entity, &is_new);
        *dst_record = *record;

        /* Only return record ptrs of the sparse set, as these pointers are
         * stable. Tables store pointers to records only of they are stable */
        return dst_record;
    }

    return NULL;
}

/* Delete entity */
void ecs_ei_delete(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (entity > ECS_HI_ENTITY_ID) {
        ecs_map_remove(entity_index->hi, entity);
    } else {
        ecs_sparse_remove(entity_index->lo, ecs_record_t, (int32_t)entity);
    }
}

void ecs_ei_clear_entity(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    bool is_watched)
{
    if (entity > ECS_HI_ENTITY_ID) {
        if (!is_watched) {
            ecs_map_remove(entity_index->hi, entity);
        } else {
            ecs_ei_set(entity_index, entity, &(ecs_record_t){
                .table = NULL,
                .row = -1
            });
        }
    } else {
        ecs_ei_set(entity_index, entity, &(ecs_record_t){
            .table = NULL,
            .row = -is_watched
        });
    }    
}

ecs_entity_t ecs_ei_recycle(
    ecs_ei_t *entity_index)
{
    int32_t result;
    if (ecs_sparse_recycle(entity_index->lo, ecs_record_t, &result)) {
        ecs_assert(result > 0, ECS_INTERNAL_ERROR, NULL);
        return (ecs_entity_t)result; // implicit upcast
    } else {
        return 0;
    }
}

/* Grow entity idex */
void ecs_ei_grow(
    ecs_ei_t *entity_index,
    int32_t count)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t sparse_size = ecs_sparse_size(entity_index->lo);
    int32_t to_grow = count;

    if (sparse_size < ECS_HI_ENTITY_ID) {
        if (to_grow + sparse_size > ECS_HI_ENTITY_ID) {
            to_grow -= ECS_HI_ENTITY_ID - sparse_size;
        }

        ecs_sparse_grow(entity_index->lo, to_grow);

        count -= to_grow;
    }

    if (count) {   
        ecs_map_grow(entity_index->hi, count);
    }
}

/* Set size of entity index */
void ecs_ei_set_size(
    ecs_ei_t *entity_index,
    int32_t size)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (size > ECS_HI_ENTITY_ID) {
        ecs_sparse_set_size(entity_index->lo, ECS_HI_ENTITY_ID);
        ecs_map_set_size(entity_index->hi, size - ECS_HI_ENTITY_ID);
    } else {
        ecs_sparse_set_size(entity_index->lo, size);
        ecs_map_set_size(entity_index->hi, 0);
    }
}

/* Count number of entities in index */
int32_t ecs_ei_count(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    return ecs_map_count(entity_index->hi) + 
        ecs_sparse_count(entity_index->lo);
}

/* Create new entity index */
void ecs_ei_new(
    ecs_ei_t *entity_index)
{
    entity_index->lo = ecs_sparse_new(ecs_record_t, 0);
    entity_index->hi = ecs_map_new(ecs_record_t, 0);
}

/* Clear entities from index */
void ecs_ei_clear(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_clear(entity_index->hi);
    ecs_sparse_clear(entity_index->lo);
}

/* Free entity index */
void ecs_ei_free(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_map_free(entity_index->hi);
    ecs_sparse_free(entity_index->lo);
}

/* Copy entity index */
ecs_ei_t ecs_ei_copy(
    const ecs_ei_t *entity_index)
{
    return (ecs_ei_t){
        .hi = ecs_map_copy(entity_index->hi),
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
    ecs_map_memory(entity_index->hi, allocd, used);
}

ecs_ei_iter_t ecs_ei_iter(
    ecs_ei_t *entity_index)
{
    ecs_ei_iter_t result;
    result.index = 0;
    result.sparse_indices = ecs_sparse_indices(entity_index->lo);
    result.sparse_count = ecs_sparse_count(entity_index->lo);
    result.map_iter = ecs_map_iter(entity_index->hi);
    result.lo = entity_index->lo;
    return result;
}

/* Return next record for iterator (return NULL when end is reached) */
ecs_record_t *ecs_ei_next(
    ecs_ei_iter_t *iter,
    ecs_entity_t *entity_out)
{
    const int32_t *sparse_indices = iter->sparse_indices;

    if (sparse_indices) {
        int32_t index = iter->index;
        if (iter->index < iter->sparse_count) {
            ecs_entity_t entity = (ecs_entity_t)sparse_indices[index];
            ecs_record_t *result = ecs_sparse_get_sparse(
                    iter->lo, ecs_record_t, (int32_t)entity);
            *entity_out = entity;
            iter->index ++;
            return result;
        } else {
            iter->sparse_indices = NULL;
        }
    }

    return ecs_map_next(&iter->map_iter, ecs_record_t, entity_out);
}
