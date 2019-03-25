#include <assert.h>
#include <string.h>
#include "include/private/flecs.h"
#include "include/util/time.h"

static
void match_type(
    ecs_world_t *world,
    EcsStage *stage,
    EcsEntity system,
    ecs_rows_tystem *system_data,
    EcsType type)
{
    /* Test if the components of the system are equal or a subset of the 
     * components of the type */
    EcsEntity match = ecs_type_contains(
        world, stage, type, system_data->base.and_from_entity, true, false);

    /* If there is a match, add the system to the type-row_system index */
    if (match) {
        EcsMap *index = NULL;
        EcsSystemKind kind = system_data->base.kind;

        if (kind == EcsOnAdd) {
            index = world->type_sys_add_index;
        } else if (kind == EcsOnRemove) {
            index = world->type_sys_remove_index;
        } else if (kind == EcsOnSet) {
            index = world->type_sys_set_index;
        } else {
            ecs_abort(ECS_INVALID_PARAMETERS, NULL);
        }

        EcsArray *systems = ecs_map_get(index, type);
        if (!systems) {
            systems = ecs_array_new(&handle_arr_params, 1);
        }

        EcsEntity *new_elem = ecs_array_add(&systems, &handle_arr_params);
        *new_elem = system;

        /* Always set the system entry, as array may have been realloc'd */
        ecs_map_set(index, type, systems);
    }
}

/* Match system against existing families to build the type-rowsys index */
static
void match_families(
    ecs_world_t *world,
    EcsEntity system,
    ecs_rows_tystem *system_data)
{
    EcsIter it = ecs_map_iter(world->main_stage.type_index);

    /* Iterating over a map is a bit slow, but this only happens when a new
     * row system is created, which is very infrequent. */
    while (ecs_iter_hasnext(&it)) {
        uint64_t key; /* Only interested in the key, which is the type hash */
        ecs_map_next(&it, &key);

        EcsType type = key;
        
        match_type(world, NULL, system, system_data, type);
    }
}

/** Create a new row system. A row system is a system executed on a single row,
 * typically as a result of a ADD, REMOVE or SET trigger.
 */
static
EcsEntity new_row_system(
    ecs_world_t *world,
    const char *id,
    EcsSystemKind kind,
    bool needs_tables,
    const char *sig,
    EcsSystemAction action)
{
    uint32_t count = ecs_columns_count(sig);
    if (!count) {
        return 0;
    }

    EcsEntity result = _ecs_new(world, world->t_row_system);
    EcsId *id_data = ecs_get_ptr(world, result, EcsId);
    *id_data = id;

    ecs_rows_tystem *system_data = ecs_get_ptr(world, result, ecs_rows_tystem);
    memset(system_data, 0, sizeof(ecs_rows_tystem));
    system_data->base.action = action;
    system_data->base.signature = sig;
    system_data->base.enabled = true;
    system_data->base.kind = kind;
    system_data->components = ecs_array_new(&handle_arr_params, count);

    if (ecs_parse_component_expr(
        world, sig, ecs_parse_component_action, system_data) != EcsOk)
    {
        ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, sig);
    }

    EcsType type_id = 0;
    uint32_t i, column_count = ecs_array_count(system_data->base.columns);
    EcsSystemColumn *buffer = ecs_array_buffer(system_data->base.columns);

    for (i = 0; i < column_count; i ++) {
        EcsEntity *h = ecs_array_add(
            &system_data->components, &handle_arr_params);

        EcsSystemColumn *column = &buffer[i];
        *h = column->is.component;

        if (column->kind != EcsFromId) {
            type_id = ecs_type_add(
                world, NULL, type_id, column->is.component);
        }
    }

    EcsEntity *elem = NULL;

    if (!needs_tables) {
        if (kind == EcsOnFrame) {
            elem = ecs_array_add(&world->tasks, &handle_arr_params);
        } else if (kind == EcsOnRemove) {
            elem = ecs_array_add(&world->fini_tasks, &handle_arr_params);
        }
    } else {
        if (kind == EcsOnAdd) {
            elem = ecs_array_add(&world->add_systems, &handle_arr_params);
        } else if (kind == EcsOnRemove) {
            elem = ecs_array_add(&world->remove_systems, &handle_arr_params);
        } else if (kind == EcsOnSet) {
            elem = ecs_array_add(&world->set_systems, &handle_arr_params);
        }
    }

    if (elem) {
        *elem = result;
    }

    ecs_system_compute_and_families(world, result, &system_data->base);

    if (needs_tables) {
        match_families(world, result, system_data);
    }

    return result;
}

