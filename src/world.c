#include "flecs_private.h"

/* -- Global array parameters -- */

const ecs_vector_params_t table_arr_params = {
    .element_size = sizeof(ecs_table_t)
};

const ecs_vector_params_t handle_arr_params = {
    .element_size = sizeof(ecs_entity_t)
};

const ecs_vector_params_t stage_arr_params = {
    .element_size = sizeof(ecs_stage_t)
};

const ecs_vector_params_t builder_params = {
    .element_size = sizeof(ecs_builder_op_t)
};

const ecs_vector_params_t ptr_params = {
    .element_size = sizeof(void*)
};

/* -- Global variables -- */

ecs_type_t TEcsComponent;
ecs_type_t TEcsTypeComponent;
ecs_type_t TEcsPrefab;
ecs_type_t TEcsPrefabParent;
ecs_type_t TEcsPrefabBuilder;
ecs_type_t TEcsRowSystem;
ecs_type_t TEcsColSystem;
ecs_type_t TEcsId;
ecs_type_t TEcsHidden;
ecs_type_t TEcsDisabled;
ecs_type_t TEcsOnDemand;

const char *ECS_COMPONENT_ID =      "EcsComponent";
const char *ECS_TYPE_COMPONENT_ID = "EcsTypeComponent";
const char *ECS_PREFAB_ID =         "EcsPrefab";
const char *ECS_PREFAB_PARENT_ID =  "EcsPrefabParent";
const char *ECS_PREFAB_BUILDER_ID = "EcsPrefabBuilder";
const char *ECS_ROW_SYSTEM_ID =     "EcsRowSystem";
const char *ECS_COL_SYSTEM_ID =     "EcsColSystem";
const char *ECS_ID_ID =             "EcsId";
const char *ECS_HIDDEN_ID =         "EcsHidden";
const char *ECS_DISABLED_ID =       "EcsDisabled";
const char *ECS_ON_DEMAND_ID =      "EcsOnDemand";

/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    return *(ecs_entity_t*)p1 - *(ecs_entity_t*)p2;
}

static
ecs_table_t* get_table(
    ecs_stage_t *stage,
    ecs_type_t type)
{
    ecs_table_t* result;

    if (ecs_map_has(stage->table_index, (uintptr_t)type, &result)) {
        return result;
    } else {
        return 0;
    }
}

static
void set_table(
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_table_t *table)
{
    ecs_map_set(stage->table_index, (uintptr_t)type, &table);
}

/** Bootstrap builtin component types and commonly used types */
static
void bootstrap_types(
    ecs_world_t *world)
{
    ecs_stage_t *stage = &world->main_stage;
    
    TEcsComponent = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsComponent}, 1);
    TEcsTypeComponent = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsTypeComponent}, 1);
    TEcsPrefab = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsPrefab}, 1);
    TEcsPrefabParent = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsPrefabParent}, 1);
    TEcsPrefabBuilder = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsPrefabBuilder}, 1);
    TEcsRowSystem = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsRowSystem}, 1);
    TEcsColSystem = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsColSystem}, 1);
    TEcsId = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsId}, 1);
    TEcsHidden = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsHidden}, 1);
    TEcsDisabled = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsDisabled}, 1);
    TEcsOnDemand = ecs_type_find_intern(world, stage, &(ecs_entity_t){EEcsOnDemand}, 1);

    world->t_component = ecs_type_merge_intern(world, stage, TEcsComponent, TEcsId, 0);
    world->t_type = ecs_type_merge_intern(world, stage, TEcsTypeComponent, TEcsId, 0);
    world->t_prefab = ecs_type_merge_intern(world, stage, TEcsPrefab, TEcsId, 0);
    world->t_row_system = ecs_type_merge_intern(world, stage, TEcsRowSystem, TEcsId, 0);
    world->t_col_system = ecs_type_merge_intern(world, stage, TEcsColSystem, TEcsId, 0);

    ecs_assert(ecs_vector_count(world->t_component) == 2, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(world->t_type) == 2, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(world->t_prefab) == 2, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(world->t_row_system) == 2, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(world->t_col_system) == 2, ECS_INTERNAL_ERROR, NULL);
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. */
static
ecs_table_t* bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_assert(world->t_component != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_stage_t *stage = &world->main_stage;
    ecs_table_t *result = ecs_chunked_add(stage->tables, ecs_table_t);
    result->type = world->t_component;
    result->frame_systems = NULL;
    result->flags = 0;
    result->flags |= EcsTableHasBuiltins;
    result->columns = ecs_os_malloc(sizeof(ecs_table_column_t) * 3);
    ecs_assert(result->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->columns[0].data = ecs_vector_new(&handle_arr_params, 16);
    result->columns[0].size = sizeof(ecs_entity_t);
    result->columns[1].data = ecs_vector_new(&handle_arr_params, 16);
    result->columns[1].size = sizeof(EcsComponent);
    result->columns[2].data = ecs_vector_new(&handle_arr_params, 16);
    result->columns[2].size = sizeof(EcsId);

    set_table(stage, world->t_component, result);

    return result;
}

/** Bootstrap the EcsComponent component */
static
void bootstrap_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *id,
    size_t size)
{
    ecs_stage_t *stage = &world->main_stage;

    /* Insert row into table to store EcsComponent itself */
    int32_t index = ecs_table_insert(world, table, table->columns, entity);

    /* Create record in entity index */
    ecs_row_t row = {.type = world->t_component, .index = index};
    ecs_map_set(stage->entity_index, entity, &row);

    /* Set size and id */
    EcsComponent *component_data = ecs_vector_first(table->columns[1].data);
    EcsId *id_data = ecs_vector_first(table->columns[2].data);
    
    component_data[index - 1].size = size;
    id_data[index - 1] = id;
}

