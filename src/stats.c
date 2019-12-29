#include "flecs_private.h"

typedef struct EcsTablePtr {
    ecs_table_t *table;
} EcsTablePtr;

/* -- Systems that add components on interest */

static
void StatsAddWorldStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsWorldStats, 1);

    ecs_set(rows->world, EcsWorld, EcsWorldStats, {0});
}

static
void StatsAddAllocStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsAllocStats, 1);

    ecs_set(rows->world, EcsWorld, EcsAllocStats, {0});
}

static
void StatsAddMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsMemoryStats, 1);

    ecs_set(rows->world, EcsWorld, EcsMemoryStats, {0});
}

static
void StatsAddSystemStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsSystemStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsSystemStats, {0});
    }
}

static
void StatsAddRowSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsRowSystemMemoryStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsRowSystemMemoryStats, {0});
    }
}

static
void StatsAddColSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsColSystemMemoryStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsColSystemMemoryStats, {0});
    }
}

static
void StatsAddComponentStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsComponentStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsComponentStats, {0});
    }
}

static
void StatsAddTableStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsTableStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsTableStats, {0});
    }
}

static
void StatsAddTypeStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsTypeStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsTypeStats, {0});
    }
}

/* -- Systems that collect metrics on interest -- */

static
void StatsCollectWorldStats_StatusAction(
    ecs_world_t *world, 
    ecs_entity_t system,
    ecs_system_status_t status, 
    void *ctx)
{
    (void)system;
    (void)ctx;

    if (status == EcsSystemActivated) {
        ecs_measure_frame_time(world, true);
    } else if (status == EcsSystemDeactivated) {
        ecs_measure_frame_time(world, false);
    }
}

static
void StatsCollectWorldStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_world_t *world = rows->world;

    stats->entities_count = ecs_map_count(world->main_stage.entity_index);
    stats->components_count = ecs_count(world, EcsComponent);
    stats->col_systems_count = ecs_count(world, EcsColSystem);
    stats->row_systems_count = ecs_count(world, EcsRowSystem);
    stats->inactive_systems_count = ecs_vector_count(world->inactive_systems);
    stats->tables_count = ecs_chunked_count(world->main_stage.tables);
    stats->threads_count = ecs_vector_count(world->worker_threads);
    stats->frame_seconds_total = world->frame_time_total;
    stats->system_seconds_total = world->system_time_total;
    stats->merge_seconds_total = world->merge_time_total;
    stats->world_seconds_total = world->world_time_total;
    stats->target_fps_hz = world->target_fps;
    stats->frame_count_total = world->frame_count_total;
}

static
void StatsCollectAllocStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsAllocStats, stats, 1);

    stats->malloc_count_total = ecs_os_api_malloc_count;
    stats->calloc_count_total = ecs_os_api_calloc_count;
    stats->realloc_count_total = ecs_os_api_realloc_count;
    stats->free_count_total = ecs_os_api_free_count;
}

static
void StatsCollectColSystemMemoryTotals(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsColSystemMemoryStats, stats, 1);

    ecs_memory_stat_t *stat = rows->param;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stat->allocd_bytes += 
            stats[i].base_memory_bytes +
            stats[i].columns_memory.allocd_bytes +
            stats[i].active_tables_memory.allocd_bytes +
            stats[i].inactive_tables_memory.allocd_bytes +
            stats[i].jobs_memory.allocd_bytes +
            stats[i].other_memory_bytes;

        stat->used_bytes += 
            stats[i].base_memory_bytes +
            stats[i].columns_memory.used_bytes +
            stats[i].active_tables_memory.used_bytes +
            stats[i].inactive_tables_memory.used_bytes +
            stats[i].jobs_memory.used_bytes +
            stats[i].other_memory_bytes;            
    }
}

static
void StatsCollectTableMemoryTotals(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1);

    EcsMemoryStats *world_stats = rows->param;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        world_stats->components_memory.used_bytes += stats[i].component_memory.used_bytes;
        world_stats->components_memory.allocd_bytes += stats[i].component_memory.allocd_bytes;

        world_stats->entities_memory.used_bytes += stats[i].entity_memory.used_bytes;
        world_stats->entities_memory.allocd_bytes += stats[i].entity_memory.allocd_bytes;

        world_stats->tables_memory.used_bytes += stats[i].other_memory_bytes;
        world_stats->tables_memory.allocd_bytes += stats[i].other_memory_bytes; 
    }
}