/* -- Private API -- */

void ecs_system_compute_and_families(
    ecs_world_t *world,
    EcsEntity system,
    EcsSystem *system_data)
{
    uint32_t i, column_count = ecs_array_count(system_data->columns);
    EcsSystemColumn *buffer = ecs_array_buffer(system_data->columns);

    for (i = 0; i < column_count; i ++) {
        EcsSystemColumn *elem = &buffer[i];
        EcsSystemExprElemKind elem_kind = elem->kind;
        EcsSystemExprOperKind oper_kind = elem->oper_kind;

        if (elem_kind == EcsFromSelf) {
            if (oper_kind == EcsOperAnd) {
                system_data->and_from_entity = ecs_type_add(
                 world, NULL, system_data->and_from_entity, elem->is.component);
            }
        } else if (elem_kind == EcsFromSystem) {
            if (oper_kind == EcsOperAnd) {
                system_data->and_from_system = ecs_type_add(
                  world, NULL, system_data->and_from_system, elem->is.component);
            }
        }
    }
}

/** Parse callback that adds component to the components array for a system */
EcsResult ecs_parse_component_action(
    ecs_world_t *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component_id,
    const char *source_id,
    void *data)
{
    EcsSystem *system_data = data;
    EcsSystemColumn *elem;

    /* Lookup component handly by string identifier */
    EcsEntity component = ecs_lookup(world, component_id);
    if (!component) {
        /* "0" is a valid expression used to indicate that a system matches no
         * components */
        if (strcmp(component_id, "0")) {
            ecs_abort(ECS_INVALID_COMPONENT_ID, component_id);
        } else {
            /* Don't add 0 component to signature */
            return EcsOk;
        }
    }

    /* If retrieving a component from a system, only the AND operator is
     * supported. The set of system components is expected to be constant, and
     * thus no conditional operators are needed. */
    if (elem_kind == EcsFromSystem && oper_kind != EcsOperAnd) {
        ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, 0);
    }

    /* AND (default) and optional columns are stored the same way */
    if (oper_kind == EcsOperAnd || oper_kind == EcsOperOptional) {
        elem = ecs_array_add(&system_data->columns, &column_arr_params);
        elem->kind = elem_kind;
        elem->oper_kind = oper_kind;
        elem->is.component = component;

        if (elem_kind == EcsFromEntity) {
            elem->source = ecs_lookup(world, source_id);
            if (!elem->source) {
                ecs_abort(ECS_UNRESOLVED_IDENTIFIER, source_id);
            }
        }

    /* OR columns store a type id instead of a single component */
    } else if (oper_kind == EcsOperOr) {
        elem = ecs_array_last(system_data->columns, &column_arr_params);
        if (elem->oper_kind == EcsOperAnd) {
            elem->is.type = ecs_type_add(
                world, NULL, 0, elem->is.component);
        } else {
            if (elem->kind != elem_kind) {
                /* Cannot mix FromEntity and FromComponent in OR */
                goto error;
            }
        }

        elem->is.type = ecs_type_add(
            world, NULL, elem->is.type, component);
        elem->kind = elem_kind;
        elem->oper_kind = EcsOperOr;

    /* A system stores two NOT familes; one for entities and one for components.
     * These can be quickly & efficiently used to exclude tables with
     * ecs_type_contains. */
    } else if (oper_kind == EcsOperNot) {
        elem = ecs_array_add(&system_data->columns, &column_arr_params);
        elem->kind = EcsFromId; /* Just pass handle to system */
        elem->oper_kind = EcsOperNot;
        elem->is.component = component;

        if (elem_kind == EcsFromSelf) {
            system_data->not_from_entity =
                ecs_type_add(
                    world, NULL, system_data->not_from_entity, component);
        } else {
            system_data->not_from_component =
              ecs_type_add(
                  world, NULL, system_data->not_from_component, component);
        }
    }

    return EcsOk;