static
void notify_create_table(
    ecs_world_t *world,
    ecs_vector_t *systems,
    ecs_table_t *table)
{
    ecs_entity_t *buffer = ecs_vector_first(systems);
    uint32_t i, count = ecs_vector_count(systems);

    for (i = 0; i < count; i ++) {
        ecs_col_system_notify_of_table(world, buffer[i], table);
    }
}

void ecs_notify_systems_of_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    notify_create_table(world, world->pre_update_systems, table);
    notify_create_table(world, world->post_update_systems, table);
    notify_create_table(world, world->on_load_systems, table);
    notify_create_table(world, world->post_load_systems, table);
    notify_create_table(world, world->pre_store_systems, table);
    notify_create_table(world, world->on_store_systems, table);
    notify_create_table(world, world->on_validate_systems, table);
    notify_create_table(world, world->on_update_systems, table);
    notify_create_table(world, world->manual_systems, table);
    notify_create_table(world, world->inactive_systems, table);
}

/** Create a new table and register it with the world and systems. A table in
 * flecs is equivalent to an archetype */
static
ecs_table_t* create_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type)
{
    /* Add and initialize table */
    ecs_table_t *result = ecs_chunked_add(stage->tables, ecs_table_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
    
    result->type = type;

    ecs_table_init(world, stage, result);

    if (world->in_progress) {
        result->flags |= EcsTableIsStaged;
    }

    set_table(stage, type, result);

    if (stage == &world->main_stage && !world->is_merging) {
        ecs_notify_systems_of_table(world, result);
    }

    return result;
}

#ifndef NDEBUG
static
void no_threading(
    const char *function)
{
    ecs_os_dbg("threading unavailable: %s not implemented", function);
}

static
void no_time(
    const char *function)
{
    ecs_os_dbg("time management: %s not implemented", function);
}
#endif

/* -- Private functions -- */

/** Find or create table from type */
ecs_table_t* ecs_world_get_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type)
{
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(type) < ECS_MAX_ENTITIES_IN_TYPE, ECS_INTERNAL_ERROR, NULL);

    ecs_stage_t *main_stage = &world->main_stage;
    ecs_table_t* table = get_table(main_stage, type);

    if (!table && world->in_progress) {
        assert(stage != NULL);
        table = get_table(stage, type);
    }

    if (!table) {
        table = create_table(world, stage, type);
    }

    return table;
}

ecs_vector_t** ecs_system_array(
    ecs_world_t *world,
    EcsSystemKind kind)
{
    ecs_assert(kind == EcsOnUpdate ||
               kind == EcsOnValidate ||
               kind == EcsPreUpdate ||
               kind == EcsPostUpdate ||
               kind == EcsPostLoad ||
               kind == EcsOnLoad ||
               kind == EcsPreStore ||
               kind == EcsOnStore ||
               kind == EcsManual,
               ECS_INTERNAL_ERROR,
               NULL);

    if (kind == EcsOnUpdate) {
        return &world->on_update_systems;
    } else if (kind == EcsOnValidate) {
        return &world->on_validate_systems;
    } else if (kind == EcsPreUpdate) {
        return &world->pre_update_systems;
    } else if (kind == EcsPostUpdate) {
        return &world->post_update_systems;
    } else if (kind == EcsPostLoad) {
        return &world->post_load_systems;
    } else if (kind == EcsOnLoad) {
        return &world->on_load_systems;
    } else if (kind == EcsPreStore) {
        return &world->pre_store_systems;        
    } else if (kind == EcsOnStore) {
        return &world->on_store_systems;
    } else if (kind == EcsManual) {
        return &world->manual_systems;        
    }
    
    return NULL;
}

/** Inactive systems are systems that either:
 * - are not enabled
 * - matched with no tables
 * - matched with only empty tables.
 *
 * These systems are not considered in the main loop, which can speed up things
 * when applications contain large numbers of disabled systems.
 */
void ecs_world_activate_system(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystemKind kind,
    bool active)
{
    ecs_vector_t *src_array, *dst_array;

    if (active) {
        src_array = world->inactive_systems;
        dst_array = *ecs_system_array(world, kind);
     } else {
        src_array = *ecs_system_array(world, kind);
        dst_array = world->inactive_systems;
    }

    uint32_t i, count = ecs_vector_count(src_array);
    for (i = 0; i < count; i ++) {
        ecs_entity_t *h = ecs_vector_get(
            src_array, &handle_arr_params, i);
        if (*h == system) {
            break;
        }
    }

    if (i == count) {
        return; /* System is already in the right array */
    }

    ecs_vector_move_index(
        &dst_array, src_array, &handle_arr_params, i);

    ecs_entity_t *to_sort;
    uint32_t sort_count;

    if (active) {
         *ecs_system_array(world, kind) = dst_array;
         to_sort = ecs_vector_first(dst_array);
         sort_count = ecs_vector_count(dst_array);
    } else {
        world->inactive_systems = dst_array;
        to_sort = ecs_vector_first(src_array);
        sort_count = ecs_vector_count(src_array);        
    } 

    qsort(to_sort, sort_count, sizeof(ecs_entity_t), compare_handle);    

    /* Signal that system has been either activated or deactivated */
    ecs_system_activate(world, system, active);

    return;
}

ecs_stage_t *ecs_get_stage(
    ecs_world_t **world_ptr)
{
    ecs_world_t *world = *world_ptr;

    ecs_assert(world->magic == ECS_WORLD_MAGIC ||
               world->magic == ECS_THREAD_MAGIC,
               ECS_INTERNAL_ERROR,
               NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        if (world->in_progress) {
            return &world->temp_stage;
        } else {
            return &world->main_stage;
        }
    } else if (world->magic == ECS_THREAD_MAGIC) {
        ecs_thread_t *thread = (ecs_thread_t*)world;
        *world_ptr = thread->world;
        return thread->stage;
    }
    
    return NULL;
}

