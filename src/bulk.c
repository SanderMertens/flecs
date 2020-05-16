#include "flecs_private.h"

static
void bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    bool is_delete)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, 
        "delete_w_filter currently only supported on main stage");

    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }

        /* Remove entities from index */
        ecs_data_t *data = ecs_table_get_data(world, table);
        if (!data) {
            /* If table has no data, there's nothing to delete */
            continue;
        }

        ecs_vector_t *entities = NULL;
        if (data) {
            entities = data->entities;
        }

        ecs_entity_t *array = ecs_vector_first(entities);
        int32_t j, row_count = ecs_vector_count(entities);

        for (j = 0; j < row_count; j ++) {
            ecs_eis_delete(&world->stage, array[j]);
        }

        /* Both filters passed, clear table */
        if (is_delete) {
            ecs_table_clear(world, table);
        } else {
            ecs_table_clear_silent(world, table);
        }
    }
}

static
void merge_table(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table)
{
    if (!dst_table->type) {
        /* If this removes all components, clear table */
        ecs_table_clear(world, src_table);
    } else {
        /* Merge table into dst_table */
        if (dst_table != src_table) {
            ecs_table_merge(world, dst_table, src_table);
        }
    }
}

/* -- Public API -- */

void ecs_bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    bulk_delete(world, filter, true);
}

void ecs_clear_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    bulk_delete(world, filter, false);
}

void ecs_bulk_add_remove_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_type_t type = table->type;

        /* Skip if the type contains none of the components in to_remove */
        if (to_remove) {
            if (!ecs_type_contains(world, type, to_remove, false, false)) {
                continue;
            }
        }

        /* Skip if the type already contains all of the components in to_add */
        if (to_add) {
            if (ecs_type_contains(world, type, to_add, true, false)) {
                continue;
            }            
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }

        ecs_entities_t to_add_array = {
            .array = ecs_vector_first(to_add),
            .count = ecs_vector_count(to_add)
        };

        ecs_entities_t to_remove_array = {
            .array = ecs_vector_first(to_remove),
            .count = ecs_vector_count(to_remove)
        };

        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, NULL);
        
        dst_table = ecs_table_traverse_add(
            world, stage, dst_table, &to_add_array, NULL);

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   

        merge_table(world, dst_table, table);
    }    
}

void ecs_bulk_add_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (ecs_type_contains(world, table->type, to_add, true, false) ||
            !ecs_table_match_filter(world, table, filter)) 
        {
            continue;
        }

        ecs_entities_t to_add_components = {
            .array = ecs_vector_first(to_add),
            .count = ecs_vector_count(to_add)
        };
        
        ecs_table_t *dst_table = ecs_table_traverse_add(
            world, stage, table, &to_add_components, NULL);

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table);
    }    
}

void ecs_bulk_add_entity(
    ecs_world_t *world,
    ecs_entity_t to_add,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (ecs_type_has_entity(world, table->type, to_add) || 
            !ecs_table_match_filter(world, table, filter)) 
        {
            continue;
        }            

        ecs_entities_t to_add_array = { .array = &to_add, .count = 1 };
        ecs_table_t *dst_table = ecs_table_traverse_add(
            world, stage, table, &to_add_array, NULL);

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table);
    }    
}

void ecs_bulk_remove_type(
    ecs_world_t *world,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (!ecs_type_contains(world, table->type, to_remove, true, false) ||
            !ecs_table_match_filter(world, table, filter)) 
        {
            continue;
        }

        ecs_entities_t to_remove_array = {
            .array = ecs_vector_first(to_remove),
            .count = ecs_vector_count(to_remove)
        };
        
        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, NULL);

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table);
    }    
}

void ecs_bulk_remove_entity(
    ecs_world_t *world,
    ecs_entity_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        if (!ecs_type_has_entity(world, table->type, to_remove) || 
            !ecs_table_match_filter(world, table, filter)) 
        {
            continue;
        }            

        ecs_entities_t to_remove_array = { .array = &to_remove, .count = 1 };
        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, NULL);

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table);
    }    
}