#include "flecs_private.h"

typedef struct EcsTablePtr {
    ecs_table_t *table;
} EcsTablePtr;

/* -- Systems that add components on interest */

static
void AddWorldStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsWorldStats, 1);

    ecs_add(rows->world, EcsWorld, EcsWorldStats);
}

static
void AddAllocStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsAllocStats, 1);

    ecs_add(rows->world, EcsWorld, EcsAllocStats);
}

static
void AddMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsMemoryStats, 1);

    ecs_add(rows->world, EcsWorld, EcsMemoryStats);
}

static
void AddSystemStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsSystemStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], EcsSystemStats);
    }
}

static
void AddRowSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsRowSystemMemoryStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], EcsRowSystemMemoryStats);
    }
}

static
void AddColSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsColSystemMemoryStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], EcsColSystemMemoryStats);
    }
}

static
void AddComponentStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsComponentStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], EcsComponentStats);
    }
}

static
void AddTableStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsTableStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], EcsTableStats);
    }
}

static
void AddTypeStats(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, EcsTypeStats, 2);
    
    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], EcsTypeStats);
    }
}

/* -- Systems that collect metrics on interest -- */

static
void CollectWorldStats_StatusAction(
    ecs_world_t *world, 
    ecs_entity_t system,
    ecs_system_status_t status, 
    void *ctx)
{
    if (status == EcsSystemActivated) {
        ecs_measure_frame_time(world, true);
    } else if (status == EcsSystemDeactivated) {
        ecs_measure_frame_time(world, false);
    }
}

static
void CollectWorldStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_world_t *world = rows->world;

    stats->entity_count = ecs_map_count(world->main_stage.entity_index);
    stats->table_count = ecs_chunked_count(world->main_stage.tables);
    stats->component_count = ecs_count(world, EcsComponent);
    stats->thread_count = ecs_vector_count(world->worker_threads);
    stats->frame_time = world->frame_time;
    stats->system_time = world->system_time;
    stats->merge_time = world->merge_time;
    stats->world_time = world->world_time;
    stats->target_fps = world->target_fps;
    stats->tick_count = world->tick_count;

    world->frame_time = 0;
    world->system_time = 0;
    world->merge_time = 0;
}

static
void CollectAllocStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsAllocStats, stats, 1);

    stats->malloc_count = ecs_os_api_malloc_count;
    stats->calloc_count = ecs_os_api_calloc_count;
    stats->realloc_count = ecs_os_api_realloc_count;
    stats->free_count = ecs_os_api_free_count;
}

static
void components_memory(
    ecs_world_t *world,
    EcsMemoryStats *stats)
{
    ecs_chunked_t *tables = world->main_stage.tables;
    uint32_t i, count = ecs_chunked_count(tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, i);

        ecs_table_column_t *columns = table->columns;
        uint32_t c, column_count = ecs_vector_count(table->type);
        for (c = 0; c < column_count; c ++) {
            ecs_table_column_t *column = &columns[c];
            ecs_vector_params_t params = {.element_size = column->size};
            ecs_vector_memory(column->data, &params, &stats->components.allocd, &stats->components.used);
        }
    }
}

static
void CollectColSystemMemoryTotals(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsColSystemMemoryStats, stats, 1);

    ecs_memory_stat_t *stat = rows->param;

    printf("collect_col_system_memory_total\n");

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stat->allocd += 
            stats[i].base +
            stats[i].columns.allocd +
            stats[i].active_tables.allocd +
            stats[i].inactive_tables.allocd +
            stats[i].jobs.allocd +
            stats[i].other;

        stat->used += 
            stats[i].base +
            stats[i].columns.used +
            stats[i].active_tables.used +
            stats[i].inactive_tables.used +
            stats[i].jobs.used +
            stats[i].other;            
    }
}