static
void compute_stage_memory(
    ecs_stage_t *stage, 
    EcsMemoryStats *stats)
{
    ecs_map_memory(stage->entity_index, 
        &stats->entities_memory.allocd_bytes, 
        &stats->entities_memory.used_bytes);

    ecs_chunked_memory(stage->tables,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);    

    ecs_map_memory(stage->table_index,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);

    ecs_map_memory(stage->data_stage,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);

    ecs_map_memory(stage->remove_merge,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);
}

static
void compute_world_memory(
    ecs_world_t *world,
    EcsMemoryStats *stats)
{
    stats->world_memory.used_bytes = 
    stats->world_memory.allocd_bytes = sizeof(ecs_world_t);

    /* Add memory spent on worker threads to world memory */
    ecs_vector_memory(world->worker_threads, &thread_arr_params,
        &stats->world_memory.allocd_bytes, &stats->world_memory.used_bytes);

    /* Add memory spent on on demand lookup structures to system memory */
    ecs_map_memory(world->on_enable_components, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);        
    ecs_map_memory(world->on_activate_components,
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);     

    /* Add memory spent on system arrays to system memory */
    ecs_vector_memory(world->on_load_systems, &handle_arr_params,
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->post_load_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->pre_update_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->on_update_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->on_validate_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->post_update_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->pre_store_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->on_store_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->manual_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->inactive_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);

    ecs_vector_memory(world->add_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->remove_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_vector_memory(world->set_systems, &handle_arr_params, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);

    ecs_vector_memory(world->fini_tasks, &handle_arr_params,
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);

    /* Add lookup indices for reactive systems to system memory */
    ecs_map_memory(world->type_sys_add_index, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_map_memory(world->type_sys_remove_index, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);
    ecs_map_memory(world->type_sys_set_index, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);

    /* Add table array to table memory */
    ecs_chunked_memory(world->main_stage.tables,
        &stats->tables_memory.allocd_bytes, &stats->tables_memory.used_bytes);
    ecs_map_memory(world->main_stage.table_index,
        &stats->tables_memory.allocd_bytes, &stats->tables_memory.used_bytes);

    /* Add misc lookup indices to world memory */
    ecs_map_memory(world->prefab_parent_index, 
        &stats->world_memory.allocd_bytes, &stats->world_memory.used_bytes);
    ecs_map_memory(world->type_handles, 
        &stats->world_memory.allocd_bytes, &stats->world_memory.used_bytes);   

    stats->stages_memory = (ecs_memory_stat_t){0};
    
    /* Compute memory used in temporary stage */
    compute_stage_memory(&world->temp_stage, stats);
    stats->stages_memory.used_bytes += sizeof(ecs_stage_t);
    stats->stages_memory.allocd_bytes += sizeof(ecs_stage_t);    

    /* Compute memory used in thread stages */
    uint32_t i, count = ecs_vector_count(world->worker_stages);
    ecs_stage_t *stages = ecs_vector_first(world->worker_stages);

    for (i = 0; i < count; i ++) {
        compute_stage_memory(&stages[i], stats);
    }

    /* Add memory used / allocated by worker_stages array */
    ecs_vector_memory(world->worker_stages, &stage_arr_params,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);
}

static
void StatsCollectMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsMemoryStats, stats, 1);
    ECS_COLUMN_ENTITY(rows, StatsCollectColSystemMemoryTotals, 2);
    ECS_COLUMN_ENTITY(rows, StatsCollectTableMemoryTotals, 3);

    ecs_world_t *world = rows->world;

    /* Compute entity memory (entity index) */
    stats->entities_memory = (ecs_memory_stat_t){0};
    ecs_map_memory(world->main_stage.entity_index, 
        &stats->entities_memory.allocd_bytes, 
        &stats->entities_memory.used_bytes);
    
    /* Compute entity (entity columns), component and table memory */
    stats->components_memory = (ecs_memory_stat_t){0};
    stats->tables_memory = (ecs_memory_stat_t){0};
    ecs_run(world, StatsCollectTableMemoryTotals, 0, stats);

    /* Compute system memory */
    stats->systems_memory = (ecs_memory_stat_t){0};
    ecs_run(world, StatsCollectColSystemMemoryTotals, 0, &stats->systems_memory);

    /* Compute world memory */
    compute_world_memory(world, stats);

    /* Add everything up to compute total memory */
    stats->total_memory.used_bytes =
        stats->entities_memory.used_bytes +
        stats->components_memory.used_bytes +
        stats->tables_memory.used_bytes +
        stats->types_memory.used_bytes +
        stats->stages_memory.used_bytes +
        stats->systems_memory.used_bytes;

    stats->total_memory.allocd_bytes =
        stats->entities_memory.allocd_bytes +
        stats->components_memory.allocd_bytes +
        stats->tables_memory.allocd_bytes +
        stats->types_memory.allocd_bytes +
        stats->stages_memory.allocd_bytes +
        stats->systems_memory.allocd_bytes;        
}

