#include "flecs_private.h"

const ecs_vector_params_t compstats_arr_params = {
    .element_size = sizeof(EcsComponentStats)
};

const ecs_vector_params_t systemstats_arr_params = {
    .element_size = sizeof(EcsSystemStats)
};

const ecs_vector_params_t featurestats_arr_params = {
    .element_size = sizeof(EcsFeatureStats)
};

static
void calculate_type_stats(
    ecs_world_t *world,
    uint32_t *allocd,
    uint32_t *used)
{
    (void)world;
    (void)allocd;
    (void)used;

    /* TODO: compute memory for type storage */

    /*ecs_map_iter_t it = ecs_map_iter(world->main_stage.type_index);
    while (ecs_map_hasnext(&it)) {
        ecs_vector_t *type = ecs_map_next_ptr(&it);
        ecs_vector_memory(type, &handle_arr_params, allocd, used);
    }*/
}

static
void calculate_system_stats(
    ecs_world_t *world,
    ecs_entity_t system,
    uint32_t *allocd,
    uint32_t *used)
{
    EcsColSystem *sys = ecs_get_ptr(world, system, EcsColSystem);
    if (!sys) {
        return;
    }

    ecs_vector_memory(sys->base.columns, &system_column_params, allocd, used);
    ecs_vector_memory(sys->tables, &matched_table_params, allocd, used);
    ecs_vector_memory(sys->inactive_tables, &matched_table_params, allocd, used);
    ecs_vector_memory(sys->jobs, &job_arr_params, allocd, used);

    int i, count = ecs_vector_count(sys->tables);
    ecs_matched_table_t *tables = ecs_vector_first(sys->tables);

    for (i = 0; i < count; i++) {
        /* For tables[i].columns */
        *allocd += ecs_vector_count(sys->base.columns) * sizeof(uint32_t);
        *used += ecs_vector_count(sys->base.columns) * sizeof(uint32_t);

        /* For tables[i].components */
        *allocd += ecs_vector_count(sys->base.columns) * sizeof(ecs_entity_t);
        *used += ecs_vector_count(sys->base.columns) * sizeof(ecs_entity_t);

        ecs_vector_memory(tables[i].references, &reference_params, allocd, used);
    }
}

static
void calculate_systems_stats(
    ecs_world_t *world,
    ecs_vector_t *systems,
    uint32_t *allocd,
    uint32_t *used)
{
    ecs_entity_t *buffer = ecs_vector_first(systems);
    uint32_t i, count = ecs_vector_count(systems);
    for (i = 0; i < count; i ++) {
        calculate_system_stats(world, buffer[i], allocd, used);
    }
}

static
void calculate_table_stats(
    ecs_world_t *world,
    uint32_t *allocd,
    uint32_t *used)
{
    ecs_chunked_t *tables = world->main_stage.tables;
    uint32_t i, count = ecs_chunked_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, i);
        ecs_vector_memory(table->frame_systems, &handle_arr_params, allocd, used);
        *allocd += ecs_vector_count(table->type) * sizeof(uint16_t);
        *used += ecs_vector_count(table->type) * sizeof(uint16_t);
    }
}

static
void calculate_stage_stats(
    ecs_world_t *world,
    ecs_stage_t *stage,
    uint32_t *allocd,
    uint32_t *used)
{
    bool is_main_stage = stage == &world->main_stage;
    bool is_temp_stage = stage == &world->temp_stage;

    if (!is_main_stage) {
        ecs_map_memory(stage->entity_index, allocd, used);
    }

    if (!is_temp_stage) {
        /* ecs_map_memory(stage->type_index, allocd, used); */
    }
    
    ecs_chunked_memory(stage->tables, allocd, used);
    ecs_map_memory(stage->table_index, allocd, used);

    if (!is_main_stage) {
        ecs_map_memory(stage->remove_merge, allocd, used);
        ecs_map_memory(stage->data_stage, allocd, used);
    }
}

static
void calculate_stages_stats(
    ecs_world_t *world,
    uint32_t *allocd,
    uint32_t *used)
{
    ecs_stage_t *buffer = ecs_vector_first(world->worker_stages);
    uint32_t i, count = ecs_vector_count(world->worker_stages);
    for (i = 0; i < count; i ++) {
        ecs_stage_t *stage = &buffer[i];
        calculate_stage_stats(world, stage, allocd, used);
    }
}