static
void CollectMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsMemoryStats, stats, 1);
    ECS_COLUMN_ENTITY(rows, CollectColSystemMemoryTotals, 2);

    ecs_world_t *world = rows->world;

    /* Compute entity memory */
    ecs_map_memory(world->main_stage.entity_index, &stats->entities.allocd, &stats->entities.used);
    
    /* Compute component memory */
    components_memory(world, stats);

    /* Compute system memory */
    stats->systems = (ecs_memory_stat_t){0};
    ecs_run(world, CollectColSystemMemoryTotals, 0, &stats->systems);
    
}

static
void CollectSystemStats_StatusAction(
    ecs_world_t *world, 
    ecs_entity_t system,
    ecs_system_status_t status, 
    void *ctx)
{
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
void CollectSystemStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsColSystem, system, 1);
    ECS_COLUMN(rows, EcsSystemStats, stats, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t entity = rows->entities[i];

        stats[i].id = ecs_get_id(rows->world, entity);
        stats[i].handle = entity;
        stats[i].signature = system[i].base.signature;
        stats[i].kind = system[i].base.kind;
        stats[i].enabled = system[i].base.enabled;
        stats[i].active = ecs_vector_count(system[i].tables);
        stats[i].is_hidden = ecs_has(rows->world, entity, EcsHidden);
        stats[i].tables_matched = system_tables_matched(&system[i]);
        stats[i].entities_matched = system_entities_matched(&system[i]);
        stats[i].period = system[i].period;
        stats[i].time_spent = system[i].base.time_spent;
        stats[i].invoke_count = system[i].base.invoke_count;

        /* Reset time_spent between measuremen=t intervals */
        system[i].base.time_spent = 0;
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
    ecs_vector_memory(tables, &params, &stat->allocd, &stat->used);

    ecs_matched_table_t *tables_buffer = ecs_vector_first(tables);
    uint32_t i, count = ecs_vector_count(tables);

    /* The 'column' member in ecs_matched_table_t */
    stat->allocd += (sizeof(uint32_t) * column_count) * count;
    stat->used += (sizeof(uint32_t) * column_count) * count;

    /* The 'components' member of ecs_matched_table_t */
    stat->allocd += (sizeof(ecs_entity_t) * column_count) * count;
    stat->used += (sizeof(ecs_entity_t) * column_count) * count;

    /* Refs are different for each table, so we'll have to loop to get accurate 
     * numbers */
    for (i = 0; i < count; i ++) {
        ecs_vector_params_t params = {.element_size = sizeof(ecs_reference_t)};
        ecs_vector_memory(tables_buffer[i].references, &params, 
            &stat->allocd, &stat->used);
    }
}

static
void CollectColSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsColSystem, system, 1);
    ECS_COLUMN(rows, EcsColSystemMemoryStats, stats, 2);

    ecs_vector_params_t params;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stats[i].base = sizeof(EcsColSystem);
        stats[i].columns = (ecs_memory_stat_t){0};
        stats[i].active_tables = (ecs_memory_stat_t){0};
        stats[i].inactive_tables = (ecs_memory_stat_t){0};
        stats[i].jobs = (ecs_memory_stat_t){0};
        stats[i].other = 0;
        
        params.element_size = sizeof(ecs_system_column_t);
        ecs_vector_memory(system->base.columns, &params, 
            &stats[i].columns.allocd, &stats[i].columns.used);

        collect_system_table_metrics(system, system->tables, 
            &stats[i].active_tables);

        collect_system_table_metrics(system, system->inactive_tables, 
            &stats[i].inactive_tables);

        params.element_size = sizeof(ecs_job_t);
        ecs_vector_memory(system->jobs, &params, 
            &stats[i].jobs.allocd, &stats[i].jobs.used);            

        if (system->on_demand) {
            stats[i].other += sizeof(ecs_on_demand_out_t);
        }
    }
}