static
void StatsCollectSystemStats_StatusAction(
    ecs_world_t *world, 
    ecs_entity_t system,
    ecs_system_status_t status, 
    void *ctx)
{
    (void)system;
    (void)ctx;

    if (status == EcsSystemActivated) {
        ecs_measure_system_time(world, true);
    } else if (status == EcsSystemDeactivated) {
        ecs_measure_system_time(world, false);
    }
}

static
uint32_t system_tables_matched(EcsColSystem *system) {
    return ecs_vector_count(system->tables) +
           ecs_vector_count(system->inactive_tables);
}

static
uint32_t system_entities_matched(EcsColSystem *system) {
    ecs_matched_table_t *tables = ecs_vector_first(system->tables);
    uint32_t i, total = 0, count = ecs_vector_count(system->tables);

    for (i = 0; i < count; i ++) {
        if (tables[i].table) {
            total += ecs_table_count(tables[i].table);
        }
    }

    return total;
}

static
void StatsCollectSystemStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsColSystem, system, 1);
    ECS_COLUMN(rows, EcsSystemStats, stats, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t entity = rows->entities[i];

        stats[i].entity = entity;
        stats[i].name = ecs_get_id(rows->world, entity);
        stats[i].signature = system[i].base.signature;
        stats[i].kind = system[i].base.kind;
        stats[i].tables_matched_count = system_tables_matched(&system[i]);
        stats[i].entities_matched_count = system_entities_matched(&system[i]);
        stats[i].period_seconds = system[i].period;
        stats[i].seconds_total = system[i].base.time_spent;
        stats[i].invoke_count_total = system[i].base.invoke_count;
        stats[i].is_enabled = system[i].base.enabled;
        stats[i].is_active = ecs_vector_count(system[i].tables) != 0;
        stats[i].is_hidden = ecs_has(rows->world, entity, EcsHidden);
    }
}

static
void collect_system_table_metrics(
    EcsColSystem *system,
    ecs_vector_t *tables,
    ecs_memory_stat_t *stat)
{
    uint32_t column_count = ecs_vector_count(system->base.columns);

    ecs_vector_params_t params;

    params.element_size = sizeof(ecs_matched_table_t);
    ecs_vector_memory(tables, &params, &stat->allocd_bytes, &stat->used_bytes);

    ecs_matched_table_t *tables_buffer = ecs_vector_first(tables);
    uint32_t i, count = ecs_vector_count(tables);

    /* The 'column' member in ecs_matched_table_t */
    stat->allocd_bytes += (sizeof(uint32_t) * column_count) * count;
    stat->used_bytes += (sizeof(uint32_t) * column_count) * count;

    /* The 'components' member of ecs_matched_table_t */
    stat->allocd_bytes += (sizeof(ecs_entity_t) * column_count) * count;
    stat->used_bytes += (sizeof(ecs_entity_t) * column_count) * count;

    /* Refs are different for each table, so we'll have to loop to get accurate 
     * numbers */
    for (i = 0; i < count; i ++) {
        ecs_vector_params_t params = {.element_size = sizeof(ecs_reference_t)};
        ecs_vector_memory(tables_buffer[i].references, &params, 
            &stat->allocd_bytes, &stat->used_bytes);
    }
}