static
void col_systems_deinit_handlers(
    ecs_world_t *world,
    ecs_vector_t *systems)
{
    uint32_t i, count = ecs_vector_count(systems);
    ecs_entity_t *buffer = ecs_vector_first(systems);

    for (i = 0; i < count; i ++) {
        EcsColSystem *ptr = ecs_get_ptr(world, buffer[i], EcsColSystem);

        /* Invoke Deactivated action for active systems */
        if (ecs_vector_count(ptr->tables)) {
            ecs_invoke_status_action(world, buffer[i], ptr, EcsSystemDeactivated);
        }

        /* Invoke Disabled action for enabled systems */
        if (ptr->base.enabled) {
            ecs_invoke_status_action(world, buffer[i], ptr, EcsSystemDisabled);
        }    
    }
}

static
void col_systems_deinit(
    ecs_world_t *world,
    ecs_vector_t *systems)
{
    uint32_t i, count = ecs_vector_count(systems);
    ecs_entity_t *buffer = ecs_vector_first(systems);

    for (i = 0; i < count; i ++) {
        EcsColSystem *ptr = ecs_get_ptr(world, buffer[i], EcsColSystem);

        ecs_os_free(ptr->base.signature);

        ecs_vector_free(ptr->base.columns);
        ecs_vector_free(ptr->jobs);

        uint32_t t;
        ecs_matched_table_t *tables = ecs_vector_first(ptr->inactive_tables);
        for (t = 0; t < ecs_vector_count(ptr->inactive_tables); t ++) {
            ecs_os_free(tables[t].columns);
            ecs_os_free(tables[t].components);
        }

        tables = ecs_vector_first(ptr->tables);
        for (t = 0; t < ecs_vector_count(ptr->tables); t ++) {
            ecs_os_free(tables[t].columns);
            ecs_os_free(tables[t].components);
        }

        ecs_vector_free(ptr->inactive_tables);
        ecs_vector_free(ptr->tables);
    }
}

static
void row_systems_deinit(
    ecs_world_t *world,
    ecs_vector_t *systems)
{
    uint32_t i, count = ecs_vector_count(systems);
    ecs_entity_t *buffer = ecs_vector_first(systems);

    for (i = 0; i < count; i ++) {
        EcsRowSystem *ptr = ecs_get_ptr(world, buffer[i], EcsRowSystem);
        ecs_os_free(ptr->base.signature);
        ecs_vector_free(ptr->base.columns);
        ecs_vector_free(ptr->components);
    }
}

static
void row_index_deinit(
    ecs_map_t *sys_index)
{
    ecs_map_iter_t it = ecs_map_iter(sys_index);

    while (ecs_map_hasnext(&it)) {
        ecs_vector_t *v = ecs_map_nextptr(&it);
        ecs_vector_free(v);
    }

    ecs_map_free(sys_index);
}

static
void deinit_tables(
    ecs_world_t *world)
{
    ecs_chunked_t *tables = world->main_stage.tables;
    uint32_t i, count = ecs_chunked_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, i);
        ecs_table_deinit(world, table);
    }
}

static
ecs_entity_t get_prefab_parent_flag(
    ecs_world_t *world,
    ecs_entity_t prefab)
{
    ecs_entity_t flag;
    if (!ecs_map_has(world->prefab_parent_index, prefab, &flag)) {
        flag = ecs_new(world, EcsPrefabParent);
        ecs_set(world, flag, EcsPrefabParent, {.parent = prefab});
        ecs_map_set(world->prefab_parent_index, prefab, &flag);
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
        builder->ops = ecs_vector_new(&builder_params, 1);
    }

    ecs_type_t type = NULL;
    if (ecs_has(world, child, EcsTypeComponent)) {
        type = ecs_type_from_entity(world, child); 

        ecs_assert(type != NULL, ECS_INVALID_PARAMETER, NULL);

        ecs_entity_t *array = ecs_vector_first(type);
        (void)array;
        uint32_t count = ecs_vector_count(type);
        (void)count;

        ecs_assert((array[count - 1] & ECS_INSTANCEOF) != 0, 
            ECS_INVALID_PREFAB_CHILD_TYPE, NULL);
    } else {
        ecs_entity_t array[] = {
            child | ECS_INSTANCEOF,
            EEcsId
        };

        type = ecs_type_find(world, array, 2);
    }

    /* If there are no child ops, this is the first time that this child is
     * added to this parent prefab. Simply add it to the vector */
    ecs_builder_op_t *op = ecs_vector_add(&builder->ops, &builder_params);
    op->id = ecs_get_id(world, child);
    op->type = type;
}

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

        bool prefab_parent_flag_added = false;
        bool prefab_parent_added = false;

        /* Walk components of entity, find prefab */
        for (t = 0; t < t_count; t++) {
            EcsPrefabParent *pparent;

            ecs_entity_t component = type[t];

            if (parent != component) {
                if ((pparent = ecs_get_ptr(world, component, EcsPrefabParent))) {                    
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

            ecs_type_t flag_type = ecs_type_from_entity(world, flag);
            _ecs_add(world, e, flag_type);
        }
    }
}


/* -- Public functions -- */