static
void get_memory_stats(
    ecs_world_t *world,
    ecs_world_stats_t *stats)
{
    EcsMemoryStats *memory = &stats->memory;

    ecs_vector_memory(world->add_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->set_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->remove_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_map_memory(world->type_sys_add_index, &memory->systems.allocd, &memory->systems.used);
    ecs_map_memory(world->type_sys_remove_index, &memory->systems.allocd, &memory->systems.used);
    ecs_map_memory(world->type_sys_set_index, &memory->systems.allocd, &memory->systems.used);

    ecs_vector_memory(world->on_load_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->post_load_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->pre_update_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->on_update_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->on_validate_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->post_update_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->pre_store_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->on_store_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->inactive_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);
    ecs_vector_memory(world->on_demand_systems, &handle_arr_params, &memory->systems.allocd, &memory->systems.used);

    calculate_systems_stats(world, world->on_load_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->post_load_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->pre_update_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->on_update_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->on_validate_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->post_update_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->pre_store_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->on_store_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->inactive_systems, &memory->systems.allocd, &memory->systems.used);
    calculate_systems_stats(world, world->on_demand_systems, &memory->systems.allocd, &memory->systems.used);

    ecs_map_memory(world->type_handles, &memory->families.allocd, &memory->families.used);
    calculate_type_stats(world, &memory->families.allocd, &memory->families.used);
    calculate_table_stats(world, &memory->tables.allocd, &memory->tables.used);

    ecs_vector_memory(world->worker_stages, &table_arr_params, &memory->stage.allocd, &memory->stage.used);
    memory->stage.allocd += sizeof(ecs_stage_t);
    memory->stage.used += sizeof(ecs_stage_t);

    calculate_stage_stats(world, &world->main_stage, &memory->world.allocd, &memory->world.used);
    calculate_stage_stats(world, &world->temp_stage, &memory->stage.allocd, &memory->stage.used);
    calculate_stages_stats(world, &memory->stage.allocd, &memory->stage.used);

    ecs_map_memory(world->main_stage.entity_index, &memory->entities.allocd, &memory->entities.used);

    ecs_vector_memory(world->worker_threads, &table_arr_params, &memory->world.allocd, &memory->world.used);
    stats->memory.world.allocd += sizeof(ecs_world_t) - sizeof(ecs_stage_t);
    stats->memory.world.used += sizeof(ecs_world_t) - sizeof(ecs_stage_t);

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
    ecs_world_t *world,
    ecs_vector_t **stats_array,
    ecs_entity_t system,
    bool active)
{
    EcsId *id = ecs_get_ptr(world, system, EcsId);
    EcsSystemStats *sstats = ecs_vector_add(
        stats_array, &systemstats_arr_params);

    sstats->handle = system;
    sstats->id = *id;
    sstats->enabled = ecs_is_enabled(world, system);
    sstats->active = active;
    sstats->is_hidden = ecs_has(world, system, EcsHidden);

    EcsSystem *system_ptr = ecs_get_ptr(world, system, EcsColSystem);
    if (system_ptr) {
        EcsColSystem *col_system = (EcsColSystem*)system_ptr;
        ecs_matched_table_t *tables = ecs_vector_first(col_system->tables);
        uint32_t i, count = ecs_vector_count(col_system->tables);

        sstats->entities_matched = 0;
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = tables[i].table;
            if (table) {
                sstats->entities_matched += ecs_table_count(table);
            }
        }

        sstats->period = col_system->period;
        sstats->tables_matched = count + ecs_vector_count(
            col_system->inactive_tables);
    } else {
        system_ptr = ecs_get_ptr(world, system, EcsRowSystem);
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
    return s1->handle - s2->handle;
}

static
int system_stats_arr(
    ecs_world_t *world,
    ecs_vector_t **stats_array,
    ecs_vector_t *systems)
{
    ecs_entity_t *handles = ecs_vector_first(systems);
    uint32_t i, count = ecs_vector_count(systems);
    for (i = 0; i < count; i ++) {
        set_system_stats(world, stats_array, handles[i], true);
    }

    ecs_vector_sort(*stats_array, &systemstats_arr_params, compare_sysstats);

    return count;
}

static
int system_stats_arr_inactive(
    ecs_world_t *world,
    ecs_world_stats_t *stats)
{
    ecs_vector_t *systems = world->inactive_systems;
    ecs_entity_t *handles = ecs_vector_first(systems);
    uint32_t i, count = ecs_vector_count(systems);
    for (i = 0; i < count; i ++) {
        EcsColSystem *data = ecs_get_ptr(world, handles[i], EcsColSystem);
        ecs_vector_t **stats_array = NULL;

        if (data->base.kind == EcsOnLoad) {
            stats_array = &stats->on_load_systems;
        } else if (data->base.kind == EcsPostLoad) {
            stats_array = &stats->post_load_systems;
        } else if (data->base.kind == EcsPreUpdate) {
            stats_array = &stats->pre_update_systems;
        } else if (data->base.kind == EcsOnUpdate) {
            stats_array = &stats->on_update_systems;
        } else if (data->base.kind == EcsOnValidate) {
            stats_array = &stats->on_validate_systems;
        } else if (data->base.kind == EcsPostUpdate) {
            stats_array = &stats->post_update_systems;
        } else if (data->base.kind == EcsPreStore) {
            stats_array = &stats->pre_store_systems;
        } else if (data->base.kind == EcsOnStore) {
            stats_array = &stats->on_store_systems;
        } else if (data->base.kind == EcsManual) {
            stats_array = &stats->on_demand_systems;
        }

        set_system_stats(world, stats_array, handles[i], false);
    }

    return count;
}

static
void add_component_measurement(
    ecs_world_t *world,
    ecs_world_stats_t *stats,
    ecs_entity_t component,
    ecs_table_column_t *column,
    uint32_t *memory_allocd,
    uint32_t *memory_used)
{
    if (!column->data) {
        return;
    }

    uint32_t i, count = ecs_vector_count(stats->components);
    EcsComponentStats *c = ecs_vector_first(stats->components);

    for (i = 0; i < count; i ++) {
        if (c[i].handle == component) {
            break;
        }
    }

    if (i == count) {
        EcsComponent *c_data = ecs_get_ptr(world, component, EcsComponent);
        if (c_data) {

            ecs_vector_add(&stats->components, &compstats_arr_params);
            c = ecs_vector_first(stats->components);

            c[i].handle = component;
            c[i].id = ecs_get_id(world, component);
            c[i].memory_allocd = 0;
            c[i].memory_used = 0;
            c[i].entities = 0;
            c[i].tables = 0;

            c[i].size = c_data->size;
        } else {
            return;
        }
    }

    ecs_vector_params_t param = {.element_size = c[i].size};
    ecs_vector_memory(column->data, &param, &c[i].memory_allocd, &c[i].memory_used);
    ecs_vector_memory(column->data, &param, memory_allocd, memory_used);

    c[i].entities += ecs_vector_count(column->data);
    c[i].tables ++;
}

static
void collect_comp_stats(
    ecs_world_t *world,
    ecs_world_stats_t *stats,
    uint32_t *memory_allocd,
    uint32_t *memory_used)
{
    ecs_chunked_t *tables = world->main_stage.tables;
    uint32_t i, count = ecs_chunked_count(world->main_stage.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, i);
        ecs_entity_t *components = ecs_vector_first(table->type);

        uint32_t c, c_count = ecs_vector_count(table->type);

        for (c = 0; c < c_count; c ++) {
            add_component_measurement(world, stats, components[c], 
                    &table->columns[c], memory_allocd, memory_used);
        }
    }
}

