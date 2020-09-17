#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ecs_record_t {
    ecs_table_t *table;  /* Identifies a type (and table) in world */
    int32_t row;         /* Table row of the entity */
};

#define ecs_eis_get(stage, entity) ecs_sparse_get_sparse((stage)->entity_index, ecs_record_t, entity)
#define ecs_eis_set(stage, entity, ...) (ecs_sparse_set((stage)->entity_index, ecs_record_t, entity, (__VA_ARGS__)))
#define ecs_eis_get_or_create(stage, entity) ecs_sparse_get_or_create((stage)->entity_index, ecs_record_t, entity)
#define ecs_eis_delete(stage, entity) ecs_sparse_remove((stage)->entity_index, entity)
#define ecs_eis_recycle(stage) ecs_sparse_new_id((stage)->entity_index)
#define ecs_eis_clear_entity(stage, entity, is_watched) ecs_eis_set(stage, entity, &(ecs_record_t){NULL, is_watched})
#define ecs_eis_grow(stage, count) ecs_sparse_grow((stage)->entity_index, count)
#define ecs_eis_set_size(stage, size) ecs_sparse_set_size((stage)->entity_index, size)
#define ecs_eis_count(stage) ecs_sparse_count((stage)->entity_index)
#define ecs_eis_clear(stage) ecs_sparse_clear((stage)->entity_index)
#define ecs_eis_copy(stage) ecs_sparse_copy((stage)->entity_index)
#define ecs_eis_free(stage) ecs_sparse_free((stage)->entity_index)
#define ecs_eis_memory(stage, allocd, used) ecs_sparse_memory((stage)->entity_index, allocd, used)

#ifdef __cplusplus
}
#endif

#endif