static
void CollectRowSystemMemoryStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsRowSystem, system, 1);
    ECS_COLUMN(rows, EcsRowSystemMemoryStats, stats, 2);

    ecs_vector_params_t params;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stats[i].base = sizeof(EcsColSystem);
        stats[i].columns = (ecs_memory_stat_t){0};
        stats[i].components = (ecs_memory_stat_t){0};

        params.element_size = sizeof(ecs_system_column_t);
        ecs_vector_memory(system->base.columns, &params, 
            &stats[i].columns.allocd, &stats[i].columns.used);

        params.element_size = sizeof(ecs_entity_t);
        ecs_vector_memory(system->components, &params, 
            &stats[i].components.allocd, &stats[i].components.used);
    }
}

static
void CollectComponentStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsComponent, component, 1);
    ECS_COLUMN(rows, EcsComponentStats, stats, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t entity = rows->entities[i];

        stats[i].id = ecs_get_id(rows->world, entity);
        stats[i].handle = entity;
        stats[i].size = component[i].size;
        
        /* Reset values */
        stats[i].tables = 0;
        stats[i].entities = 0;
        stats[i].memory_allocd = 0;
        stats[i].memory_used = 0;

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
                    stats[i].tables ++;
                    stats[i].entities += ecs_vector_count(column);
                    ecs_vector_params_t param = {.element_size = stats[i].size};
                    ecs_vector_memory(column, &param, 
                        &stats[i].memory_allocd, &stats[i].memory_used);
                    break;
                }
            }
        }
    }
}

static
void CollectTableStats_StatusAction(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx)
{
    ecs_type_t ecs_type(EcsTablePtr) = ctx;
    ecs_entity_t ecs_entity(EcsTablePtr) = ecs_type_to_entity(world, ecs_type(EcsTablePtr));

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
        ecs_delete_w_filter(world, &(ecs_type_filter_t){
            .include = ecs_type(EcsTablePtr),
            .include_kind = EcsMatchAny
        });
    }
}

static
void collect_table_data_memory(
    ecs_table_t *table,
    ecs_memory_stat_t *stat)
{
    uint32_t i, count = ecs_vector_count(table->type);
    ecs_table_column_t *columns = table->columns;

    *stat = (ecs_memory_stat_t){0};

    for (i = 0; i < count; i ++) {
        ecs_vector_params_t params = {
            .element_size = columns[0].size
        };

        ecs_vector_memory(columns[i].data, &params, &stat->allocd, &stat->used);
    }
}

static
void CollectTableStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTablePtr, table_ptr, 1);
    ECS_COLUMN(rows, EcsTableStats, stats, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_table_t *table = table_ptr[i].table;
        ecs_table_column_t *columns = table->columns;
        ecs_type_t type = table->type;
        stats[i].type = table->type;
        stats[i].column_count = ecs_vector_count(type);
        stats[i].entity_count = ecs_vector_count(columns[0].data);
        stats[i].systems_matched = ecs_vector_count(table->frame_systems);
        stats[i].other_memory = 
            sizeof(ecs_table_t) +
            sizeof(ecs_table_column_t) + ecs_vector_count(type) +
            sizeof(ecs_entity_t) * ecs_vector_count(table->frame_systems);

        collect_table_data_memory(table, &stats[i].data_memory);
    }
}

