#include "include/private/flecs.h"

static
ecs_entity_t get_prefab_parent_flag(
    ecs_world_t *world,
    ecs_entity_t prefab)
{
    ecs_entity_t flag = ecs_map_get64(world->prefab_parent_index, prefab);
    if (!flag) {
        flag = ecs_new(world, EcsPrefabParent);
        ecs_set(world, flag, EcsPrefabParent, {.parent = prefab});
        ecs_map_set64(world->prefab_parent_index, prefab, flag);
    }

    return flag;
}

static
void add_prefab_child_to_builder(
    ecs_world_t *world,
    ecs_entity_t prefab,
    ecs_entity_t child)
{
    EcsPrefabBuilder *builder = ecs_get_ptr(world, prefab, EcsPrefabBuilder);
    if (!builder) {
        ecs_add(world, prefab, EcsPrefabBuilder);
        builder = ecs_get_ptr(world, prefab, EcsPrefabBuilder);
        builder->stack_depth = 0;
    }
}

/* -- Systems -- */

void EcsInitPrefab(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsPrefab, prefab, 1);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        prefab[i].parent = 0;
    }
}

void EcsSetPrefab(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;

    ECS_COLUMN(rows, EcsPrefab, prefab, 1);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t parent = prefab[i].parent;

        ecs_entity_t e = rows->entities[i];
        ecs_table_t *table = rows->table;

        ecs_entity_t *type = ecs_vector_first(table->type);
        uint32_t t, t_count = ecs_vector_count(table->type);

        ecs_entity_t found = 0;
        bool prefab_parent_flag_added = false;
        bool prefab_parent_added = false;

        /* Walk components of entity, find prefab */
        for (t = 0; t < t_count; t++) {
            EcsPrefabParent *pparent;

            ecs_entity_t component = type[t];

            if (parent != component) {
                if (ecs_has(world, component, EcsPrefab)) {
                    ecs_assert(found == 0, ECS_MORE_THAN_ONE_PREFAB, NULL);
                    found = component;

                } else if ((pparent = ecs_get_ptr(world, component, EcsPrefabParent))) {                    
                    if (pparent->parent != parent) {
                        /* If this entity has a flag that is for a different prefab,
                        * it must have switched prefabs. Remove the old flag. */
                       ecs_type_t old_type = ecs_type_from_entity(world, component);
                        _ecs_remove(world, e, old_type);
                    } else {
                        /* If the entity has a flag for the current prefab parent,
                        * keep track of it so we don't add it again. */
                        prefab_parent_flag_added = true;
                    }
                }
            } else {
                prefab_parent_added = true;
            }
        }

        /* Add the prefab parent to the type of the entity */
        if (!prefab_parent_added && parent) {
            ecs_adopt(world, e, parent);
            add_prefab_child_to_builder(world, parent, e);
        }

        /* Add the prefab parent flag to the type of the entity */
        if (!prefab_parent_flag_added && parent) {
            ecs_entity_t flag = get_prefab_parent_flag(world, parent);
            ecs_assert(flag != 0, ECS_INTERNAL_ERROR, NULL);

            /* Add the flag as a type instead of with adopt, to prevent adding
             * the EcsContainer flag, and to prevent tracking the flag for
             * changes, to keep it as light weight as possible. */
            ecs_type_t flag_type = ecs_type_from_entity(world, flag);
            _ecs_add(world, e, flag_type);
        }
    }
}


/* -- Private API -- */

/** Copy default values from base (and base of base) prefabs */
ecs_type_t ecs_copy_from_prefab(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entity_t entity,
    uint32_t offset,
    uint32_t limit,
    ecs_type_t type_id,
    ecs_type_t to_add)
{
    ecs_entity_t prefab;
    ecs_type_t entity_type = type_id;
    ecs_type_t modified = 0;
    ecs_table_column_t *columns = NULL;

    if (world->in_progress) {
        uint64_t row64 = ecs_map_get64(stage->entity_index, entity);
        if (row64) {
            ecs_row_t row = ecs_to_row(row64);
            entity_type = row.type_id;
        }
    }

    while ((prefab = ecs_map_get64(world->prefab_index, entity_type))) {
        /* Prefabs are only resolved from the main stage. Prefabs created while
         * iterating cannot be resolved in the same iteration. */
        ecs_row_t prefab_row = ecs_to_row(ecs_map_get64(world->main_stage.entity_index, prefab));

        ecs_table_t *prefab_table = ecs_world_get_table(
            world, stage, prefab_row.type_id);
        ecs_vector_t *prefab_type = prefab_table->type;
        ecs_table_column_t *prefab_columns = prefab_table->columns;

        ecs_vector_t *add_type = ecs_type_get(world, stage, to_add);
        ecs_entity_t *add_handles = ecs_vector_first(add_type);
        uint32_t i, add_count = ecs_vector_count(add_type);

        for (i = 0; i < add_count; i ++) {
            ecs_entity_t component = add_handles[i];

            /* Nothing to copy if this component is the prefab itself */
            if (component != prefab) {
                void *prefab_ptr = ecs_get_row_ptr(prefab_type, prefab_columns, 
                    prefab_row.index, component);

                if (prefab_ptr) {
                    if (!columns) {
                        if (world->in_progress) {
                            columns = ecs_map_get(stage->data_stage, type_id);
                        } else {
                            columns = table->columns;
                        }
                    }

                    ecs_vector_t *type_arr = ecs_type_get(world, stage, type_id);
                    uint32_t column_index = ecs_type_index_of(type_arr, component);
                    uint32_t size = columns[column_index + 1].size;

                    if (size) {
                        void *buffer = ecs_vector_first(columns[column_index + 1].data);
                        void *ptr = ECS_OFFSET(buffer, offset * size);
                        uint32_t i;
                        for (i = 0; i < limit; i ++) {
                            memcpy(ptr, prefab_ptr, size);
                            ptr = ECS_OFFSET(ptr, size);
                        }

                        /* Keep track of which components were set by prefabs */
                        modified = ecs_type_add(world, stage, modified, component);
                    }
                }
            } else {
                /* If the prefab itself is added, test if the prefab has any
                 * children that need to be instantiated for the entity */
                EcsPrefabBuilder *builder = ecs_get_row_ptr(prefab_type, prefab_columns, 
                    prefab_row.index, EEcsPrefabBuilder);
            }
        }

        /* Recursively search through prefabs */
        entity_type = prefab_row.type_id;
    }

    return modified;
}