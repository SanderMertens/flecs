#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_ei_t ecs_ei_t;

struct ecs_record_t {
    ecs_table_t *table;            /* Identifies a type (and table) in world */
    int32_t row;                  /* Table row of the entity */
};

typedef struct ecs_ei_iter_t {
    int32_t index;
    const int32_t *sparse_indices;
    int32_t sparse_count;
    ecs_map_iter_t map_iter;
    ecs_sparse_t *lo;
} ecs_ei_iter_t;

/* Get entity record */
ecs_record_t* ecs_ei_get(
    ecs_ei_t *entity_index,
    ecs_entity_t entity);

/* Set entity */
ecs_record_t* ecs_ei_set(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    ecs_record_t *record);

/* Get or set entity */
ecs_record_t* ecs_ei_get_or_create(
    ecs_ei_t *entity_index,
    ecs_entity_t entity);

/* Delete entity from stage */
void ecs_ei_clear_entity(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    bool is_watched);

/* Delete entity from stage */
void ecs_ei_delete(
    ecs_ei_t *entity_index,
    ecs_entity_t entity);

/* Recycle deleted entity id (returns 0 if no available) */
ecs_entity_t ecs_ei_recycle(
    ecs_ei_t *entity_index);

/* Grow entity index */
void ecs_ei_grow(
    ecs_ei_t *entity_index,
    int32_t count);

/* Grow entity index to specific size */
void ecs_ei_set_size(
    ecs_ei_t *entity_index,
    int32_t size);    

/* Count entities in stage */
int32_t ecs_ei_count(
    ecs_ei_t *entity_index);      

/* Initialize entity index for stage */
void ecs_ei_new(
    ecs_ei_t *entity_index);

/* Clear all entities from a stage */
void ecs_ei_clear(
    ecs_ei_t *entity_index);

/* Clear all entities from a stage */
ecs_ei_t ecs_ei_copy(
    const ecs_ei_t *entity_index);    

/* Free entity index for stage */
void ecs_ei_free(
    ecs_ei_t *entity_index);

void ecs_ei_memory(
    ecs_ei_t *entity_index,
    int32_t *allocd,
    int32_t *used);

/* Create iterator for entity index */
ecs_ei_iter_t ecs_ei_iter(
    ecs_ei_t *entity_index);

/* Return next record for iterator (return NULL when end is reached) */
ecs_record_t *ecs_ei_next(
    ecs_ei_iter_t *iter,
    ecs_entity_t *entity_out);

// Convenience macro's for directly calling operations for stage
#define ecs_eis_get(stage, entity) ecs_ei_get(&(stage)->entity_index, entity)
#define ecs_eis_set(stage, entity, ...) ecs_ei_set(&(stage)->entity_index, entity, __VA_ARGS__)
#define ecs_eis_get_or_create(stage, entity) ecs_ei_get_or_create(&(stage)->entity_index, entity)
#define ecs_eis_delete(stage, entity) ecs_ei_delete(&(stage)->entity_index, entity)
#define ecs_eis_recycle(stage) ecs_ei_recycle(&(stage)->entity_index)
#define ecs_eis_clear_entity(stage, entity, is_watched) ecs_ei_clear_entity(&(stage)->entity_index, entity, is_watched)
#define ecs_eis_grow(stage, count) ecs_ei_grow(&(stage)->entity_index, count)
#define ecs_eis_set_size(stage, size) ecs_ei_set_size(&(stage)->entity_index, size)
#define ecs_eis_count(stage) ecs_ei_count(&(stage)->entity_index)
#define ecs_eis_new(stage) ecs_ei_new(&(stage)->entity_index)
#define ecs_eis_clear(stage) ecs_ei_clear(&(stage)->entity_index)
#define ecs_eis_copy(stage) ecs_ei_copy(&(stage)->entity_index)
#define ecs_eis_free(stage) ecs_ei_free(&(stage)->entity_index)
#define ecs_eis_memory(stage, allocd, used) ecs_ei_memory(&(stage)->entity_index, allocd, used)

#ifdef __cplusplus
}
#endif

#endif
