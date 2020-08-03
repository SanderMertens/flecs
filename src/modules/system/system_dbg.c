
#include "flecs.h"

#ifdef FLECS_SYSTEM
#ifdef FLECS_DBG

#include "system.h"

int ecs_dbg_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_dbg_system_t *dbg_out)
{
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data) {
        return -1;
    }

    *dbg_out = (ecs_dbg_system_t){.system = system};
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


ecs_table_t* ecs_dbg_active_table(
    ecs_world_t *world,
    ecs_dbg_system_t *dbg,
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
    ecs_dbg_system_t *dbg,
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

#endif
#endif