static
void CollectTypeStats(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTypeComponent, type_component, 1);
    ECS_COLUMN(rows, EcsTypeStats, stats, 2);

    ecs_world_t *world = rows->world;

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        stats[i].id = ecs_get_id(world, rows->entities[i]);
        stats[i].type = type_component[i].type;
        stats[i].normalized_type = type_component[i].resolved;
        stats[i].is_hidden = ecs_has(world, rows->entities[i], EcsHidden);
        stats[i].entity_count = 0;
        stats[i].entity_childof_count = 0;
        stats[i].entity_instanceof_count = 0;
        stats[i].component_count = 0;
        stats[i].col_system_count = 0;
        stats[i].row_system_count = 0;
        stats[i].enabled_system_count = 0;
        stats[i].active_system_count = 0;

        uint32_t j, count = ecs_vector_count(type_component[i].resolved);
        ecs_entity_t *entities = ecs_vector_first(type_component[i].resolved);
        for (j = 0; j < count; j ++) {
            ecs_entity_t e = entities[j];
            bool has_flags = false;
            
            if (e & ECS_CHILDOF) {
                stats[i].entity_childof_count ++;
                has_flags = true;
            }
            if (e & ECS_INSTANCEOF) {
                stats[i].entity_instanceof_count ++;
                has_flags = true;
            }
            if (!has_flags) {
                if (ecs_has(world, e, EcsComponent)) {
                    stats[i].component_count ++;
                } else
                if (ecs_has(world, e, EcsColSystem)) {
                    stats[i].col_system_count ++;
                    if (ecs_is_enabled(world, e)) {
                        stats[i].enabled_system_count ++;
                    }

                    EcsColSystem *ptr = ecs_get_ptr(world, e, EcsColSystem);
                    if (ecs_vector_count(ptr->tables)) {
                        stats[i].active_system_count ++;
                    }
                } else
                if (ecs_has(world, e, EcsRowSystem)) {
                    stats[i].row_system_count ++;
                    if (ecs_is_enabled(world, e)) {
                        stats[i].enabled_system_count ++;
                    }                    
                } else {
                    stats[i].entity_count ++;
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

    ECS_SYSTEM(world, CollectColSystemMemoryTotals, EcsManual, 
        [in] EcsColSystemMemoryStats,
        [out] EcsMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* -- Component creation systems -- */

    ECS_SYSTEM(world, AddWorldStats, EcsOnStore, [out] !EcsWorld.EcsWorldStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, AddAllocStats, EcsOnStore, [out] !EcsWorld.EcsAllocStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, AddMemoryStats, EcsPostLoad, [out] !EcsWorld.EcsMemoryStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, AddSystemStats, EcsOnStore,
        EcsColSystem, [out] !EcsSystemStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden, 
        SYSTEM.EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, AddRowSystemMemoryStats, EcsOnStore,
        EcsRowSystem, [out] !EcsRowSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, AddColSystemMemoryStats, EcsOnStore,
        EcsColSystem, [out] !EcsColSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden, 
        SYSTEM.EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, AddComponentStats, EcsOnStore,
        EcsComponent, [out] !EcsComponentStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, AddTableStats, EcsOnStore,
        EcsTablePtr, [out] !EcsTableStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, AddTypeStats, EcsOnStore,
        EcsTypeComponent, [out] !EcsTypeStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* -- Metrics collection systems -- */

    ECS_SYSTEM(world, CollectWorldStats, EcsPostLoad,
        [out] EcsWorldStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* This handler enables frame time monitoring when system is activated */
    ecs_set_system_status_action(
        world, CollectWorldStats, CollectWorldStats_StatusAction, NULL);

    ECS_SYSTEM(world, CollectAllocStats, EcsPostLoad,
        [out] EcsAllocStats, 
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, CollectMemoryStats, EcsPostLoad,
        [out] EcsMemoryStats,
        .CollectColSystemMemoryTotals,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);      

    ECS_SYSTEM(world, CollectSystemStats, EcsPostLoad,
        EcsColSystem, [out] EcsSystemStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* This handler enables system time monitoring when system is activated */
    ecs_set_system_status_action(
        world, CollectSystemStats, CollectSystemStats_StatusAction, NULL);

    ECS_SYSTEM(world, CollectColSystemMemoryStats, EcsPostLoad,
        EcsColSystem, [out] EcsColSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, CollectRowSystemMemoryStats, EcsPostLoad,
        EcsRowSystem, [out] EcsRowSystemMemoryStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, CollectComponentStats, EcsPostLoad,
        EcsComponent, [out] EcsComponentStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    ECS_SYSTEM(world, CollectTableStats, EcsPostLoad,
        EcsTablePtr, [out] EcsTableStats,
        SYSTEM.EcsOnDemand, SYSTEM.EcsHidden);

    /* This handler creates entities for tables when system is enabled */
    ecs_set_system_status_action(
        world, CollectTableStats, CollectTableStats_StatusAction, 
        ecs_type(EcsTablePtr));

    ECS_SYSTEM(world, CollectTypeStats, EcsPostLoad,
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
