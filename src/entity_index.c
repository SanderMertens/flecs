#include "flecs_private.h"

/* Get entity */
ecs_record_t* ecs_ei_get(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    if (entity > ECS_HI_ENTITY_ID) {
        if (entity == EcsSingleton) {
            return &entity_index->singleton;
        } else {
            return ecs_map_get(
                entity_index->hi, ecs_record_t, entity);
        }
    } else {
        return ecs_sparse_get_sparse(
            entity_index->lo, ecs_record_t, entity);
    }
}

/* Get or create entity */
ecs_record_t* ecs_ei_get_or_create(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    if (entity > ECS_HI_ENTITY_ID) {
        if (entity == EcsSingleton) {
            return &entity_index->singleton;
        } else {
            ecs_record_t *record =  ecs_map_get(
                entity_index->hi, ecs_record_t, entity);

            if (!record) {
                ecs_record_t new_record = { 0 };
                ecs_map_set(
                    entity_index->hi, entity, &new_record);

                record = ecs_map_get(
                    entity_index->hi, ecs_record_t, entity);  

                record->type = NULL;              
            }
            return record;
        }
    } else {
        bool is_new = false;

        ecs_record_t *record = ecs_sparse_get_or_set_sparse(
            entity_index->lo, ecs_record_t, entity, &is_new);

        if (is_new) {
            record->type = NULL;
        }
        
        return record;
    }
}

/* Set entity */
void ecs_ei_set(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (entity > ECS_HI_ENTITY_ID) {
        if (entity == EcsSingleton) {
            entity_index->singleton = *record;
        } else {
            ecs_map_set(entity_index->hi, entity, record);
        }
    } else {
        bool is_new;
        ecs_record_t *dst_record = ecs_sparse_get_or_set_sparse(
            entity_index->lo, ecs_record_t, entity, &is_new);
        *dst_record = *record;
    }
}

/* Delete entity */
void ecs_ei_delete(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (entity > ECS_HI_ENTITY_ID) {
        if (entity == EcsSingleton) {
            entity_index->singleton = (ecs_record_t){ 0 };
        } else {
            ecs_map_remove(entity_index->hi, entity);
        }
    } else {
        ecs_sparse_remove(entity_index->lo, ecs_record_t, entity);
    }
}

/* Grow entity idex */
void ecs_ei_grow(
    ecs_ei_t *entity_index,
    uint32_t count)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    uint32_t sparse_size = ecs_sparse_size(entity_index->lo);
    uint32_t to_grow = count;

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
    uint32_t size)
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
uint32_t ecs_ei_count(
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
    entity_index->singleton = (ecs_record_t){ 0 };
    entity_index->lo = ecs_sparse_new(ecs_record_t, 0);
    entity_index->hi = ecs_map_new(ecs_record_t, 0);
}

/* Clear entities from index */
void ecs_ei_clear(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    entity_index->singleton = (ecs_record_t){ 0 };
    ecs_map_clear(entity_index->hi);
    ecs_sparse_clear(entity_index->lo);
}

/* Free entity index */
void ecs_ei_free(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    
    entity_index->singleton = (ecs_record_t){ 0 };
    ecs_map_free(entity_index->hi);
    ecs_sparse_free(entity_index->lo);
}

/* Copy entity index */
ecs_ei_t ecs_ei_copy(
    const ecs_ei_t *entity_index)
{
    return (ecs_ei_t){
        .singleton = entity_index->singleton,
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
