#include "../flecs_private.h"
#include "flecs/addon/dbg.h"

ecs_table_t *ecs_dbg_find_table(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_table_t *table = ecs_table_from_type(
        world, &world->stage, type);
        
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table;
}

void ecs_dbg_table(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_dbg_table_t *dbg_out)
{
    *dbg_out = (ecs_dbg_table_t){.table = table};

    dbg_out->type = table->type;
    dbg_out->systems_matched = table->queries;

    /* Determine components from parent/base entities */
    ecs_entity_t *entities = ecs_vector_first(table->type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(table->type);

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

            /* Never inherit EcsName or EcsPrefab */
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, ecs_type(EcsName));
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, ecs_type_from_entity(world, EcsPrefab));

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

    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        dbg_out->entities = ecs_vector_first(data->entities, ecs_entity_t);
        dbg_out->entities_count = ecs_vector_count(data->entities);
    }
}

ecs_table_t* ecs_dbg_get_table(
    ecs_world_t *world,
    int32_t index)
{
    if (ecs_sparse_count(world->stage.tables) <= index) {
        return NULL;
    }

    return ecs_sparse_get(
        world->stage.tables, ecs_table_t, index);
}

bool ecs_dbg_filter_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_filter_t *filter)
{
    return ecs_table_match_filter(world, table, filter);
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
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data) {
        return -1;
    }

    *dbg_out = (ecs_dbg_col_system_t){.system = system};
    dbg_out->active_table_count = ecs_vector_count(system_data->query->tables);
    dbg_out->inactive_table_count = ecs_vector_count(system_data->query->empty_tables);
    dbg_out->enabled = !ecs_has_entity(world, system, EcsDisabled);

    ecs_vector_each(system_data->query->tables, ecs_matched_table_t, mt, {
        ecs_table_t *table = mt->table;
        if (table) {
            dbg_out->entities_matched_count += ecs_table_count(table);
        }        
    });

    /* Inactive tables are inactive because they are empty, so no need to 
     * iterate them */

    return 0;
}

ecs_table_t* ecs_dbg_active_table(
    ecs_world_t *world,
    ecs_dbg_col_system_t *dbg,
    int32_t index)
{
    (void)world;

    EcsSystem *system_data = dbg->system_data;
    ecs_matched_table_t *table = ecs_vector_get(
        system_data->query->tables, ecs_matched_table_t, index);
    if (!table) {
        return NULL;
    }
    
    return table->table;
}

ecs_table_t* ecs_dbg_inactive_table(
    ecs_world_t *world,
    ecs_dbg_col_system_t *dbg,
    int32_t index)
{
    (void)world;

    EcsSystem *system_data = dbg->system_data;
    ecs_matched_table_t *table = ecs_vector_get(
        system_data->query->empty_tables, ecs_matched_table_t, index);
    if (!table) {
        return NULL;
    }
    
    return table->table;    
}

ecs_type_t ecs_dbg_get_column_type(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t column_index)
{
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data) {
        return NULL;
    }
    
    ecs_sig_column_t *columns = ecs_vector_first(
        system_data->query->sig.columns, ecs_sig_column_t);
    int32_t count = ecs_vector_count(system_data->query->sig.columns);

    if (count < column_index) {
        return NULL;
    }

    ecs_sig_column_t *column = &columns[column_index - 1];
    ecs_sig_oper_kind_t oper_kind = column->oper_kind;
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

void ecs_dbg_entity(
    ecs_world_t *world, 
    ecs_entity_t entity, 
    ecs_dbg_entity_t *dbg_out)
{
    *dbg_out = (ecs_dbg_entity_t){.entity = entity};
    
    ecs_entity_info_t info = { 0 };
    if (ecs_get_info(world, &world->stage, entity, &info)) {
        dbg_out->table = info.table;
        dbg_out->row = info.row;
        dbg_out->is_watched = info.is_watched;
        dbg_out->type = info.table ? info.table->type : NULL;
    }
}

bool ecs_dbg_match_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t system,
    ecs_match_failure_t *failure_info_out)
{
    ecs_dbg_entity_t dbg;
    ecs_dbg_entity(world, entity, &dbg);

    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data) {
        failure_info_out->reason = EcsMatchNotASystem;
        failure_info_out->column = -1;
        return false;
    }

    return ecs_query_match(
        world, dbg.table, system_data->query, failure_info_out);
}
