#include "include/util/stats.h"
#include "include/private/reflecs.h"
#include <string.h>

const EcsArrayParams tablestats_arr_params = {
    .element_size = sizeof(EcsTableStats)
};

const EcsArrayParams systemstats_arr_params = {
    .element_size = sizeof(EcsSystemStats)
};

const EcsArrayParams featurestats_arr_params = {
    .element_size = sizeof(EcsFeatureStats)
};

static
void calculate_family_stats(
    EcsWorld *world,
    uint32_t *allocd,
    uint32_t *used)
{
    EcsIter it = ecs_map_iter(world->family_index);
    while (ecs_iter_hasnext(&it)) {
        EcsArray *family = ecs_iter_next(&it);
        ecs_array_memory(family, &handle_arr_params, allocd, used);
    }
}

static
void calculate_system_stats(
    EcsWorld *world,
    EcsArray *systems,
    uint32_t *allocd,
    uint32_t *used)
{
    EcsEntity *buffer = ecs_array_buffer(systems);
    uint32_t i, count = ecs_array_count(systems);
    for (i = 0; i < count; i ++) {
        EcsTableSystem *sys = ecs_get_ptr(world, buffer[i], EcsTableSystem_h);
        ecs_array_memory(sys->base.columns, &column_arr_params, allocd, used);
        ecs_array_memory(sys->components, &handle_arr_params, allocd, used);
        ecs_array_memory(sys->inactive_tables, &sys->table_params, allocd, used);
        ecs_array_memory(sys->jobs, &job_arr_params, allocd, used);
        ecs_array_memory(sys->tables, &sys->table_params, allocd, used);
        ecs_array_memory(sys->refs, &sys->ref_params, allocd, used);
    }
}

static
void calculate_table_stats(
    EcsWorld *world,
    uint32_t *allocd,
    uint32_t *used)
{
    EcsTable *buffer = ecs_array_buffer(world->table_db);
    uint32_t i, count = ecs_array_count(world->table_db);
    for (i = 0; i < count; i ++) {
        EcsTable *table = &buffer[i];
        ecs_array_memory(table->frame_systems, &handle_arr_params, allocd, used);
        *allocd += ecs_array_count(table->family) * sizeof(uint16_t);
        *used += ecs_array_count(table->family) * sizeof(uint16_t);
    }
}

static
void calculate_stage_stats(
    EcsStage *stage,
    uint32_t *allocd,
    uint32_t *used)
{
    ecs_map_memory(stage->add_stage, allocd, used);
    ecs_map_memory(stage->remove_stage, allocd, used);
    ecs_map_memory(stage->remove_merge, allocd, used);
    ecs_array_memory(stage->delete_stage, &handle_arr_params, allocd, used);
    ecs_map_memory(stage->entity_stage, allocd, used);
    ecs_map_memory(stage->data_stage, allocd, used);
    ecs_map_memory(stage->family_stage, allocd, used);
    ecs_array_memory(stage->table_db_stage, &table_arr_params, allocd, used);
    ecs_map_memory(stage->table_stage, allocd, used);
}

static
void calculate_stages_stats(
    EcsWorld *world,
    uint32_t *allocd,
    uint32_t *used)
{
    EcsStage *buffer = ecs_array_buffer(world->stage_db);
    uint32_t i, count = ecs_array_count(world->stage_db);
    for (i = 0; i < count; i ++) {
        EcsStage *stage = &buffer[i];
        calculate_stage_stats(stage, allocd, used);
    }
}