error:
    return EcsError;
}

/** Run system on a single row */
bool ecs_notify_row_system(
    ecs_world_t *world,
    EcsStage *stage,
    EcsEntity system,
    EcsArray *type,
    EcsTableColumn *table_columns,
    uint32_t offset,
    uint32_t limit)
{
    EcsEntityInfo entity_info = {0};

    ecs_world_t *real_world = world;
    if (world->magic == ECS_THREAD_MAGIC) {
        real_world = ((EcsThread*)world)->world; /* dispel the magic */
    }

    ecs_rows_tystem *system_data = get_ptr(
        real_world, &real_world->main_stage, system, Eecs_rows_tystem, false, true, &entity_info);
    
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return false;
    }

    EcsSystemAction action = system_data->base.action;

    uint32_t i, column_count = ecs_array_count(system_data->base.columns);
    EcsSystemColumn *buffer = ecs_array_buffer(system_data->base.columns);
    int32_t columns[column_count];
    EcsReference references[column_count];
    void* ref_ptrs[column_count];
    uint32_t ref_id = 0;

    for (i = 0; i < column_count; i ++) {
        if (buffer[i].kind == EcsFromSelf) {
            columns[i] = ecs_type_index_of(type, buffer[i].is.component) + 1;
        } else {
            EcsEntity entity = 0;
            EcsEntity component = buffer[i].is.component;
            
            if (buffer[i].kind == EcsFromSystem) {
                entity = system;
            } else if (buffer[i].kind == EcsFromSingleton) {
                entity = 0;         
            } else if (buffer[i].kind == EcsFromEntity) {
                entity = buffer[i].source;
            }

            references[ref_id] = (EcsReference){.entity = entity, .component = component};
            ref_ptrs[ref_id] = get_ptr(real_world, &real_world->main_stage, entity, component, false, true, &entity_info);

            ref_id ++;
            columns[i] = -ref_id;
        }
    }

    ecs_rows_t rows = {
        .world = world,
        .system = system,
        .columns = columns,
        .column_count = ecs_array_count(system_data->components),
        .references = references,
        .table_columns = table_columns,
        .components = ecs_array_buffer(system_data->components),
        .frame_offset = 0,
        .offset = offset,
        .count = limit
    };

    if (ref_id) {
        rows.references = references;
        rows.ref_ptrs = ref_ptrs;
    }

    if (table_columns) {
        EcsEntity *entities = ecs_array_buffer(table_columns[0].data);
        rows.entities = &entities[rows.offset];
    }

    action(&rows);

    return true;
}

/** Run a task. A task is a system that contains no columns that can be matched
 * against a table. Examples of such columns are EcsFromSystem or EcsFromId.
 * Tasks are ran once every frame. */
void ecs_run_task(
    ecs_world_t *world,
    EcsEntity system,
    float delta_time)
{
    EcsStage *stage = ecs_get_stage(&world);
    ecs_notify_row_system(world, stage, system, NULL, NULL, 0, 1);
}

/* Notify row system of a new type */
void ecs_row_system_notify_of_type(
    ecs_world_t *world,
    EcsStage *stage,
    EcsEntity system,
    EcsType type)
{
    ecs_rows_tystem *system_data = ecs_get_ptr(world, system, ecs_rows_tystem);

    match_type(world, stage, system, system_data, type);
}

/* -- Public API -- */