void ecs_get_stats(
    ecs_world_t *world,
    ecs_world_stats_t *stats)
{
    uint32_t mem_used = 0, mem_allocd = 0;

    if (!stats->components) {
        stats->components = ecs_vector_new(&compstats_arr_params, stats->component_count);
    } else {
        ecs_vector_clear(stats->components);
    }

    collect_comp_stats(world, stats, &mem_allocd, &mem_used);

    stats->component_count = ecs_vector_count(stats->components);
    stats->table_count = ecs_chunked_count(world->main_stage.tables);

    if (!stats->features) {
        stats->features = ecs_vector_new(&featurestats_arr_params, 0);
    } else {
        ecs_vector_clear(stats->features);
    }

    ecs_map_iter_t it = ecs_map_iter(world->type_handles);
    while (ecs_map_hasnext(&it)) {
        ecs_entity_t h = *(ecs_entity_t*)ecs_map_next_w_key(&it, NULL);
        EcsTypeComponent *data = ecs_get_ptr(world, h, EcsTypeComponent);
        ecs_entity_t *buffer = ecs_vector_first(data->resolved);
        uint32_t i, count = ecs_vector_count(data->resolved);

        EcsFeatureStats feature = {0};
        for (i = 0; i < count; i ++) {
            if (ecs_has(world, buffer[i], EcsColSystem)) {
                feature.system_count ++;
                if (ecs_is_enabled(world, buffer[i])) {
                    feature.systems_enabled ++;
                }
            }
            if (ecs_has(world, buffer[i], EcsRowSystem)) {
                feature.system_count ++;
                feature.systems_enabled ++;
            }
        }

        if (feature.system_count) {
            EcsFeatureStats *elem = ecs_vector_add(
                &stats->features, &featurestats_arr_params);
            *elem = feature;
            elem->id = ecs_get_id(world, h);
            elem->entities = ecs_type_to_expr(world, data->type);
            elem->is_hidden = ecs_has(world, h, EcsHidden);
        }
    }

    stats->system_count = 0;
    stats->system_count += system_stats_arr_inactive(world, stats);
    stats->system_count += system_stats_arr(
        world, &stats->on_load_systems, world->on_load_systems);
    stats->system_count += system_stats_arr(
        world, &stats->post_load_systems, world->post_load_systems);        
    stats->system_count += system_stats_arr(
        world, &stats->pre_update_systems, world->pre_update_systems);
    stats->system_count += system_stats_arr(
        world, &stats->on_update_systems, world->on_update_systems);
    stats->system_count += system_stats_arr(
        world, &stats->on_validate_systems, world->on_validate_systems);        
    stats->system_count += system_stats_arr(
        world, &stats->post_update_systems, world->post_update_systems);
    stats->system_count += system_stats_arr(
        world, &stats->pre_store_systems, world->pre_store_systems);
    stats->system_count += system_stats_arr(
        world, &stats->on_store_systems, world->on_store_systems);
    stats->system_count += system_stats_arr(
        world, &stats->on_demand_systems, world->on_demand_systems);

    uint32_t table_sys_count = stats->system_count;

    stats->system_count += system_stats_arr(
        world, &stats->on_add_systems, world->add_systems);
    stats->system_count += system_stats_arr(
        world, &stats->on_set_systems, world->set_systems);
    stats->system_count += system_stats_arr(
        world, &stats->on_remove_systems, world->remove_systems);

    uint32_t row_sys_count = stats->system_count - table_sys_count;

    stats->memory.components.used = mem_used;
    stats->memory.components.allocd = mem_allocd;
    get_memory_stats(world, stats);

    uint32_t system_memory =
      table_sys_count * (sizeof(EcsColSystem) + sizeof(EcsId)) +
      row_sys_count * (sizeof(EcsRowSystem) + sizeof(EcsId));
    stats->memory.components.used -= system_memory;
    stats->memory.components.allocd -= system_memory;
    stats->memory.systems.used += system_memory;
    stats->memory.systems.allocd += system_memory;

/*     uint32_t type_memory = ecs_map_count(world->main_stage.type_index) *
      (sizeof(EcsTypeComponent) + sizeof(EcsId));
    stats->memory.components.used -= type_memory;
    stats->memory.components.allocd -= type_memory;
    stats->memory.families.used += type_memory;
    stats->memory.families.allocd += type_memory; */

    stats->entity_count = ecs_map_count(world->main_stage.entity_index);
    stats->tick_count = world->tick;

    if (world->tick) {
        stats->frame_time = world->frame_time;
        stats->system_time = world->system_time;
        stats->merge_time = world->merge_time;
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
    ecs_world_stats_t *stats)
{
    EcsFeatureStats *entities = ecs_vector_first(stats->features);
    uint32_t i, count = ecs_vector_count(stats->features);
    for (i = 0; i < count; i ++) {
        ecs_os_free(entities[i].entities);
    }

    ecs_vector_free(stats->components);
    ecs_vector_free(stats->features);
    ecs_vector_free(stats->on_load_systems);
    ecs_vector_free(stats->post_load_systems);
    ecs_vector_free(stats->pre_update_systems);
    ecs_vector_free(stats->on_update_systems);
    ecs_vector_free(stats->on_validate_systems);
    ecs_vector_free(stats->post_update_systems);
    ecs_vector_free(stats->pre_store_systems);
    ecs_vector_free(stats->on_store_systems);
    ecs_vector_free(stats->on_demand_systems);
    ecs_vector_free(stats->on_add_systems);
    ecs_vector_free(stats->on_set_systems);
    ecs_vector_free(stats->on_remove_systems);
}
