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

        ecs_vector_each(entities, ecs_entity_t, e_ptr, {
            ecs_eis_delete(&world->stage, *e_ptr);
        })

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
    ecs_table_t *src_table,
    ecs_entities_t *to_add,
    ecs_entities_t *to_remove)
{
    if (!dst_table->type) {
        /* If this removes all components, clear table */
        ecs_table_clear(world, src_table);
    } else {
        /* Merge table into dst_table */
        if (dst_table != src_table) {
            ecs_data_t *src_data = ecs_table_get_data(world, src_table);
            int32_t dst_count = ecs_table_count(dst_table);
            int32_t src_count = ecs_table_count(src_table);

            if (to_remove && to_remove->count && src_data) {
                ecs_run_deinit_actions(world, &world->stage, src_table, src_data, 
                    0, src_count, *to_remove, true);
            }

            ecs_table_merge(world, dst_table, src_table);
            ecs_data_t *dst_data = ecs_table_get_data(world, dst_table);

            if (to_add && to_add->count && dst_data) {
                ecs_run_init_actions(world, &world->stage, dst_table, dst_data, 
                    dst_count, src_count, *to_add, (ecs_comp_mask_t){ 0 }, true);
            }
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

void ecs_bulk_add_remove_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    ecs_entities_t to_add_array = ecs_type_to_entities(to_add);
    ecs_entities_t to_remove_array = ecs_type_to_entities(to_remove);

    ecs_entities_t added = {
        .array = ecs_os_alloca(sizeof(ecs_entity_t) * to_add_array.count),
        .count = 0
    }; 

    ecs_entities_t removed = {
        .array = ecs_os_alloca(sizeof(ecs_entity_t) * to_remove_array.count),
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }

        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, &removed);
        
        dst_table = ecs_table_traverse_add(
            world, stage, dst_table, &to_add_array, &added);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);

        if (table == dst_table || (!added.count && !removed.count)) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   

        merge_table(world, dst_table, table, &added, &removed);
        added.count = 0;
        removed.count = 0;
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

    ecs_entities_t to_add_array = ecs_type_to_entities(to_add);
    ecs_entities_t added = {
        .array = ecs_os_alloca(sizeof(ecs_entity_t) * to_add_array.count),
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = ecs_table_traverse_add(
            world, stage, table, &to_add_array, &added);
        
        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!added.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table, &added, NULL);
        added.count = 0;
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

    ecs_entities_t to_add_array = { .array = &to_add, .count = 1 };

    ecs_entity_t added_entity;
    ecs_entities_t added = {
        .array = &added_entity,
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = ecs_table_traverse_add(
            world, stage, table, &to_add_array, &added);

        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);
        
        if (!added.count) {
            continue;
        }         

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table, &added, NULL);
        added.count = 0;
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

    ecs_entities_t to_remove_array = ecs_type_to_entities(to_remove);
    ecs_entities_t removed = {
        .array = ecs_os_alloca(sizeof(ecs_entity_t) * to_remove_array.count),
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, &removed);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!removed.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table, NULL, &removed);
        removed.count = 0;        
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

    ecs_entities_t to_remove_array = { .array = &to_remove, .count = 1 };

    ecs_entity_t removed_entity;
    ecs_entities_t removed = {
        .array = &removed_entity,
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }            

        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, &removed);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!removed.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table, NULL, &removed);
        removed.count = 0;        
    }    
}