EcsEntity ecs_new_system(
    ecs_world_t *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action)
{
    EcsEntity result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    bool needs_tables = ecs_needs_tables(world, sig);

    if (!needs_tables && (kind == EcsOnAdd ||
        kind == EcsOnSet))
    {
        ecs_abort(ECS_INVALID_PARAMETERS, 0);
    }

    if (needs_tables && (kind == EcsOnFrame || kind == EcsPreFrame ||
                         kind == EcsPostFrame || kind == EcsManual ||
                         kind == EcsOnLoad || kind == EcsOnStore))
    {
        result = ecs_new_col_system(world, id, kind, sig, action);
    } else if (!needs_tables ||
        (kind == EcsOnAdd || kind == EcsOnRemove || kind == EcsOnSet))
    {
        result = new_row_system(world, id, kind, needs_tables, sig, action);
    } else {
        ecs_abort(ECS_INVALID_PARAMETERS, 0);
    }

    EcsSystem *system_data = ecs_get_ptr(world, result, EcsColSystem);
    if (!system_data) {
        system_data = _ecs_get_ptr(world, result, Tecs_rows_tystem);
        if (!system_data) {
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
        }
    }

    /* If system contains FromSystem params, add them tot the system */
    if (system_data->and_from_system) {
        EcsArray *f = ecs_type_get(world, NULL, system_data->and_from_system);
        EcsEntity *buffer = ecs_array_buffer(f);
        uint32_t i, count = ecs_array_count(f);
        for (i = 0; i < count; i ++) {
            EcsType type = ecs_type_from_entity(world, buffer[i]);
            _ecs_add(world, result, type);
        }
    }

    return result;
}

void ecs_enable(
    ecs_world_t *world,
    EcsEntity system,
    bool enabled)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    bool col_system = false;

    /* Try to get either ColSystem or RowSystem data */
    EcsSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        system_data = ecs_get_ptr(world, system, ecs_rows_tystem);
    } else {
        col_system = true;
    }

    /* If entity is neither ColSystem nor RowSystem, it should be a type */
    if (!system_data) {
        EcsTypeComponent *type_data = ecs_get_ptr(
            world, system, EcsTypeComponent);

        assert(type_data != NULL);

        ecs_world_t *world_temp = world;
        EcsStage *stage = ecs_get_stage(&world_temp);
        EcsArray *type = ecs_type_get(world, stage, type_data->type);
        EcsEntity *buffer = ecs_array_buffer(type);
        uint32_t i, count = ecs_array_count(type);
        for (i = 0; i < count; i ++) {
            /* Enable/disable all systems in type */
            ecs_enable(world, buffer[i], enabled);
        }
    } else {
        if (col_system) {
            EcsColSystem *col_system = (EcsColSystem*)system_data;
            if (enabled) {
                if (!system_data->enabled) {
                    if (ecs_array_count(col_system->tables)) {
                        ecs_world_activate_system(
                            world, system, col_system->base.kind, true);
                    }
                }
            } else {
                if (system_data->enabled) {
                    if (ecs_array_count(col_system->tables)) {
                        ecs_world_activate_system(
                            world, system, col_system->base.kind, false);
                    }
                }
            }
        }

        system_data->enabled = enabled;
    }
}

bool ecs_is_enabled(
    ecs_world_t *world,
    EcsEntity system)
{
    EcsSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        system_data = ecs_get_ptr(world, system, ecs_rows_tystem);
    }

    if (system_data) {
        return system_data->enabled;
    } else {
        return true;
    }
}

void ecs_set_period(
    ecs_world_t *world,
    EcsEntity system,
    float period)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (system_data) {
        system_data->period = period;
    }
}