ecs_world_t *ecs_init(void) {
    ecs_os_set_api_defaults();

#ifdef __BAKE__
    ut_init(NULL);
    if (ut_load_init(NULL, NULL, NULL, NULL)) {
        ecs_os_err("warning: failed to initialize package loader");
    }
#endif

    ecs_assert(ecs_os_api.malloc != NULL, ECS_MISSING_OS_API, "malloc");
    ecs_assert(ecs_os_api.realloc != NULL, ECS_MISSING_OS_API, "realloc");
    ecs_assert(ecs_os_api.calloc != NULL, ECS_MISSING_OS_API, "calloc");

    bool time_ok = true;

#ifndef NDEBUG
    bool thr_ok = true;
    if (!ecs_os_api.mutex_new) {thr_ok = false; no_threading("mutex_new");}
    if (!ecs_os_api.mutex_free) {thr_ok = false; no_threading("mutex_free");}
    if (!ecs_os_api.mutex_lock) {thr_ok = false; no_threading("mutex_lock");}
    if (!ecs_os_api.mutex_unlock) {thr_ok = false; no_threading("mutex_unlock");}
    if (!ecs_os_api.cond_new) {thr_ok = false; no_threading("cond_new");}
    if (!ecs_os_api.cond_free) {thr_ok = false; no_threading("cond_free");}
    if (!ecs_os_api.cond_wait) {thr_ok = false; no_threading("cond_wait");}
    if (!ecs_os_api.cond_signal) {thr_ok = false; no_threading("cond_signal");}
    if (!ecs_os_api.cond_broadcast) {thr_ok = false; no_threading("cond_broadcast"); }
    if (!ecs_os_api.thread_new) {thr_ok = false; no_threading("thread_new");}
    if (!ecs_os_api.thread_join) {thr_ok = false; no_threading("thread_join");}
    if (thr_ok) {
        ecs_os_dbg("threading available");
    }

    if (!ecs_os_api.get_time) {time_ok = false; no_time("get_time");}
    if (!ecs_os_api.sleep) {time_ok = false; no_time("sleep");}
    if (time_ok) {
        ecs_os_dbg("time management available");
    }
#endif

    ecs_world_t *world = ecs_os_malloc(sizeof(ecs_world_t));
    ecs_assert(world != NULL, ECS_OUT_OF_MEMORY, NULL);

    world->magic = ECS_WORLD_MAGIC;

    world->on_update_systems = ecs_vector_new(&handle_arr_params, 0);
    world->on_validate_systems = ecs_vector_new(&handle_arr_params, 0);
    world->pre_update_systems = ecs_vector_new(&handle_arr_params, 0);
    world->post_update_systems = ecs_vector_new(&handle_arr_params, 0);
    world->post_load_systems = ecs_vector_new(&handle_arr_params, 0);
    world->on_load_systems = ecs_vector_new(&handle_arr_params, 0);
    world->pre_store_systems = ecs_vector_new( &handle_arr_params, 0);
    world->on_store_systems = ecs_vector_new( &handle_arr_params, 0);
    world->inactive_systems = ecs_vector_new(&handle_arr_params, 0);
    world->manual_systems = ecs_vector_new(&handle_arr_params, 0);

    world->add_systems = ecs_vector_new(&handle_arr_params, 0);
    world->remove_systems = ecs_vector_new(&handle_arr_params, 0);
    world->set_systems = ecs_vector_new(&handle_arr_params, 0);
    world->fini_tasks = ecs_vector_new(&handle_arr_params, 0);

    world->type_sys_add_index = ecs_map_new(0, sizeof(ecs_vector_t*));
    world->type_sys_remove_index = ecs_map_new(0, sizeof(ecs_vector_t*));
    world->type_sys_set_index = ecs_map_new(0, sizeof(ecs_vector_t*));
    world->type_handles = ecs_map_new(0, sizeof(ecs_entity_t));
    world->prefab_parent_index = ecs_map_new(0, sizeof(ecs_entity_t));
    world->on_activate_components = ecs_map_new(0, sizeof(ecs_on_demand_in_t));
    world->on_enable_components = ecs_map_new(0, sizeof(ecs_on_demand_in_t));

    world->worker_stages = NULL;
    world->worker_threads = NULL;
    world->jobs_finished = 0;
    world->threads_running = 0;
    world->valid_schedule = false;
    world->quit_workers = false;
    world->in_progress = false;
    world->is_merging = false;
    world->auto_merge = true;
    world->measure_frame_time = false;
    world->measure_system_time = false;
    world->last_handle = 0;
    world->should_quit = false;
    world->should_match = false;

    world->frame_start_time = (ecs_time_t){0, 0};
    if (time_ok) {
        ecs_os_get_time(&world->world_start_time);
    }
    world->target_fps = 0;
    world->fps_sleep = 0;

    world->frame_time_total = 0;
    world->system_time_total = 0;
    world->merge_time_total = 0;
    world->frame_count_total = 0;
    world->world_time_total = 0;

    world->context = NULL;

    world->arg_fps = 0;
    world->arg_threads = 0;

    ecs_stage_init(world, &world->main_stage);
    ecs_stage_init(world, &world->temp_stage);

    /* Initialize types for builtin types */
    bootstrap_types(world);

    /* Create table that will hold components (EcsComponent, EcsId) */
    ecs_table_t *table = bootstrap_component_table(world);
    assert(table != NULL);

    /* Create records for internal components */
    bootstrap_component(world, table, EEcsComponent, ECS_COMPONENT_ID, sizeof(EcsComponent));
    bootstrap_component(world, table, EEcsTypeComponent, ECS_TYPE_COMPONENT_ID, sizeof(EcsTypeComponent));
    bootstrap_component(world, table, EEcsPrefab, ECS_PREFAB_ID, sizeof(EcsPrefab));
    bootstrap_component(world, table, EEcsPrefabParent, ECS_PREFAB_PARENT_ID, sizeof(EcsPrefabParent));
    bootstrap_component(world, table, EEcsPrefabBuilder, ECS_PREFAB_BUILDER_ID, sizeof(EcsPrefabBuilder));
    bootstrap_component(world, table, EEcsRowSystem, ECS_ROW_SYSTEM_ID, sizeof(EcsRowSystem));
    bootstrap_component(world, table, EEcsColSystem, ECS_COL_SYSTEM_ID, sizeof(EcsColSystem));
    bootstrap_component(world, table, EEcsId, ECS_ID_ID, sizeof(EcsId));
    bootstrap_component(world, table, EEcsHidden, ECS_HIDDEN_ID, 0);
    bootstrap_component(world, table, EEcsDisabled, ECS_DISABLED_ID, 0);
    bootstrap_component(world, table, EEcsOnDemand, ECS_ON_DEMAND_ID, 0);

    world->last_handle = EcsWorld + 1;
    world->min_handle = 0;
    world->max_handle = 0;

    ecs_new_system(world, "EcsInitPrefab", EcsOnAdd, "EcsPrefab", EcsInitPrefab);
    ecs_new_system(world, "EcsSetPrefab", EcsOnSet, "EcsPrefab", EcsSetPrefab);

    /* Create type that allows for quickly checking if a type contains builtin
     * components. */
    world->t_builtins = ecs_expr_to_type(world,
        "EcsComponent, EcsTypeComponent, EcsPrefab, EcsPrefabParent"
        ", EcsPrefabBuilder, EcsRowSystem, EcsColSystem");

    /* Initialize EcsWorld */
    ecs_set(world, EcsWorld, EcsId, {"EcsWorld"});

    return world;
}

