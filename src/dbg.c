#include "flecs_private.h"

ecs_table_t *ecs_dbg_find_table(
    ecs_world_t *world,
    ecs_type_t type)
{
    return ecs_world_get_table(world, &world->main_stage, type);
}

void ecs_dbg_table(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_dbg_table_t *dbg_out)
{
    *dbg_out = (ecs_dbg_table_t){.table = table};

    dbg_out->type = table->type;
    dbg_out->systems_matched = table->frame_systems;

    /* Determine components from parent/base entities */
    ecs_entity_t *entities = ecs_vector_first(table->type);
    uint32_t i, count = ecs_vector_count(table->type);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        if (e & ECS_CHILDOF) {
            ecs_dbg_entity_t parent_dbg;
            ecs_dbg_entity(world, e & ECS_ENTITY_MASK, &parent_dbg);

            ecs_dbg_table_t parent_table_dbg;
            ecs_dbg_table(world, parent_dbg.table, &parent_table_dbg);

            /* Owned and shared components are available from container */
            dbg_out->container = ecs_type_merge(
                world, dbg_out->container, parent_dbg.type, NULL);
            dbg_out->container = ecs_type_merge(
                world, dbg_out->container, parent_table_dbg.shared, NULL);

            /* Add entity to list of parent entities */
            dbg_out->parent_entities = ecs_type_add(
                world, dbg_out->parent_entities, e & ECS_ENTITY_MASK);
        }

        if (e & ECS_INSTANCEOF) {
            ecs_dbg_entity_t base_dbg;
            ecs_dbg_entity(world, e & ECS_ENTITY_MASK, &base_dbg);

            ecs_dbg_table_t base_table_dbg;
            ecs_dbg_table(world, base_dbg.table, &base_table_dbg);            

            /* Owned and shared components are available from base */
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, base_dbg.type, NULL);
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, base_table_dbg.shared, NULL);

            /* Never inherit EcsId or EcsPrefab */
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, TEcsId);
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, TEcsPrefab);

            /* Shared components are always masked by owned components */
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, table->type);

            /* Add entity to list of base entities */
            dbg_out->base_entities = ecs_type_add(
                world, dbg_out->base_entities, e & ECS_ENTITY_MASK);

            /* Add base entities of entity to list of base entities */
            dbg_out->base_entities = ecs_type_add(
                world, base_table_dbg.base_entities, e & ECS_ENTITY_MASK);                                                       
        }
    }    

    if (table->columns) {
        dbg_out->entities = ecs_vector_first(table->columns[0].data);
        dbg_out->entities_count = ecs_vector_count(table->columns[0].data);
    }
}

ecs_table_t* ecs_dbg_get_table(
    ecs_world_t *world,
    uint32_t index)
{
    if (ecs_chunked_count(world->main_stage.tables) <= index) {
        return NULL;
    }

    return ecs_chunked_get(
        world->main_stage.tables, ecs_table_t, index);
}

bool ecs_dbg_filter_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_filter_t *filter)
{
    return ecs_type_match_w_filter(world, table->type, filter);
}

ecs_type_t ecs_dbg_table_get_type(
    ecs_table_t *table)
{
    return table->type;
}

int ecs_dbg_col_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_dbg_col_system_t *dbg_out)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        return -1;
    }

    *dbg_out = (ecs_dbg_col_system_t){.system = system};
    dbg_out->active_table_count = ecs_vector_count(system_data->tables);
    dbg_out->inactive_table_count = ecs_vector_count(system_data->inactive_tables);
    dbg_out->enabled = system_data->base.enabled;

    ecs_matched_table_t *mt = ecs_vector_first(system_data->tables);
    uint32_t i, count = ecs_vector_count(system_data->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = mt[i].table;
        if (table && table->columns) {
            dbg_out->entities_matched_count += ecs_vector_count(
                table->columns[0].data);
        }
    }

    /* Inactive tables are inactive because they are empty, so no need to 
     * iterate them */

    return 0;
}

ecs_table_t* ecs_dbg_active_table(
    ecs_world_t *world,
    ecs_dbg_col_system_t *dbg,
    uint32_t index)
{
    (void)world;

    EcsColSystem *system_data = dbg->system_data;
    ecs_matched_table_t *table = ecs_vector_get(
        system_data->tables, &matched_table_params, index);
    if (!table) {
        return NULL;
    }
    
    return table->table;
}

ecs_table_t* ecs_dbg_inactive_table(
    ecs_world_t *world,
    ecs_dbg_col_system_t *dbg,
    uint32_t index)
{
    (void)world;

    EcsColSystem *system_data = dbg->system_data;
    ecs_matched_table_t *table = ecs_vector_get(
        system_data->inactive_tables, &matched_table_params, index);
    if (!table) {
        return NULL;
    }
    
    return table->table;    
}

ecs_type_t ecs_dbg_get_column_type(
    ecs_world_t *world,
    ecs_entity_t system,
    uint32_t column_index)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        return NULL;
    }
    
    ecs_system_column_t *columns = ecs_vector_first(system_data->base.columns);
    uint32_t count = ecs_vector_count(system_data->base.columns);

    if (count < column_index) {
        return NULL;
    }

    ecs_system_column_t *column = &columns[column_index - 1];
    ecs_system_expr_oper_kind_t oper_kind = column->oper_kind;
    ecs_type_t result;

    switch(oper_kind) {
    case EcsOperOr:
        result = column->is.type;
        break;
    default:
        result = ecs_type_from_entity(world, column->is.component);
        break;
    }
    
    return result;
}