static
void StatsCollectColSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsColSystem, system, 1);
    ECS_COLUMN(rows, EcsColSystemMemoryStats, stats, 2);

    ecs_vector_params_t params;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stats[i].base_memory_bytes = sizeof(EcsColSystem);
        stats[i].columns_memory = (ecs_memory_stat_t){0};
        stats[i].active_tables_memory = (ecs_memory_stat_t){0};
        stats[i].inactive_tables_memory = (ecs_memory_stat_t){0};
        stats[i].jobs_memory = (ecs_memory_stat_t){0};
        stats[i].other_memory_bytes = 0;
        
        params.element_size = sizeof(ecs_system_column_t);
        ecs_vector_memory(system->base.columns, &params, 
            &stats[i].columns_memory.allocd_bytes, 
            &stats[i].columns_memory.used_bytes);

        collect_system_table_metrics(system, system->tables, 
            &stats[i].active_tables_memory);

        collect_system_table_metrics(system, system->inactive_tables, 
            &stats[i].inactive_tables_memory);

        params.element_size = sizeof(ecs_job_t);
        ecs_vector_memory(system->jobs, &params, 
            &stats[i].jobs_memory.allocd_bytes, 
            &stats[i].jobs_memory.used_bytes);            

        if (system->on_demand) {
            stats[i].other_memory_bytes += sizeof(ecs_on_demand_out_t);
        }
    }
}

static
void StatsCollectRowSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsRowSystem, system, 1);
    ECS_COLUMN(rows, EcsRowSystemMemoryStats, stats, 2);

    ecs_vector_params_t params;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stats[i].base_memory_bytes = sizeof(EcsColSystem);
        stats[i].columns_memory = (ecs_memory_stat_t){0};
        stats[i].components_memory = (ecs_memory_stat_t){0};

        params.element_size = sizeof(ecs_system_column_t);
        ecs_vector_memory(system->base.columns, &params, 
            &stats[i].columns_memory.allocd_bytes, 
            &stats[i].columns_memory.used_bytes);

        params.element_size = sizeof(ecs_entity_t);
        ecs_vector_memory(system->components, &params, 
            &stats[i].components_memory.allocd_bytes, 
            &stats[i].components_memory.used_bytes);
    }
}

static
void StatsCollectComponentStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsComponent, component, 1);
    ECS_COLUMN(rows, EcsComponentStats, stats, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t entity = rows->entities[i];

        stats[i].entity = entity;
        stats[i].name = ecs_get_id(rows->world, entity);
        stats[i].size_bytes = component[i].size;
        
        /* Reset values */
        stats[i].tables_count = 0;
        stats[i].entities_count = 0;
        stats[i].memory = (ecs_memory_stat_t){0};

        /* Walk tables to collect memory and entity stats per component */
        ecs_chunked_t *tables = rows->world->main_stage.tables;
        uint32_t t, count = ecs_chunked_count(rows->world->main_stage.tables);

        for (t = 0; t < count; t ++) {
            ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, t);
            ecs_entity_t *components = ecs_vector_first(table->type);
            uint32_t c, c_count = ecs_vector_count(table->type);

            /* Iterate over table columns until component is found */
            for (c = 0; c < c_count; c ++) {
                if (components[c] == entity) {
                    ecs_vector_t *column = table->columns[c].data;
                    stats[i].tables_count ++;
                    stats[i].entities_count += ecs_vector_count(column);
                    ecs_vector_params_t param = {
                        .element_size = stats[i].size_bytes
                    };
                    ecs_vector_memory(column, &param, 
                        &stats[i].memory.allocd_bytes, 
                        &stats[i].memory.used_bytes);
                    break;
                }
            }
        }
    }
}

static
void StatsCollectTableStats_StatusAction(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx)
{
    ecs_type_t ecs_type(EcsTablePtr) = ctx;
    ecs_entity_t ecs_entity(EcsTablePtr) = ecs_type_to_entity(world, ecs_type(EcsTablePtr));

    (void)system;

    if (status == EcsSystemEnabled) {
        /* Create an entity for every table */
        ecs_chunked_t *tables = world->main_stage.tables;
        uint32_t i, count = ecs_chunked_count(tables);

        for (i = 0; i < count; i ++) {
            ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, i);
            ecs_set(world, 0, EcsTablePtr, {table});
        }
    } else if (status == EcsSystemDisabled) {
        /* Delete all entities with EcsTable tag */
        ecs_delete_w_filter(world, &(ecs_filter_t){
            .include = ecs_type(EcsTablePtr),
            .include_kind = EcsMatchAny
        });
    }
}

