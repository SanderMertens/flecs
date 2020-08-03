#include "flecs.h"

#ifdef FLECS_STATS

#include "../private_api.h"
#include "system/system.h"

typedef struct EcsTablePtr {
    ecs_table_t *table;
} EcsTablePtr;

/* -- Systems that add components on interest */

static
void StatsAddWorldStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsWorldStats, 1);

    ecs_set(it->world, EcsWorld, EcsWorldStats, {0});
}

static
void StatsAddAllocStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsAllocStats, 1);

    ecs_set(it->world, EcsWorld, EcsAllocStats, {0});
}

static
void StatsAddMemoryStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsMemoryStats, 1);

    ecs_set(it->world, EcsWorld, EcsMemoryStats, {0});
}

static
void StatsAddSystemStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsSystemStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsSystemStats, {0});
    }
}

static
void StatsAddColSystemMemoryStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsSystemMemoryStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsSystemMemoryStats, {0});
    }
}

static
void StatsAddComponentStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsComponentStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsComponentStats, {0});
    }
}

static
void StatsAddTableStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsTableStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTableStats, {0});
    }
}

static
void StatsAddTypeStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsTypeStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTypeStats, {0});
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
void StatsCollectWorldStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsWorldStats, stats, 1);

    ecs_world_t *world = it->world;

    stats->entities_count = ecs_eis_count(&world->stage);
    stats->components_count = ecs_count(world, EcsComponent);
    stats->col_systems_count = ecs_count(world, EcsSystem);
    stats->tables_count = ecs_sparse_count(world->stage.tables);
    stats->threads_count = ecs_vector_count(world->workers);
    
    stats->frame_seconds_total = world->stats.frame_time_total;
    stats->system_seconds_total = world->stats.system_time_total;
    stats->merge_seconds_total = world->stats.merge_time_total;
    stats->world_seconds_total = world->stats.world_time_total;
    stats->target_fps_hz = world->stats.target_fps;
    stats->frame_count_total = world->stats.frame_count_total;
}

static
void StatsCollectAllocStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsAllocStats, stats, 1);

    stats->malloc_count_total = ecs_os_api_malloc_count;
    stats->calloc_count_total = ecs_os_api_calloc_count;
    stats->realloc_count_total = ecs_os_api_realloc_count;
    stats->free_count_total = ecs_os_api_free_count;
}

