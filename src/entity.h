/**
 * @file entity.h
 * @brief Internal functions for dealing with entities.
 */

#ifndef FLECS_ENTITY_H
#define FLECS_ENTITY_H

#define ecs_get_low_id(table, r, id)\
    ecs_assert(table->component_map != NULL, ECS_INTERNAL_ERROR, NULL);\
    int16_t column_index = table->component_map[id];\
    if (column_index > 0) {\
        ecs_column_t *column = &table->data.columns[column_index - 1];\
        return ECS_ELEM(column->data, column->ti->size, \
            ECS_RECORD_TO_ROW(r->row));\
    }

typedef struct {
    const ecs_type_info_t *ti;
    void *ptr;
} flecs_component_ptr_t;

flecs_component_ptr_t flecs_ensure(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    ecs_record_t *r,
    ecs_size_t size);

flecs_component_ptr_t flecs_get_mut(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t id,
    ecs_record_t *r,
    ecs_size_t size);

/* Get component pointer with type info. */
flecs_component_ptr_t flecs_get_component_ptr(
    const ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_component_record_t *cr);

/* Get component pointer. */
void* flecs_get_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_component_record_t *cr);

/* Create new entity. */
ecs_entity_t flecs_new_id(
    const ecs_world_t *world);

/* Create new entities in bulk. */
const ecs_entity_t* flecs_bulk_new(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_entity_t *entities,
    ecs_type_t *component_ids,
    int32_t count,
    void **c_info,
    bool move,
    int32_t *row_out,
    ecs_table_diff_t *diff);

/* Add new entity id to root table. */
void flecs_add_to_root_table(
    ecs_world_t *world,
    ecs_entity_t e);

/* Mark an entity as being watched. This is used to trigger automatic rematching
 * when entities used in system expressions change their components. */
void flecs_add_flag(
    ecs_world_t *world,
    ecs_entity_t entity,
    uint32_t flag);

/* Same as flecs_add_flag but for ecs_record_t. */
void flecs_record_add_flag(
    ecs_record_t *record,
    uint32_t flag);

/* Get entity that should be used for OneOf constraint from relationship. */
ecs_entity_t flecs_get_oneof(
    const ecs_world_t *world,
    ecs_entity_t e);

/* Compute relationship depth for table */
int32_t flecs_relation_depth(
    const ecs_world_t *world,
    ecs_entity_t r,
    const ecs_table_t *table);

/* Get component from base entity (follows IsA relationship) */
void* flecs_get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_component_record_t *table_index,
    int32_t recur_depth);

/* Commit entity to (new) table */
void flecs_commit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *dst_table,   
    ecs_table_diff_t *diff,
    bool construct,
    ecs_flags32_t evt_flags);

/* Add multiple component ids to entity */
void flecs_add_ids(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t *ids,
    int32_t count);

/* Like regular modified, but doesn't assert if entity doesn't have component */
void flecs_modified_id_if(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool invoke_hook);

/* Like regular set, but uses move instead of copy */
void flecs_set_id_move(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    void *ptr,
    ecs_cmd_kind_t cmd_kind);

/* Add single component id */
void flecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/* Remove single component id */
void flecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/* Run on delete action */
void flecs_on_delete(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action,
    bool delete_id,
    bool force_delete);

/* Remove non-fragmenting components from entity */
void flecs_entity_remove_non_fragmenting(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_record_t *r);

const char* flecs_entity_invalid_reason(
    const ecs_world_t *world,
    ecs_entity_t entity);

ecs_table_range_t flecs_range_from_entity(
    const ecs_world_t *world,
    ecs_entity_t e);

ecs_entity_t flecs_set_identifier(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t tag,
    const char *name);

#endif