static
void collect_table_data_memory(
    ecs_table_t *table,
    EcsTableStats *stats)
{
    uint32_t i, count = ecs_vector_count(table->type);
    ecs_table_column_t *columns = table->columns;

    ecs_vector_params_t params = {
        .element_size = sizeof(ecs_entity_t)
    };

    stats->entity_memory = (ecs_memory_stat_t){0};

    ecs_vector_memory(columns[0].data, &params, 
        &stats->entity_memory.allocd_bytes, 
        &stats->entity_memory.used_bytes);

    stats->component_memory = (ecs_memory_stat_t){0};

    for (i = 1; i <= count; i ++) {
        ecs_vector_params_t params = {
            .element_size = columns[i].size
        };

        ecs_vector_memory(columns[i].data, &params, 
            &stats->component_memory.allocd_bytes, 
            &stats->component_memory.used_bytes);
    }
}

static
void StatsCollectTableStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTablePtr, table_ptr, 1);
    ECS_COLUMN(rows, EcsTableStats, stats, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_table_t *table = table_ptr[i].table;
        ecs_table_column_t *columns = table->columns;
        ecs_type_t type = table->type;
        stats[i].type = table->type;
        stats[i].columns_count = ecs_vector_count(type);
        stats[i].rows_count = ecs_vector_count(columns[0].data);
        stats[i].systems_matched_count = ecs_vector_count(table->frame_systems);
        stats[i].other_memory_bytes = 
            sizeof(ecs_table_column_t) + ecs_vector_count(type) +
            sizeof(ecs_entity_t) * ecs_vector_count(table->frame_systems);

        collect_table_data_memory(table, &stats[i]);
    }
}

static
void StatsCollectTypeStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTypeComponent, type_component, 1);
    ECS_COLUMN(rows, EcsTypeStats, stats, 2);

    ecs_world_t *world = rows->world;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stats[i].name = ecs_get_id(world, rows->entities[i]);
        stats[i].entity = rows->entities[i];
        stats[i].type = type_component[i].type;
        stats[i].normalized_type = type_component[i].resolved;
        stats[i].is_hidden = ecs_has(world, rows->entities[i], EcsHidden);
        stats[i].entities_count = 0;
        stats[i].entities_childof_count = 0;
        stats[i].entities_instanceof_count = 0;
        stats[i].components_count = 0;
        stats[i].col_systems_count = 0;
        stats[i].row_systems_count = 0;
        stats[i].enabled_systems_count = 0;
        stats[i].active_systems_count = 0;
        stats[i].instance_count = _ecs_count(world, type_component[i].resolved);

        uint32_t j, count = ecs_vector_count(type_component[i].resolved);
        ecs_entity_t *entities = ecs_vector_first(type_component[i].resolved);
        for (j = 0; j < count; j ++) {
            ecs_entity_t e = entities[j];
            bool has_flags = false;
            
            if (e & ECS_CHILDOF) {
                stats[i].entities_childof_count ++;
                has_flags = true;
            }
            if (e & ECS_INSTANCEOF) {
                stats[i].entities_instanceof_count ++;
                has_flags = true;
            }
            if (!has_flags) {
                if (ecs_has(world, e, EcsComponent)) {
                    stats[i].components_count ++;
                } else
                if (ecs_has(world, e, EcsColSystem)) {
                    stats[i].col_systems_count ++;
                    if (ecs_is_enabled(world, e)) {
                        stats[i].enabled_systems_count ++;
                    }

                    EcsColSystem *ptr = ecs_get_ptr(world, e, EcsColSystem);
                    if (ecs_vector_count(ptr->tables)) {
                        stats[i].active_systems_count ++;
                    }
                } else
                if (ecs_has(world, e, EcsRowSystem)) {
                    stats[i].row_systems_count ++;
                    if (ecs_is_enabled(world, e)) {
                        stats[i].enabled_systems_count ++;
                    }                    
                } else {
                    stats[i].entities_count ++;
                }
            }
        }
    }
}

/* -- Module import function -- */