#define ARG(short, long, action)\
    if (i < argc) {\
        if (argv[i][0] == '-') {\
            if (argv[i][1] == '-') {\
                if (long && !strcmp(&argv[i][2], long ? long : "")) {\
                    action;\
                    parsed = true;\
                }\
            } else {\
                if (short && argv[i][1] == short) {\
                    action;\
                    parsed = true;\
                }\
            }\
        }\
    }

ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[])
{
    /* First parse debug argument so logging is enabled while initializing world */ 
    int i;
    for (i = 1; i < argc; i ++) {
        if (argv[i][0] == '-') {
            bool parsed = false;

            ARG(0, "debug", ecs_os_enable_dbg(true));

            /* Ignore arguments that were not parsed */
            (void)parsed;
        } else {
            /* Ignore arguments that don't start with '-' */
        }
    }

    ecs_world_t *world = ecs_init();
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Parse remaining arguments */
    for (i = 1; i < argc; i ++) {
        if (argv[i][0] == '-') {
            bool parsed = false;
            int threads;
            
            ARG(0, "threads", 
                threads = atoi(argv[i + 1]); 
                ecs_set_threads(world, threads);
                i ++;
            );

            ARG(0, "fps", 
                ecs_set_target_fps(world, atoi(argv[i + 1]));
                world->arg_fps = world->target_fps; 
                i ++);

            ARG(0, "admin", 
				ecs_enable_admin(world, atoi(argv[i + 1]));
                i ++);

            ARG(0, "console", ecs_enable_console(world));

            /* Ignore arguments that were not parsed */
            (void)parsed;
        } else {
            /* Ignore arguments that don't start with '-' */
        }
    }
    
    return world;
}

static
void on_demand_in_map_deinit(
    ecs_map_t *map)
{
    ecs_map_iter_t it = ecs_map_iter(map);

    while (ecs_map_hasnext(&it)) {
        ecs_on_demand_in_t *elem = ecs_map_next(&it);
        ecs_vector_free(elem->systems);
    }

    ecs_map_free(map);
}

int ecs_fini(
    ecs_world_t *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    assert(!world->in_progress);
    assert(!world->is_merging);

    uint32_t i, system_count = ecs_vector_count(world->fini_tasks);
    if (system_count) {
        ecs_entity_t *buffer = ecs_vector_first(world->fini_tasks);
        for (i = 0; i < system_count; i ++) {
            ecs_run_task(world, buffer[i]);
        }
    }

    if (world->worker_threads) {
        ecs_set_threads(world, 0);
    }

    deinit_tables(world);

    col_systems_deinit_handlers(world, world->on_update_systems);
    col_systems_deinit_handlers(world, world->on_validate_systems);
    col_systems_deinit_handlers(world, world->pre_update_systems);
    col_systems_deinit_handlers(world, world->post_update_systems);
    col_systems_deinit_handlers(world, world->on_load_systems);
    col_systems_deinit_handlers(world, world->post_load_systems);
    col_systems_deinit_handlers(world, world->pre_store_systems);
    col_systems_deinit_handlers(world, world->on_store_systems);
    col_systems_deinit_handlers(world, world->manual_systems);
    col_systems_deinit_handlers(world, world->inactive_systems);

    col_systems_deinit(world, world->on_update_systems);
    col_systems_deinit(world, world->on_validate_systems);
    col_systems_deinit(world, world->pre_update_systems);
    col_systems_deinit(world, world->post_update_systems);
    col_systems_deinit(world, world->on_load_systems);
    col_systems_deinit(world, world->post_load_systems);
    col_systems_deinit(world, world->pre_store_systems);
    col_systems_deinit(world, world->on_store_systems);
    col_systems_deinit(world, world->manual_systems);
    col_systems_deinit(world, world->inactive_systems);

    row_systems_deinit(world, world->add_systems);
    row_systems_deinit(world, world->remove_systems);
    row_systems_deinit(world, world->set_systems);

    row_index_deinit(world->type_sys_add_index);
    row_index_deinit(world->type_sys_remove_index);
    row_index_deinit(world->type_sys_set_index);
    ecs_map_free(world->type_handles);
    ecs_map_free(world->prefab_parent_index);

    ecs_stage_deinit(world, &world->main_stage);
    ecs_stage_deinit(world, &world->temp_stage);

    on_demand_in_map_deinit(world->on_activate_components);
    on_demand_in_map_deinit(world->on_enable_components);

    ecs_vector_free(world->on_update_systems);
    ecs_vector_free(world->on_validate_systems);
    ecs_vector_free(world->pre_update_systems);
    ecs_vector_free(world->post_update_systems);
    ecs_vector_free(world->on_load_systems);
    ecs_vector_free(world->post_load_systems);
    ecs_vector_free(world->pre_store_systems);
    ecs_vector_free(world->on_store_systems);

    ecs_vector_free(world->inactive_systems);
    ecs_vector_free(world->manual_systems);
    ecs_vector_free(world->fini_tasks);

    ecs_vector_free(world->add_systems);
    ecs_vector_free(world->remove_systems);
    ecs_vector_free(world->set_systems);



    world->magic = 0;

    ecs_os_free(world);

#ifdef __BAKE__
    ut_deinit();
#endif    

    return 0;
}