static
void get_memory_stats(
    EcsWorld *world,
    EcsWorldStats *stats)
{
    EcsMemoryStats *memory = &stats->memory;

    ecs_map_memory(world->entity_index, &memory->entities.allocd, &memory->entities.used);

    ecs_map_memory(world->add_systems, &memory->systems.allocd, &memory->systems.used);
    ecs_map_memory(world->set_systems, &memory->systems.allocd, &memory->systems.used);
    ecs_map_memory(world->remove_systems, &memory->systems.allocd, &memory->systems.used);

    ecs_array_memory(world->on_load_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_array_memory(world->pre_frame_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_array_memory(world->on_frame_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_array_memory(world->post_frame_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_array_memory(world->on_store_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_array_memory(world->tasks, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_array_memory(world->inactive_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_array_memory(world->on_demand_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);

    calculate_system_stats(world, world->on_load_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_system_stats(world, world->pre_frame_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_system_stats(world, world->on_frame_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_system_stats(world, world->post_frame_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_system_stats(world, world->on_store_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_system_stats(world, world->tasks, &memory->systems.allocd, &memory->systems.used);
    calculate_system_stats(world, world->inactive_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_system_stats(world, world->on_demand_systems, &memory->systems.allocd, &memory->systems.used);

    ecs_map_memory(world->family_index, &memory->families.allocd, &memory->families.used);
    ecs_map_memory(world->family_handles, &memory->families.allocd, &memory->families.used);
    ecs_map_memory(world->prefab_index, &memory->families.allocd, &memory->families.used);
    calculate_family_stats(world, &memory->families.allocd, &memory->families.used);

    ecs_map_memory(world->table_index, &stats->memory.tables.allocd, &stats->memory.tables.used);
    ecs_array_memory(world->table_db, &table_arr_params, &memory->tables.allocd, &memory->tables.used);
    calculate_table_stats(world, &memory->tables.allocd, &memory->tables.used);

    ecs_array_memory(world->stage_db, &table_arr_params, &memory->stage.allocd, &memory->stage.used);
    memory->stage.allocd += sizeof(EcsStage);
    memory->stage.used += sizeof(EcsStage);
    calculate_stage_stats(&world->stage, &memory->stage.allocd, &memory->stage.used);
    calculate_stages_stats(world, &memory->stage.allocd, &memory->stage.used);

    ecs_array_memory(world->worker_threads, &table_arr_params, &memory->world.allocd, &memory->world.used);
    stats->memory.world.allocd += sizeof(EcsWorld) - sizeof(EcsStage);
    stats->memory.world.used += sizeof(EcsWorld) - sizeof(EcsStage);

    stats->memory.total.used =
      stats->memory.components.used +
      stats->memory.entities.used +
      stats->memory.systems.used +
      stats->memory.families.used +
      stats->memory.tables.used +
      stats->memory.stage.used +
      stats->memory.world.used;

    stats->memory.total.allocd =
      stats->memory.components.allocd +
      stats->memory.entities.allocd +
      stats->memory.systems.allocd +
      stats->memory.families.allocd +
      stats->memory.tables.allocd +
      stats->memory.stage.allocd +
      stats->memory.world.allocd;
}

static
void set_system_stats(
    EcsWorld *world,
    EcsArray **stats_array,
    EcsEntity system,
    bool active)
{
    EcsId *id = ecs_get_ptr(world, system, EcsId_h);
    EcsSystemStats *sstats = ecs_array_add(
        stats_array, &systemstats_arr_params);

    sstats->system = system;
    sstats->id = *id;
    sstats->enabled = ecs_is_enabled(world, system);
    sstats->active = active;
    sstats->is_hidden = ecs_has(world, system, EcsHidden_h);

    EcsSystem *system_ptr = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (system_ptr) {
        EcsTableSystem *table_system = (EcsTableSystem*)system_ptr;
        EcsArray *tables = table_system->tables;
        uint32_t i, count = ecs_array_count(tables);

        sstats->entities_matched = 0;
        for (i = 0; i < count; i ++) {
            int32_t *index = ecs_array_get(tables, &table_system->table_params, i);
            EcsTable *table = ecs_array_get(
                world->table_db, &table_arr_params, *index);
            sstats->entities_matched += ecs_array_count(table->rows);
        }

        sstats->period = table_system->period;
        sstats->tables_matched = count + ecs_array_count(
            table_system->inactive_tables);
    } else {
        system_ptr = ecs_get_ptr(world, system, EcsRowSystem_h);
        EcsRowSystem *row_system = (EcsRowSystem*)system_ptr;
        assert(row_system != NULL);
        sstats->entities_matched = 0;
        sstats->tables_matched = 0;
        sstats->period = 0;
    }

    sstats->signature = system_ptr->signature;
    sstats->time_spent = system_ptr->time_spent;
    sstats->enabled = system_ptr->enabled;
    system_ptr->time_spent = 0;
}

static
int compare_sysstats(
    const void *e1,
    const void *e2)
{
    const EcsSystemStats *s1 = e1;
    const EcsSystemStats *s2 = e2;
    return strcmp(s1->id, s2->id);
}

static
int compare_tablestats(
    const void *e1,
    const void *e2)
{
    const EcsTableStats *t1 = e1;
    const EcsTableStats *t2 = e2;
    return t2->row_count > t1->row_count
      ? 1
      : t2->row_count < t1->row_count
        ? -1
        : 0
        ;
}

static
int system_stats_arr(
    EcsWorld *world,
    EcsArray **stats_array,
    EcsArray *systems,
    bool active)
{
    EcsEntity *handles = ecs_array_buffer(systems);
    uint32_t i, count = ecs_array_count(systems);
    for (i = 0; i < count; i ++) {
        set_system_stats(world, stats_array, handles[i], active);
    }

    ecs_array_sort(*stats_array, &systemstats_arr_params, compare_sysstats);

    return count;
}

static
int system_stats_map(
    EcsWorld *world,
    EcsArray **stats_array,
    EcsMap *systems)
{
    EcsIter it = ecs_map_iter(systems);
    while (ecs_iter_hasnext(&it)) {
        set_system_stats(world, stats_array, ecs_map_next(&it, NULL), true);
    }
    return ecs_map_count(systems);
}

void ecs_get_stats(
    EcsWorld *world,
    EcsWorldStats *stats)
{
    uint32_t mem_used = 0, mem_allocd = 0;
    stats->table_count = ecs_array_count(world->table_db);

    if (!stats->tables) {
        stats->tables = ecs_array_new(&tablestats_arr_params, stats->table_count);
    } else {
        ecs_array_clear(stats->tables);
    }

    EcsTable *tables = ecs_array_buffer(world->table_db);
    uint32_t i, count = ecs_array_count(world->table_db);
    for (i = 0; i < count; i ++) {
        EcsTable *table = &tables[i];
        EcsTableStats *tstats = ecs_array_add(
            &stats->tables, &tablestats_arr_params);
        uint32_t row_size = table->row_params.element_size;
        tstats->row_count = ecs_array_count(table->rows);
        tstats->memory_used = tstats->row_count * row_size;
        tstats->memory_allocd = ecs_array_size(table->rows) * row_size;
        tstats->columns = ecs_family_tostr(world, NULL, table->family_id);

        EcsEntity family_handle = ecs_map_get64(
            world->family_handles, table->family_id);
        if (family_handle) {
            tstats->id = ecs_id(world, family_handle);
        } else {
            tstats->id = NULL;
        }

        mem_used += tstats->memory_used;
        mem_allocd += tstats->memory_allocd;
    }

    ecs_array_sort(stats->tables, &tablestats_arr_params, compare_tablestats);

    if (!stats->features) {
        stats->features = ecs_array_new(&featurestats_arr_params, 0);
    } else {
        ecs_array_clear(stats->features);
    }

    EcsIter it = ecs_map_iter(world->family_handles);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity h = ecs_map_next(&it, NULL);
        EcsFamilyComponent *data = ecs_get_ptr(world, h, EcsFamilyComponent_h);
        EcsArray *family = ecs_map_get(world->family_index, data->resolved);
        EcsEntity *buffer = ecs_array_buffer(family);
        uint32_t i, count = ecs_array_count(family);

        EcsFeatureStats feature = {0};
        for (i = 0; i < count; i ++) {
            if (ecs_has(world, buffer[i], EcsTableSystem_h)) {
                feature.system_count ++;
                if (ecs_is_enabled(world, buffer[i])) {
                    feature.systems_enabled ++;
                }
            }
            if (ecs_has(world, buffer[i], EcsRowSystem_h)) {
                feature.system_count ++;
                feature.systems_enabled ++;
            }
        }

        if (feature.system_count) {
            EcsFeatureStats *elem = ecs_array_add(
                &stats->features, &featurestats_arr_params);
            *elem = feature;
            elem->id = ecs_id(world, h);
            elem->entities = ecs_family_tostr(world, NULL, data->family);
            elem->is_hidden = ecs_has(world, h, EcsHidden_h);
        }
    }

    stats->system_count = 0;
    stats->system_count += system_stats_arr(
        world, &stats->frame_systems, world->on_load_systems, true);
    stats->system_count += system_stats_arr(
        world, &stats->frame_systems, world->pre_frame_systems, true);
    stats->system_count += system_stats_arr(
        world, &stats->frame_systems, world->on_frame_systems, true);
    stats->system_count += system_stats_arr(
        world, &stats->frame_systems, world->post_frame_systems, true);
    stats->system_count += system_stats_arr(
        world, &stats->frame_systems, world->on_store_systems, true);
    stats->system_count += system_stats_arr(
        world, &stats->frame_systems, world->inactive_systems, false);
    stats->system_count += system_stats_arr(
        world, &stats->frame_systems, world->tasks, true);

    stats->system_count += system_stats_arr(
        world, &stats->on_demand_systems, world->on_demand_systems, true);

    uint32_t table_sys_count = stats->system_count;

    stats->system_count += system_stats_map(
        world, &stats->on_add_systems, world->add_systems);
    stats->system_count += system_stats_map(
        world, &stats->on_set_systems, world->set_systems);
    stats->system_count += system_stats_map(
        world, &stats->on_remove_systems, world->remove_systems);

    uint32_t row_sys_count = stats->system_count - table_sys_count;

    stats->memory.components.used = mem_used;
    stats->memory.components.allocd = mem_allocd;
    get_memory_stats(world, stats);

    uint32_t system_memory =
      table_sys_count * (sizeof(EcsTableSystem) + sizeof(EcsId)) +
      row_sys_count * (sizeof(EcsRowSystem) + sizeof(EcsId));
    stats->memory.components.used -= system_memory;
    stats->memory.components.allocd -= system_memory;
    stats->memory.systems.used += system_memory;
    stats->memory.systems.allocd += system_memory;

    uint32_t family_memory = ecs_map_count(world->family_index) *
      (sizeof(EcsFamilyComponent) + sizeof(EcsId));
    stats->memory.components.used -= family_memory;
    stats->memory.components.allocd -= family_memory;
    stats->memory.families.used += family_memory;
    stats->memory.families.allocd += family_memory;

    stats->entity_count = ecs_map_count(world->entity_index);
    stats->tick_count = world->tick;

    if (world->tick) {
        stats->frame_time = world->frame_time;
        stats->system_time = world->system_time;
    } else {
        stats->frame_time = 0;
        stats->system_time = 0;
    }

    stats->frame_profiling = world->measure_frame_time;
    stats->system_profiling = world->measure_system_time;

    world->tick = 0;
    world->frame_time = 0;
    world->system_time = 0;
}

void ecs_free_stats(
    EcsWorld *world,
    EcsWorldStats *stats)
{
    uint32_t i, count = ecs_array_count(stats->tables);
    EcsTableStats *tables = ecs_array_buffer(stats->tables);
    for (i = 0; i < count; i ++) {
        free(tables[i].columns);
    }

    EcsFeatureStats *entities = ecs_array_buffer(stats->features);
    count = ecs_array_count(stats->features);
    for (i = 0; i < count; i ++) {
        free(entities[i].entities);
    }

    ecs_array_free(stats->tables);
    ecs_array_free(stats->features);
    ecs_array_free(stats->frame_systems);
    ecs_array_free(stats->on_demand_systems);
    ecs_array_free(stats->on_add_systems);
    ecs_array_free(stats->on_set_systems);
    ecs_array_free(stats->on_remove_systems);
}