static
void StatsCollectColSystemMemoryTotals(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsSystemMemoryStats, stats, 1);

    ecs_memory_stat_t *stat = it->param;

    int32_t i;
    for (i = 0; i < it->count; i ++) {
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
void StatsCollectTableMemoryTotals(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsTableStats, stats, 1);

    EcsMemoryStats *world_stats = it->param;

    int32_t i;
    for (i = 0; i < it->count; i ++) {
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
    ecs_eis_memory(stage, 
        &stats->entities_memory.allocd_bytes, 
        &stats->entities_memory.used_bytes);

    ecs_sparse_memory(stage->tables,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);    
}

static
void compute_world_memory(
    ecs_world_t *world,
    EcsMemoryStats *stats)
{
    stats->world_memory.used_bytes = 
    stats->world_memory.allocd_bytes = ECS_SIZEOF(ecs_world_t);

    /* Add memory spent on worker threads to world memory */
    ecs_vector_memory(world->workers, ecs_thread_t,
        &stats->world_memory.allocd_bytes, &stats->world_memory.used_bytes);

    /* Add memory spent on on demand lookup structures to system memory */
    ecs_map_memory(world->on_enable_components, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);        
    ecs_map_memory(world->on_activate_components,
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);     

    ecs_vector_memory(world->fini_tasks, ecs_entity_t,
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);

    /* Add table array to table memory */
    ecs_sparse_memory(world->stage.tables,
        &stats->tables_memory.allocd_bytes, &stats->tables_memory.used_bytes);

    /* Add misc lookup indices to world memory */
    ecs_map_memory(world->type_handles, 
        &stats->world_memory.allocd_bytes, &stats->world_memory.used_bytes);   

    stats->stages_memory = (ecs_memory_stat_t){0};
    
    /* Compute memory used in temporary stage */
    compute_stage_memory(&world->temp_stage, stats);
    stats->stages_memory.used_bytes += ECS_SIZEOF(ecs_stage_t);
    stats->stages_memory.allocd_bytes += ECS_SIZEOF(ecs_stage_t);    

    /* Compute memory used in thread stages */
    int32_t i, count = ecs_vector_count(world->worker_stages);
    ecs_stage_t *stages = ecs_vector_first(world->worker_stages, ecs_stage_t);

    for (i = 0; i < count; i ++) {
        compute_stage_memory(&stages[i], stats);
    }

    /* Add memory used / allocated by worker_stages array */
    ecs_vector_memory(world->worker_stages, ecs_stage_t,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);
}

static
void StatsCollectMemoryStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsMemoryStats, stats, 1);
    ECS_COLUMN_ENTITY(it, StatsCollectColSystemMemoryTotals, 2);
    ECS_COLUMN_ENTITY(it, StatsCollectTableMemoryTotals, 3);

    ecs_world_t *world = it->world;

    /* Compute entity memory (entity index) */
    stats->entities_memory = (ecs_memory_stat_t){0};
    
    ecs_eis_memory(&world->stage, 
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
int32_t system_tables_matched(EcsSystem *system) {
    return ecs_vector_count(system->query->tables) +
           ecs_vector_count(system->query->empty_tables);
}

static
int32_t system_entities_matched(EcsSystem *system) {
    ecs_matched_table_t *tables = ecs_vector_first(system->query->tables, ecs_matched_table_t);
    int32_t i, total = 0, count = ecs_vector_count(system->query->tables);

    for (i = 0; i < count; i ++) {
        if (tables[i].table) {
            total += ecs_table_count(tables[i].table);
        }
    }

    return total;
}

static
void StatsCollectSystemStats(ecs_iter_t *it) {
    EcsSystem *system = ecs_column(it, EcsSystem, 1);
    ECS_COLUMN(it, EcsSystemStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t entity = it->entities[i];

        stats[i].entity = entity;
        stats[i].name = ecs_get_name(it->world, entity);
        stats[i].signature = system[i].query->sig.expr;
        stats[i].phase = 0;
        stats[i].tables_matched_count = system_tables_matched(&system[i]);
        stats[i].entities_matched_count = system_entities_matched(&system[i]);
        stats[i].period_seconds = ecs_get_interval(it->world, system[i].tick_source);
        stats[i].seconds_total = system[i].time_spent;
        stats[i].invoke_count_total = system[i].invoke_count;
        stats[i].is_enabled = !ecs_has_entity(it->world, entity, EcsDisabled);
        stats[i].is_active = ecs_vector_count(system[i].query->tables) != 0;
        stats[i].is_hidden = ecs_has_entity(it->world, entity, EcsHidden);
    }
}

static
void collect_system_table_metrics(
    EcsSystem *system,
    ecs_vector_t *tables,
    ecs_memory_stat_t *stat)
{
    int32_t column_count = ecs_vector_count(system->query->sig.columns);

    ecs_vector_memory(tables, ecs_matched_table_t, 
        &stat->allocd_bytes, &stat->used_bytes);

    ecs_matched_table_t *tables_buffer = ecs_vector_first(tables, ecs_matched_table_t);
    int32_t i, count = ecs_vector_count(tables);

    /* The 'column' member in ecs_matched_table_t */
    stat->allocd_bytes += (ECS_SIZEOF(int32_t) * column_count) * count;
    stat->used_bytes += (ECS_SIZEOF(int32_t) * column_count) * count;

    /* The 'components' member of ecs_matched_table_t */
    stat->allocd_bytes += (ECS_SIZEOF(ecs_entity_t) * column_count) * count;
    stat->used_bytes += (ECS_SIZEOF(ecs_entity_t) * column_count) * count;

    /* Refs are different for each table, so we'll have to loop to get accurate 
     * numbers */
    for (i = 0; i < count; i ++) {
        ecs_vector_memory(tables_buffer[i].references, ecs_ref_t, 
            &stat->allocd_bytes, &stat->used_bytes);
    }
}

static
void StatsCollectColSystemMemoryStats(ecs_iter_t *it) {
    EcsSystem *system = ecs_column(it, EcsSystem, 1);
    ECS_COLUMN(it, EcsSystemMemoryStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        stats[i].base_memory_bytes = ECS_SIZEOF(EcsSystem);
        stats[i].columns_memory = (ecs_memory_stat_t){0};
        stats[i].active_tables_memory = (ecs_memory_stat_t){0};
        stats[i].inactive_tables_memory = (ecs_memory_stat_t){0};
        stats[i].jobs_memory = (ecs_memory_stat_t){0};
        stats[i].other_memory_bytes = 0;
        
        ecs_vector_memory(system->query->sig.columns, ecs_sig_column_t, 
            &stats[i].columns_memory.allocd_bytes, 
            &stats[i].columns_memory.used_bytes);

        collect_system_table_metrics(system, system->query->tables, 
            &stats[i].active_tables_memory);

        collect_system_table_metrics(system, system->query->empty_tables, 
            &stats[i].inactive_tables_memory);            

        if (system->on_demand) {
            stats[i].other_memory_bytes += ECS_SIZEOF(ecs_on_demand_out_t);
        }
    }
}

static
void StatsCollectComponentStats(ecs_iter_t *it) {
    EcsComponent *component = ecs_column(it, EcsComponent, 1);
    ECS_COLUMN(it, EcsComponentStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t entity = it->entities[i];

        stats[i].entity = entity;
        stats[i].name = ecs_get_name(it->world, entity);
        stats[i].size_bytes = component[i].size;
        
        /* Reset values */
        stats[i].tables_count = 0;
        stats[i].entities_count = 0;
        stats[i].memory = (ecs_memory_stat_t){0};

        /* Walk tables to collect memory and entity stats per component */
        ecs_sparse_t *tables = it->world->stage.tables;
        int32_t t, count = ecs_sparse_count(it->world->stage.tables);

        for (t = 0; t < count; t ++) {
            ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, t);
            ecs_entity_t *components = ecs_vector_first(table->type, ecs_entity_t);
            int32_t c, c_count = table->column_count;

            /* Iterate over table columns until component is found */
            ecs_data_t *data = ecs_table_get_data(it->world, table);
            ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_column_t *columns = data->columns;
            ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

            for (c = 0; c < c_count; c ++) {
                if (components[c] == entity) {
                    ecs_vector_t *column = columns[c].data;
                    stats[i].tables_count ++;
                    stats[i].entities_count += ecs_vector_count(column);
                    ecs_vector_memory_t(column, columns[c].size, columns[c].alignment, 
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
        ecs_sparse_t *tables = world->stage.tables;
        int32_t i, count = ecs_sparse_count(tables);

        for (i = 0; i < count; i ++) {
            ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
            ecs_set(world, 0, EcsTablePtr, {table});
        }
    } else if (status == EcsSystemDisabled) {
        /* Delete all entities with EcsTable tag */
        ecs_bulk_delete(world, &(ecs_filter_t){
            .include = ecs_type(EcsTablePtr),
            .include_kind = EcsMatchAny
        });
    }
}

static
void collect_table_data_memory(
    ecs_world_t *world,
    ecs_table_t *table,
    EcsTableStats *stats)
{
    int32_t c, count = table->column_count;
    ecs_data_t *data = ecs_table_get_data(world, table);

    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    stats->entity_memory = (ecs_memory_stat_t){0};

    ecs_vector_memory(data->entities, ecs_entity_t, 
        &stats->entity_memory.allocd_bytes, 
        &stats->entity_memory.used_bytes);

    stats->component_memory = (ecs_memory_stat_t){0};

    for (c = 0; c < count; c ++) {
        ecs_column_t *column = &columns[c];
        ecs_vector_memory_t(column->data, columns->size, columns->alignment,
            &stats->component_memory.allocd_bytes, 
            &stats->component_memory.used_bytes);
    }
}

static
void StatsCollectTableStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsTablePtr, table_ptr, 1);
    ECS_COLUMN(it, EcsTableStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_table_t *table = table_ptr[i].table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_data_t *data = ecs_table_get_data(it->world, table);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_column_t *columns = data->columns;
        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_type_t type = table->type;
        stats[i].type = table->type;
        stats[i].columns_count = ecs_vector_count(type);
        stats[i].rows_count = ecs_vector_count(columns[0].data);
        stats[i].systems_matched_count = ecs_vector_count(table->queries);
        stats[i].other_memory_bytes = 
            ECS_SIZEOF(ecs_column_t) + ecs_vector_count(type) +
            ECS_SIZEOF(ecs_entity_t) * ecs_vector_count(table->queries);

        collect_table_data_memory(it->world, table, &stats[i]);
    }
}

static
void StatsCollectTypeStats(ecs_iter_t *it) {
    EcsType *type_component = ecs_column(it, EcsType, 1);
    ECS_COLUMN(it, EcsTypeStats, stats, 2);

    ecs_world_t *world = it->world;

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        stats[i].name = ecs_get_name(world, it->entities[i]);
        stats[i].entity = it->entities[i];
        stats[i].type = type_component[i].type;
        stats[i].normalized_type = type_component[i].normalized;
        stats[i].is_hidden = ecs_has_entity(world, it->entities[i], EcsHidden);
        stats[i].entities_count = 0;
        stats[i].entities_childof_count = 0;
        stats[i].entities_instanceof_count = 0;
        stats[i].components_count = 0;
        stats[i].col_systems_count = 0;
        stats[i].row_systems_count = 0;
        stats[i].enabled_systems_count = 0;
        stats[i].active_systems_count = 0;
        stats[i].instance_count = ecs_count_type(world, type_component[i].normalized);

        int32_t j, count = ecs_vector_count(type_component[i].normalized);
        ecs_entity_t *entities = ecs_vector_first(type_component[i].normalized, ecs_entity_t);
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
                if (ecs_has(world, e, EcsSystem)) {
                    stats[i].col_systems_count ++;
                    if (!ecs_has_entity(world, e, EcsDisabled)) {
                        stats[i].enabled_systems_count ++;
                    }

                    const EcsSystem *ptr = ecs_get(world, e, EcsSystem);
                    if (ecs_vector_count(ptr->query->tables)) {
                        stats[i].active_systems_count ++;
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
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsStats);

    ECS_IMPORT(world, FlecsPipeline);

    ecs_set_name_prefix(world, "Ecs");

    ECS_COMPONENT(world, EcsAllocStats);
    ECS_COMPONENT(world, EcsWorldStats);
    ECS_COMPONENT(world, EcsMemoryStats);
    ECS_COMPONENT(world, EcsSystemStats);
    ECS_COMPONENT(world, EcsSystemMemoryStats);
    ECS_COMPONENT(world, EcsRowSystemMemoryStats);
    ECS_COMPONENT(world, EcsComponentStats);
    ECS_COMPONENT(world, EcsTableStats);
    ECS_COMPONENT(world, EcsTablePtr);
    ECS_COMPONENT(world, EcsTypeStats);
    
    ECS_ENTITY(world, EcsStatsSkipCollect, 0);

    /* -- Helper systems -- */

    ECS_SYSTEM(world, StatsCollectColSystemMemoryTotals, 0, 
        [in] EcsSystemMemoryStats,
        [out] World:EcsMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectTableMemoryTotals, 0, 
        [in] EcsTableStats,
        [out] World:EcsMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* -- Component creation systems -- */

    ECS_SYSTEM(world, StatsAddWorldStats, EcsOnStore, [out] !World:EcsWorldStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddAllocStats, EcsOnStore, [out] !World:EcsAllocStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddMemoryStats, EcsPostLoad, [out] !World:EcsMemoryStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddSystemStats, EcsOnStore,
        flecs.system.EcsSystem, [out] !EcsSystemStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden, 
        SYSTEM:EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, StatsAddColSystemMemoryStats, EcsOnStore,
        flecs.system.EcsSystem, [out] !EcsSystemMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden, 
        SYSTEM:EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, StatsAddComponentStats, EcsOnStore,
        EcsComponent, [out] !EcsComponentStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddTableStats, EcsOnStore,
        EcsTablePtr, [out] !EcsTableStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddTypeStats, EcsOnStore,
        EcsType, [out] !EcsTypeStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* -- Metrics collection systems -- */

    ECS_SYSTEM(world, StatsCollectWorldStats, EcsPostLoad,
        [out] EcsWorldStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* This handler enables frame time monitoring when system is activated */
    ecs_set_system_status_action(
        world, StatsCollectWorldStats, StatsCollectWorldStats_StatusAction, NULL);

    ECS_SYSTEM(world, StatsCollectAllocStats, EcsPostLoad,
        [out] EcsAllocStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectMemoryStats, EcsPostLoad,
        [out] EcsMemoryStats,
        :StatsCollectColSystemMemoryTotals,
        :StatsCollectTableMemoryTotals,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);      

    ECS_SYSTEM(world, StatsCollectSystemStats, EcsPostLoad,
        flecs.system.EcsSystem, [out] EcsSystemStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* This handler enables system time monitoring when system is activated */
    ecs_set_system_status_action(
        world, StatsCollectSystemStats, StatsCollectSystemStats_StatusAction, NULL);

    ECS_SYSTEM(world, StatsCollectColSystemMemoryStats, EcsPostLoad,
        flecs.system.EcsSystem, [out] EcsSystemMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectComponentStats, EcsPostLoad,
        EcsComponent, [out] EcsComponentStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectTableStats, EcsPostLoad,
        EcsTablePtr, [out] EcsTableStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* This handler creates entities for tables when system is enabled */
    ecs_set_system_status_action(
        world, StatsCollectTableStats, StatsCollectTableStats_StatusAction, 
        ecs_type(EcsTablePtr));

    ECS_SYSTEM(world, StatsCollectTypeStats, EcsPostLoad,
        EcsType, [out] EcsTypeStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* Export components to module */
    ECS_EXPORT_COMPONENT(EcsAllocStats);
    ECS_EXPORT_COMPONENT(EcsWorldStats);
    ECS_EXPORT_COMPONENT(EcsMemoryStats);
    ECS_EXPORT_COMPONENT(EcsSystemStats);
    ECS_EXPORT_COMPONENT(EcsSystemMemoryStats);
    ECS_EXPORT_COMPONENT(EcsComponentStats);
    ECS_EXPORT_COMPONENT(EcsTableStats);
    ECS_EXPORT_COMPONENT(EcsTablePtr);
    ECS_EXPORT_COMPONENT(EcsTypeStats);
}

#endif