void ecs_dim(
    ecs_world_t *world,
    uint32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    ecs_map_set_size(world->main_stage.entity_index, entity_count);
}

void _ecs_dim_type(
    ecs_world_t *world,
    ecs_type_t type,
    uint32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    if (type) {
        ecs_table_t *table = ecs_world_get_table(world, &world->main_stage, type);
        if (table) {
            ecs_table_dim(table, NULL, entity_count);
        }
    }
}

static
ecs_entity_t ecs_lookup_child_in_columns(
    ecs_type_t type,
    ecs_table_column_t *columns,
    ecs_entity_t parent,
    const char *id)
{
    int16_t column_index;

    if ((column_index = ecs_type_index_of(type, EEcsId)) == -1) {
        return 0;
    }

    if (parent && ecs_type_index_of(type, parent) == -1) {
        return 0;
    }

    ecs_table_column_t *column = &columns[column_index + 1];
    EcsId *buffer = ecs_vector_first(column->data);
    uint32_t i, count = ecs_vector_count(column->data);
    
    for (i = 0; i < count; i ++) {
        if (!buffer[i]) {
            continue;
        }
        
        if (!strcmp(buffer[i], id)) {
            return *(ecs_entity_t*)ecs_vector_get(
                columns[0].data, &handle_arr_params, i);
        }
    }

    return 0;
}

ecs_entity_t ecs_lookup_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *id)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_t result = 0;

    if (stage != &world->main_stage) {
        ecs_map_iter_t it = ecs_map_iter(stage->data_stage);

        while (ecs_map_hasnext(&it)) {
            uint64_t key;
            ecs_table_column_t *columns = ecs_map_nextptr_w_key(&it, &key);
            ecs_type_t key_type = (ecs_type_t)(uintptr_t)key;
            result = ecs_lookup_child_in_columns(key_type, columns, parent, id);
            if (result) {
                break;
            }
        }
    }

    if (!result) {
        ecs_chunked_t *tables = world->main_stage.tables;
        uint32_t t, count = ecs_chunked_count(tables);

        for (t = 0; t < count; t ++) {
            ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, t);
            result = ecs_lookup_child_in_columns(
                table->type, table->columns, parent, id);
            if (result) {
                break;
            }
        }
    }

    return result;
}

ecs_entity_t ecs_lookup(
    ecs_world_t *world,
    const char *id)
{
    if (!strcmp(id, "$")) {
        return ECS_SINGLETON;
    }
    
    return ecs_lookup_child(world, 0, id);
}

static
void rematch_system_array(
    ecs_world_t *world,
    ecs_vector_t *systems)
{
    uint32_t i, count = ecs_vector_count(systems);
    ecs_entity_t *buffer = ecs_vector_first(systems);

    for (i = 0; i < count; i ++) {
        ecs_entity_t system = buffer[i];
        ecs_rematch_system(world, system);

        if (system != buffer[i]) {
            /* It is possible that rematching a system caused it to be activated
             * or deactived. In that case, reevaluate the current element again,
             * as it will now contain a different system. */
            i --;
            count = ecs_vector_count(systems);
        }
    }
}

static
void rematch_systems(
    ecs_world_t *world)
{
    rematch_system_array(world, world->on_load_systems);
    rematch_system_array(world, world->post_load_systems);
    rematch_system_array(world, world->pre_update_systems);
    rematch_system_array(world, world->on_update_systems);
    rematch_system_array(world, world->on_validate_systems);
    rematch_system_array(world, world->post_update_systems);
    rematch_system_array(world, world->pre_store_systems);
    rematch_system_array(world, world->on_store_systems);    
    rematch_system_array(world, world->inactive_systems);   
}

static
void revalidate_system_array(
    ecs_world_t *world,
    ecs_vector_t *systems)
{
    uint32_t i, count = ecs_vector_count(systems);
    ecs_entity_t *buffer = ecs_vector_first(systems);

    for (i = 0; i < count; i ++) {
        ecs_entity_t system = buffer[i];
        ecs_revalidate_system_refs(world, system);
    }
}

static
void revalidate_system_refs(
    ecs_world_t *world)
{
    revalidate_system_array(world, world->on_load_systems);
    revalidate_system_array(world, world->post_load_systems);
    revalidate_system_array(world, world->pre_update_systems);
    revalidate_system_array(world, world->on_update_systems);
    revalidate_system_array(world, world->on_validate_systems);
    revalidate_system_array(world, world->post_update_systems);
    revalidate_system_array(world, world->pre_store_systems);
    revalidate_system_array(world, world->on_store_systems);    
    revalidate_system_array(world, world->inactive_systems);   
}

static
void run_single_thread_stage(
    ecs_world_t *world,
    ecs_vector_t *systems,
    bool staged)
{
    uint32_t i, system_count = ecs_vector_count(systems);

    if (system_count) {
        ecs_entity_t *buffer = ecs_vector_first(systems);

        if (staged) {
            world->in_progress = true;
        }

        ecs_time_t start = {0};
        ecs_time_measure(&start);

        for (i = 0; i < system_count; i ++) {
            ecs_run(world, buffer[i], world->delta_time, NULL);
        }

        world->system_time_total += ecs_time_measure(&start);

        if (staged && world->auto_merge) {
            world->in_progress = false;
            ecs_merge(world);
            world->in_progress = true;
        }
    }
}

static
void run_multi_thread_stage(
    ecs_world_t *world,
    ecs_vector_t *systems)
{
    /* Run periodic table systems */
    uint32_t i, system_count = ecs_vector_count(systems);
    if (system_count) {
        bool valid_schedule = world->valid_schedule;
        ecs_entity_t *buffer = ecs_vector_first(systems);

        world->in_progress = true;

        for (i = 0; i < system_count; i ++) {
            if (!valid_schedule) {
                ecs_schedule_jobs(world, buffer[i]);
            }
            ecs_prepare_jobs(world, buffer[i]);
        }

        ecs_time_t start;
        ecs_time_measure(&start);

        ecs_run_jobs(world);

        world->system_time_total += ecs_time_measure(&start);

        if (world->auto_merge) {
            world->in_progress = false;
            ecs_merge(world);
            world->in_progress = true;
        }
    }
}