void FlecsStatsImport(
    ecs_world_t *world,
    int flags)
{
    (void)flags;
    
    ECS_MODULE(world, FlecsStats);

    ECS_COMPONENT(world, EcsAllocStats);
    ECS_COMPONENT(world, EcsWorldStats);
    ECS_COMPONENT(world, EcsMemoryStats);
    ECS_COMPONENT(world, EcsSystemStats);
    ECS_COMPONENT(world, EcsColSystemMemoryStats);
    ECS_COMPONENT(world, EcsRowSystemMemoryStats);
    ECS_COMPONENT(world, EcsComponentStats);
    ECS_COMPONENT(world, EcsTableStats);
    ECS_COMPONENT(world, EcsTablePtr);
    ECS_COMPONENT(world, EcsTypeStats);
    
    ECS_TAG(world, EcsStatsSkipCollect);

    /* -- Helper systems -- */

    ECS_SYSTEM(world, StatsCollectColSystemMemoryTotals, EcsManual, 
        [in] EcsColSystemMemoryStats,
        [out] EcsWorld.EcsMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsCollectTableMemoryTotals, EcsManual, 
        [in] EcsTableStats,
        [out] EcsWorld.EcsMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* -- Component creation systems -- */

    ECS_SYSTEM(world, StatsAddWorldStats, EcsOnStore, [out] !EcsWorld.EcsWorldStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsAddAllocStats, EcsOnStore, [out] !EcsWorld.EcsAllocStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsAddMemoryStats, EcsPostLoad, [out] !EcsWorld.EcsMemoryStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsAddSystemStats, EcsOnStore,
        EcsColSystem, [out] !EcsSystemStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden, 
        SYSTEM.EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, StatsAddRowSystemMemoryStats, EcsOnStore,
        EcsRowSystem, [out] !EcsRowSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsAddColSystemMemoryStats, EcsOnStore,
        EcsColSystem, [out] !EcsColSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden, 
        SYSTEM.EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, StatsAddComponentStats, EcsOnStore,
        EcsComponent, [out] !EcsComponentStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsAddTableStats, EcsOnStore,
        EcsTablePtr, [out] !EcsTableStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsAddTypeStats, EcsOnStore,
        EcsTypeComponent, [out] !EcsTypeStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* -- Metrics collection systems -- */

    ECS_SYSTEM(world, StatsCollectWorldStats, EcsPostLoad,
        [out] EcsWorldStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* This handler enables frame time monitoring when system is activated */
    ecs_set_system_status_action(
        world, StatsCollectWorldStats, StatsCollectWorldStats_StatusAction, NULL);

    ECS_SYSTEM(world, StatsCollectAllocStats, EcsPostLoad,
        [out] EcsAllocStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsCollectMemoryStats, EcsPostLoad,
        [out] EcsMemoryStats,
        .StatsCollectColSystemMemoryTotals,
        .StatsCollectTableMemoryTotals,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);      

    ECS_SYSTEM(world, StatsCollectSystemStats, EcsPostLoad,
        EcsColSystem, [out] EcsSystemStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* This handler enables system time monitoring when system is activated */
    ecs_set_system_status_action(
        world, StatsCollectSystemStats, StatsCollectSystemStats_StatusAction, NULL);

    ECS_SYSTEM(world, StatsCollectColSystemMemoryStats, EcsPostLoad,
        EcsColSystem, [out] EcsColSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsCollectRowSystemMemoryStats, EcsPostLoad,
        EcsRowSystem, [out] EcsRowSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsCollectComponentStats, EcsPostLoad,
        EcsComponent, [out] EcsComponentStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, StatsCollectTableStats, EcsPostLoad,
        EcsTablePtr, [out] EcsTableStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* This handler creates entities for tables when system is enabled */
    ecs_set_system_status_action(
        world, StatsCollectTableStats, StatsCollectTableStats_StatusAction, 
        ecs_type(EcsTablePtr));

    ECS_SYSTEM(world, StatsCollectTypeStats, EcsPostLoad,
        EcsTypeComponent, [out] EcsTypeStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* Export components to module */
    ECS_EXPORT_COMPONENT(EcsAllocStats);
    ECS_EXPORT_COMPONENT(EcsWorldStats);
    ECS_EXPORT_COMPONENT(EcsMemoryStats);
    ECS_EXPORT_COMPONENT(EcsSystemStats);
    ECS_EXPORT_COMPONENT(EcsColSystemMemoryStats);
    ECS_EXPORT_COMPONENT(EcsRowSystemMemoryStats);
    ECS_EXPORT_COMPONENT(EcsComponentStats);
    ECS_EXPORT_COMPONENT(EcsTableStats);
    ECS_EXPORT_COMPONENT(EcsTablePtr);
    ECS_EXPORT_COMPONENT(EcsTypeStats);
}