void* _ecs_column(
    ecs_rows_t *rows,
    uint32_t index,
    bool test)
{
    if (index > rows->column_count) {
        ecs_assert(test, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return NULL;
    }

    int32_t table_column;

    if (index == 0) {
        table_column = 0;
    } else {
        if (!rows->columns) {
            ecs_assert(test, ECS_INTERNAL_ERROR, NULL);
            return NULL;
        }

        table_column = rows->columns[index - 1];
        if (!table_column) {
            ecs_assert(test, ECS_COLUMN_IS_NOT_SET, NULL);
            return NULL;
        }
    }

    if (table_column < 0) {
        ecs_assert(test, ECS_COLUMN_IS_SHARED, NULL);
        return NULL;
    }

    if (!rows->table_columns) {
        ecs_assert(test, ECS_INTERNAL_ERROR, NULL);
        return NULL;
    }

    EcsTableColumn *column = &((EcsTableColumn*)rows->table_columns)[table_column];
    void *buffer = ecs_array_buffer(column->data);
    return ECS_OFFSET(buffer, column->size * rows->offset);
}

void* _ecs_shared(
    ecs_rows_t *rows,
    uint32_t index,
    bool test)
{
    if (index > rows->column_count) {
        ecs_assert(test, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return NULL;
    }
    
    int32_t table_column;

    if (index == 0) {
        return 0;
    } else {
        if (!rows->columns) {
            ecs_assert(test, ECS_INTERNAL_ERROR, NULL);
            return NULL;
        }

        table_column = rows->columns[index - 1];
        if (table_column >= 0) {
            ecs_assert(test, ECS_COLUMN_IS_NOT_SHARED, NULL);
            return NULL;
        }

        if (!rows->references) {
            ecs_assert(test, ECS_INTERNAL_ERROR, NULL);
            return NULL;
        }

        return rows->ref_ptrs[-table_column - 1];
    }
}

EcsEntity _ecs_column_source(
    ecs_rows_t *rows,
    uint32_t index,
    bool test)
{
    if (index > rows->column_count) {
        return 0;
    }

    if (index == 0) {
        return 0;
    } else {
        if (!rows->columns) {
            ecs_assert(test, ECS_INTERNAL_ERROR, NULL);
            return 0;
        }

        int32_t table_column = rows->columns[index - 1];
        if (table_column >= 0) {
            return 0;
        }

        ecs_assert(rows->references != NULL, ECS_INTERNAL_ERROR, NULL);

        EcsReference *ref = &rows->references[-table_column - 1];
        return ref->entity;
    }
}

EcsType _ecs_column_type(
    ecs_rows_t *rows,
    uint32_t index,
    bool test)
{
    if (index == 0) {
        return 0;
    } else if (index > rows->column_count) {
        return 0;
    } else {
        if (!rows->components) {
            ecs_assert(test, ECS_INTERNAL_ERROR, NULL);
            return 0;
        }

        EcsEntity component = rows->components[index - 1];
        return ecs_type_from_entity(rows->world, component);
    }
}

EcsEntity _ecs_column_component(
    ecs_rows_t *rows,
    uint32_t index,
    bool test)
{
    if (index == 0) {
        return 0;
    } else if (index > rows->column_count) {
        return 0; 
    } else {
        if (!rows->components) {
            ecs_assert(test, ECS_INTERNAL_ERROR, NULL);
            return 0;
        }
        return rows->components[index - 1];
    }  
}

void *_ecs_field(
    ecs_rows_t *rows, 
    uint32_t index, 
    uint32_t column,
    bool test)
{
    if (column > rows->column_count) {
        ecs_assert(test, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return NULL;
    }

    int32_t table_column;

    if (column == 0) {
        table_column = 0;
    } else {
        ecs_assert(rows->columns != NULL, ECS_INTERNAL_ERROR, NULL);
        table_column = rows->columns[column - 1];

        if (!table_column) {
            return NULL;
        }
    }

    if (table_column < 0) {
        ecs_assert(rows->ref_ptrs != NULL, ECS_INTERNAL_ERROR, NULL);
        return rows->ref_ptrs[-table_column - 1];
    } else {
        ecs_assert(rows->table_columns != NULL, ECS_INTERNAL_ERROR, NULL);
        EcsTableColumn *column = &((EcsTableColumn*)rows->table_columns)[table_column];

#ifndef NDEBUG
        ecs_assert(index < ecs_array_count(column->data), ECS_OUT_OF_RANGE, 0);
#endif

        void *buffer = ecs_array_buffer(column->data);
        return ECS_OFFSET(buffer, column->size * (index + rows->offset));
    }
}