static
float start_measure_frame(
    ecs_world_t *world,
    float user_delta_time)
{
    float delta_time = 0;

    if (world->measure_frame_time || !user_delta_time) {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.sec) {
                delta_time = ecs_time_measure(&t);
            } else {
                ecs_time_measure(&t);
                if (world->target_fps) {
                    delta_time = 1.0 / world->target_fps;
                } else {
                    delta_time = 1.0 / 60.0; /* Best guess */
                }
            }
        
        /* Keep trying while delta_time is zero */
        } while (delta_time == 0);

        world->frame_start_time = t;  

        /* Compute total time passed since start of simulation */
        ecs_time_t diff = ecs_time_sub(t, world->world_start_time);
        world->world_time_total = ecs_time_to_double(diff);
    }

    return delta_time;
}

static
void stop_measure_frame(
    ecs_world_t *world,
    float delta_time)
{
    if (world->measure_frame_time) {
        ecs_time_t t = world->frame_start_time;
        double frame_time = ecs_time_measure(&t);
        world->frame_time_total += frame_time;

        /* Sleep if processing faster than target FPS */
        float target_fps = world->target_fps;
        if (target_fps) {
            float sleep = (1.0 / target_fps) - delta_time + world->fps_sleep;

            if (sleep > 0.01) {
                ecs_sleepf(sleep);
            }

            world->fps_sleep = sleep;
        }        
    }
}

bool ecs_progress(
    ecs_world_t *world,
    float user_delta_time)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(user_delta_time || ecs_os_api.get_time, ECS_MISSING_OS_API, "get_time");

    /* Start measuring total frame time */
    float delta_time = start_measure_frame(world, user_delta_time);

    if (!user_delta_time) {
        user_delta_time = delta_time;
    }

    world->delta_time = user_delta_time;

    bool has_threads = ecs_vector_count(world->worker_threads) != 0;

    if (world->should_match) {
        rematch_systems(world);
        world->should_match = false;
    }

    if (world->should_resolve) {
        revalidate_system_refs(world);
        world->should_resolve = false;
    }    

    /* -- System execution starts here -- */

    run_single_thread_stage(world, world->on_load_systems, false);
    run_single_thread_stage(world, world->post_load_systems, true);

    if (has_threads) {
        run_multi_thread_stage(world, world->pre_update_systems);
        run_multi_thread_stage(world, world->on_update_systems);
        run_multi_thread_stage(world, world->on_validate_systems);
        run_multi_thread_stage(world, world->post_update_systems);
    } else {
        run_single_thread_stage(world, world->pre_update_systems, true);
        run_single_thread_stage(world, world->on_update_systems, true);
        run_single_thread_stage(world, world->on_validate_systems, true);
        run_single_thread_stage(world, world->post_update_systems, true);
    }

    run_single_thread_stage(world, world->pre_store_systems, true);
    run_single_thread_stage(world, world->on_store_systems, true);

    /* -- System execution stops here -- */

    world->frame_count_total ++;
    
    stop_measure_frame(world, delta_time);

    world->in_progress = false;

    return !world->should_quit;
}

float ecs_get_delta_time(
    ecs_world_t *world)
{
    return world->delta_time;
}

void ecs_quit(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    world->should_quit = true;
}

void ecs_merge(
    ecs_world_t *world)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    assert(world->is_merging == false);

    bool measure_frame_time = world->measure_frame_time;

    world->is_merging = true;

    ecs_time_t t_start;
    if (measure_frame_time) {
        ecs_os_get_time(&t_start);
    }

    ecs_stage_merge(world, &world->temp_stage);

    uint32_t i, count = ecs_vector_count(world->worker_stages);
    if (count) {
        ecs_stage_t *buffer = ecs_vector_first(world->worker_stages);
        for (i = 0; i < count; i ++) {
            ecs_stage_merge(world, &buffer[i]);
        }
    }

    if (measure_frame_time) {
        world->merge_time_total += ecs_time_measure(&t_start);
    }

    world->is_merging = false;
}

void ecs_set_automerge(
    ecs_world_t *world,
    bool auto_merge)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    world->auto_merge = auto_merge;
}

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    if (!world->target_fps || enable) {
        world->measure_frame_time = enable;
    }
}

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    world->measure_system_time = enable;
}

void ecs_set_target_fps(
    ecs_world_t *world,
    float fps)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    ecs_assert(ecs_os_api.sleep != NULL, ECS_MISSING_OS_API, "sleep");

    if (!world->arg_fps) {
        ecs_measure_frame_time(world, true);
        world->target_fps = fps;
    }
}

/* Mock types so we don't have to depend on them. 
 * TODO: Need a better workaround */
typedef uint16_t EcsAdmin;
typedef uint32_t EcsConsole;

int ecs_enable_admin(
	ecs_world_t* world,
	uint16_t port)
{
    if (ecs_import_from_library(
        world, "flecs.systems.civetweb", NULL, 0) == ECS_INVALID_ENTITY) 
    {
        ecs_os_err("Failed to load flecs.systems.civetweb");
        return 1;
    }
    
    if (ecs_import_from_library(
        world, "flecs.systems.admin", NULL, 0) == ECS_INVALID_ENTITY)
    {
        ecs_os_err("Failed to load flecs.systems.admin");
        return 1;
    }

    /* Enable monitoring */
    ecs_measure_frame_time(world, true);
    ecs_measure_system_time(world, true);

    /* Create admin instance */
    ecs_entity_t EEcsAdmin = ecs_lookup(world, "EcsAdmin");
    ecs_set(world, 0, EcsAdmin, {port});

    ecs_os_log("admin is running on port %d", port);

    return 0;
}

int ecs_enable_console(
	ecs_world_t* world)
{
    if (ecs_import_from_library(
        world, "flecs.systems.console", NULL, 0) == ECS_INVALID_ENTITY) 
    {
        ecs_os_err("Failed to load flecs.systems.console");
        return 1;
    }

    /* Create console instance */
    ecs_entity_t EEcsConsole = ecs_lookup(world, "EcsConsole");
    ecs_set(world, 0, EcsConsole, {0});

    return 0;
}

void* ecs_get_context(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    return world->context;
}

uint32_t ecs_get_tick(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    return world->frame_count_total;
}

void ecs_set_context(
    ecs_world_t *world,
    void *context)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    world->context = context;
}

void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(!id_end || id_end > id_start, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!id_end || id_end > world->last_handle, ECS_INVALID_PARAMETER, NULL);

    if (world->last_handle < id_start) {
        world->last_handle = id_start - 1;
    }

    world->min_handle = id_start;
    world->max_handle = id_end;
}

bool ecs_enable_range_check(
    ecs_world_t *world,
    bool enable)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    bool old_value = stage->range_check_enabled;
    stage->range_check_enabled = enable;
    return old_value;
}

ecs_entity_t _ecs_import(
    ecs_world_t *world,
    ecs_module_init_action_t init_action,
    const char *module_name,
    int flags,
    void *handles_out,
    size_t handles_size)
{
    ecs_entity_t e = ecs_lookup(world, module_name);
    if (!e) {
        /* Load module */
        init_action(world, flags);

        /* Lookup module component (must be registered by module) */
        e = ecs_lookup(world, module_name);
        ecs_assert(e != ECS_INVALID_ENTITY, ECS_MODULE_UNDEFINED, module_name);

        /* Copy value of module component in handles_out parameter */
        if (handles_size && handles_out) {
            ecs_type_t t = ecs_type_from_entity(world, e);
            void *module_ptr = _ecs_get_ptr(world, ECS_SINGLETON, t);
            memcpy(handles_out, module_ptr, handles_size);
        }

    /* If module was already loaded, copy module component into handles_out */
    } else if (handles_size) {
        ecs_type_t t = ecs_type_from_entity(world, e);
        void *handles_ptr = _ecs_get_ptr(world, ECS_SINGLETON, t);
        memcpy(handles_out, handles_ptr, handles_size);
    }

    return e;
}

ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name,
    int flags)
{
    ecs_assert(library_name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(module_name != NULL, ECS_INVALID_PARAMETER, NULL);

    char *import_func = (char*)module_name; /* safe */
    char *module = (char*)module_name;

    if (!ecs_os_api.module_to_dl || 
        !ecs_os_api.dlopen || 
        !ecs_os_api.dlproc || 
        !ecs_os_api.dlclose) 
    {
        ecs_os_err(
            "library loading not supported, set module_to_dl, dlopen, dlclose "
            "and dlproc os API callbacks first");
        return ECS_INVALID_ENTITY;
    }

    /* If no module name is specified, try default naming convention for loading
     * the main module from the library */
    if (!import_func) {
        import_func = ecs_os_malloc(strlen(library_name) + sizeof("Import"));
        ecs_assert(import_func != NULL, ECS_OUT_OF_MEMORY, NULL);
        
        const char *ptr;
        char ch, *bptr = import_func;
        bool capitalize = true;
        for (ptr = library_name; (ch = *ptr); ptr ++) {
            if (ch == '.') {
                capitalize = true;
            } else {
                if (capitalize) {
                    *bptr = toupper(ch);
                    bptr ++;
                    capitalize = false;
                } else {
                    *bptr = tolower(ch);
                    bptr ++;
                }
            }
        }

        *bptr = '\0';

        module = ecs_os_strdup(import_func);
        ecs_assert(module != NULL, ECS_OUT_OF_MEMORY, NULL);

        strcat(bptr, "Import");
    }

    char *library_filename = ecs_os_module_to_dl(library_name);
    if (!library_filename) {
        ecs_os_err("failed to find library file for '%s'", library_name);
        if (module != module_name) {
            ecs_os_free(module);
        }
        return 0;
    } else {
        ecs_os_dbg("found file '%s' for library '%s'", 
            library_filename, library_name);
    }

    ecs_os_dl_t dl = ecs_os_dlopen(library_filename);
    if (!dl) {
        ecs_os_err("failed to load library '%s' ('%s')", 
            library_name, library_filename);
        
        ecs_os_free(library_filename);

        if (module != module_name) {
            ecs_os_free(module);
        }    

        return 0;
    } else {
        ecs_os_dbg("library '%s' ('%s') loaded", 
            library_name, library_filename);
    }

    ecs_module_init_action_t action = (ecs_module_init_action_t)
        ecs_os_dlproc(dl, import_func);
    if (!action) {
        ecs_os_err("failed to load import function %s from library %s",
            import_func, library_name);
        free(library_filename);
        ecs_os_dlclose(dl);            
        return ECS_INVALID_ENTITY;
    } else {
        ecs_os_dbg("found import function '%s' in library '%s' for module '%s'",
            import_func, library_name, module);
    }

    /* Do not free id, as it will be stored as the component identifier */
    ecs_entity_t result = _ecs_import(world, action, module, flags, NULL, 0);

    if (import_func != module_name) {
        ecs_os_free(import_func);
    }

    if (module != module_name) {
        ecs_os_free(module);
    }

    free(library_filename);

    return result;
}

uint16_t ecs_get_thread_index(
    ecs_world_t *world)
{
    if (world->magic == ECS_THREAD_MAGIC) {
        ecs_thread_t *thr = (ecs_thread_t*)world;
        return thr->index;
    } else if (world->magic == ECS_WORLD_MAGIC) {
        return 0;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

uint32_t ecs_get_threads(
    ecs_world_t *world)
{
    return ecs_vector_count(world->worker_threads);
}

uint32_t ecs_get_target_fps(
    ecs_world_t *world)
{
    return world->target_fps;
}
