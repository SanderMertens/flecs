/**
 * @file commands.c
 * @brief Command queue implementation.
 */

#include "private_api.h"

static
ecs_table_t* flecs_find_table_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_table_diff_builder_t *diff)
{
    ecs_table_diff_t temp_diff = ECS_TABLE_DIFF_INIT;
    table = flecs_table_traverse_remove(world, table, &id, &temp_diff);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_table_diff_build_append_table(world, diff, &temp_diff);
    return table;
error:
    return NULL;
}

static
ecs_cmd_t* flecs_cmd_new(
    ecs_stage_t *stage)
{
    ecs_cmd_t *cmd = ecs_vec_append_t(&stage->allocator, &stage->cmd->queue, 
        ecs_cmd_t);
    cmd->is._1.value = NULL;
    cmd->id = 0;
    cmd->next_for_entity = 0;
    cmd->entry = NULL;
    cmd->system = stage->system;
    return cmd;
}

static
ecs_cmd_t* flecs_cmd_new_batched(
    ecs_stage_t *stage, 
    ecs_entity_t e)
{
    ecs_vec_t *cmds = &stage->cmd->queue;
    ecs_cmd_entry_t *entry = flecs_sparse_get_t(
        &stage->cmd->entries, ecs_cmd_entry_t, e);

    int32_t cur = ecs_vec_count(cmds);
    ecs_cmd_t *cmd = flecs_cmd_new(stage);
    bool is_new = false;
    if (entry) {
        if (entry->first == -1) {
            /* Existing but invalidated entry */
            entry->first = cur;
            cmd->entry = entry;
        } else {
            int32_t last = entry->last;
            ecs_cmd_t *arr = ecs_vec_first_t(cmds, ecs_cmd_t);
            if (arr[last].entity == e) {
                ecs_cmd_t *last_op = &arr[last];
                last_op->next_for_entity = cur;
                if (last == entry->first) {
                    /* Flip sign bit so flush logic can tell which command
                    * is the first for an entity */
                    last_op->next_for_entity *= -1;
                }
            } else {
                /* Entity with different version was in the same queue. Discard
                 * the old entry and create a new one. */
                is_new = true;
            }
        }
    } else {
        is_new = true;
    }

    if (is_new) {
        cmd->entry = entry = flecs_sparse_ensure_fast_t(
            &stage->cmd->entries, ecs_cmd_entry_t, e);
        entry->first = cur;
    }

    entry->last = cur;

    return cmd;
}

bool flecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    flecs_poly_assert(world, ecs_world_t);
    flecs_poly_assert(stage, ecs_stage_t);
    flecs_check_exclusive_world_access_write(world);
    (void)world;
    if (stage->defer < 0) return false;
    return (++ stage->defer) == 1;
}

bool flecs_defer_cmd(
    ecs_stage_t *stage)
{
    if (stage->defer) {
        return (stage->defer > 0);
    }

    stage->defer ++;
    return false;
}

bool flecs_defer_modified(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        if (cmd) {
            cmd->kind = EcsCmdModified;
            cmd->id = id;
            cmd->entity = entity;
        }
        return true;
    }
    return false;
}

bool flecs_defer_clone(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value)
{   
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdClone;
        cmd->id = src;
        cmd->entity = entity;
        cmd->is._1.clone_value = clone_value;
        return true;
    }
    return false;   
}

bool flecs_defer_path(
    ecs_stage_t *stage,
    ecs_entity_t parent,
    ecs_entity_t entity,
    const char *name)
{
    if (stage->defer > 0) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdPath;
        cmd->entity = entity;
        cmd->id = parent;
        cmd->is._1.value = ecs_os_strdup(name);
        return true;
    }
    return false;
}

bool flecs_defer_delete(
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdDelete;
        cmd->entity = entity;
        return true;
    }
    return false;
}

bool flecs_defer_clear(
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
        cmd->kind = EcsCmdClear;
        cmd->entity = entity;
        return true;
    }
    return false;
}

bool flecs_defer_on_delete_action(
    ecs_stage_t *stage,
    ecs_id_t id,
    ecs_entity_t action)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdOnDeleteAction;
        cmd->id = id;
        cmd->entity = action;
        return true;
    }
    return false;
}

bool flecs_defer_enable(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = enable ? EcsCmdEnable : EcsCmdDisable;
        cmd->entity = entity;
        cmd->id = id;
        return true;
    }
    return false;
}

bool flecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    ecs_id_t id,
    const ecs_entity_t **ids_out)
{
    if (flecs_defer_cmd(stage)) {
        ecs_entity_t *ids = ecs_os_malloc(count * ECS_SIZEOF(ecs_entity_t));

        /* Use ecs_new_id as this is thread safe */
        int i;
        for (i = 0; i < count; i ++) {
            ids[i] = ecs_new(world);
        }

        *ids_out = ids;

        /* Store data in op */
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdBulkNew;
        cmd->id = id;
        cmd->is._n.entities = ids;
        cmd->is._n.count = count;
        cmd->entity = 0;
        return true;
    }
    return false;
}

bool flecs_defer_add(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage)) {
        ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
        cmd->kind = EcsCmdAdd;
        cmd->id = id;
        cmd->entity = entity;
        return true;
    }
    return false;
}

bool flecs_defer_remove(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage)) {
        ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity); 
        cmd->kind = EcsCmdRemove;
        cmd->id = id;
        cmd->entity = entity;

        /* If an override is removed, restore to the component to the value of 
         * the overridden component. This serves to purposes:
         *
         * - the application immediately sees the correct component value
         * - if a remove command is followed up by an add command, the override
         *   will still be applied vs. getting cancelled out because of
         *   command batching.
         */
        ecs_world_t *world = stage->world;
        ecs_record_t *r = flecs_entities_get(world, entity);
        ecs_table_t *table = r->table;
        ecs_table_overrides_t *o = table->data.overrides;
        if (o) {
            ecs_component_record_t *cr = flecs_components_get(world, id);
            const ecs_type_info_t *ti;
            if (cr && (ti = cr->type_info)) {
                const ecs_table_record_t *tr = flecs_component_get_table(
                    cr, table);
                if (tr) {
                    ecs_assert(tr->column != -1, ECS_INTERNAL_ERROR, NULL);
                    ecs_ref_t *ref = &o->refs[tr->column];
                    if (ref->entity) {
                        void *dst = ECS_OFFSET(
                            table->data.columns[tr->column].data, 
                            ti->size * ECS_RECORD_TO_ROW(r->row));
                        const void *src = ecs_ref_get_id(
                            world, &o->refs[tr->column], id);
                        ecs_copy_t copy = ti->hooks.copy;
                        if (copy) {
                            copy(dst, src, 1, ti);
                        } else {
                            ecs_os_memcpy(dst, src, ti->size);
                        }
                    }
                }
            }
        }

        return true;
    }
    return false;
}

/* Return existing component pointer & type info */
static
flecs_component_ptr_t flecs_defer_get_existing(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *r,
    ecs_id_t id,
    ecs_size_t size)
{
    flecs_component_ptr_t ptr = flecs_get_mut(world, entity, id, r, size);

    /* Make sure we return type info, even if entity doesn't have component */
    if (!ptr.ti) {
        ecs_component_record_t *cr = flecs_components_get(world, id);
        if (!cr) {
            /* If cr doesn't exist yet, create it but only if the 
            * application is not multithreaded. */
            if (world->flags & EcsWorldMultiThreaded) {
                ptr.ti = ecs_get_type_info(world, id);
            } else {
                /* When not in multi threaded mode, it's safe to find or 
                * create the component record. */
                cr = flecs_components_ensure(world, id);
                ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
                
                /* Get type_info from component record. We could have called 
                * ecs_get_type_info directly, but since this function can be
                * expensive for pairs, creating the component record ensures we 
                * can find the type_info quickly for subsequent operations. */
                ptr.ti = cr->type_info;
            }
        } else {
            ptr.ti = cr->type_info;
        }
    }
    return ptr;
}

void* flecs_defer_emplace(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    bool *is_new)
{
    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
    cmd->entity = entity;
    cmd->id = id;

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t ptr = flecs_defer_get_existing(
        world, entity, r, id, size);

    const ecs_type_info_t *ti = ptr.ti;
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "provided component is not a type");
    ecs_assert(!size || size == ti->size, ECS_INVALID_PARAMETER,
        "mismatching size specified for component in ensure/emplace/set");
    size = ti->size;

    void *cmd_value = ptr.ptr;
    if (!ptr.ptr) {
        ecs_stack_t *stack = &stage->cmd->stack;
        cmd_value = flecs_stack_alloc(stack, size, ti->alignment);

        cmd->kind = EcsCmdEmplace;
        cmd->is._1.size = size;
        cmd->is._1.value = cmd_value;
        if (is_new) *is_new = true;
    } else {
        cmd->kind = EcsCmdAdd;
        if (is_new) *is_new = false;
    }

    return cmd_value;
error:
    return NULL;
}

void* flecs_defer_ensure(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size)
{
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
    cmd->entity = entity;
    cmd->id = id;

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t ptr = flecs_defer_get_existing(
        world, entity, r, id, size);

    const ecs_type_info_t *ti = ptr.ti;
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "provided component is not a type");
    ecs_assert(size == ti->size, ECS_INVALID_PARAMETER,
        "bad size for component in ensure");

    ecs_table_t *table = r->table;
    if (!ptr.ptr) {
        ecs_stack_t *stack = &stage->cmd->stack;
        cmd->kind = EcsCmdEnsure;
        cmd->is._1.size = size;
        cmd->is._1.value = ptr.ptr = 
            flecs_stack_alloc(stack, size, ti->alignment);

        /* Check if entity inherits component */
        void *base = NULL;
        if (table && (table->flags & EcsTableHasIsA)) {
            ecs_component_record_t *cr = flecs_components_get(world, id);
            base = flecs_get_base_component(world, table, id, cr, 0);
        }

        if (!base) {
            /* Normal ctor */
            ecs_xtor_t ctor = ti->hooks.ctor;
            if (ctor) {
                ctor(ptr.ptr, 1, ti);
            }
        } else {
            /* Override */
            ecs_copy_t copy = ti->hooks.copy_ctor;
            if (copy) {
                copy(ptr.ptr, base, 1, ti);
            } else {
                ecs_os_memcpy(ptr.ptr, base, size);
            }
        }
    } else {
        cmd->kind = EcsCmdAdd;
    }

    return ptr.ptr;
error:
    return NULL;
}

void* flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    void *value)
{
    ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
    ecs_assert(cmd != NULL, ECS_INTERNAL_ERROR, NULL);
    cmd->entity = entity;
    cmd->id = id;

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t ptr = flecs_defer_get_existing(
        world, entity, r, id, size);

    const ecs_type_info_t *ti = ptr.ti;
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "provided component is not a type");
    ecs_assert(size == ti->size, ECS_INVALID_PARAMETER,
        "mismatching size specified for component in ensure/emplace/set (%u vs %u)",
            size, ti->size);

    /* Handle trivial set command (no hooks, OnSet observers) */
    if (id < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[id]) {
            if (!ptr.ptr) {
                ptr.ptr = flecs_stack_alloc(
                    &stage->cmd->stack, size, ti->alignment);
                
                /* No OnSet observers, so ensure is enough */
                cmd->kind = EcsCmdEnsure;
                cmd->is._1.size = size;
                cmd->is._1.value = ptr.ptr;
            } else {
                /* No OnSet observers, so only thing we need to do is make sure
                 * that a preceding remove command doesn't cause the entity to
                 * end up without the component. */
                cmd->kind = EcsCmdAdd;
            }

            ecs_os_memcpy(ptr.ptr, value, size);
            return ptr.ptr;
        }
    }

    ecs_copy_t copy;
    if (!ptr.ptr) {
        cmd->kind = EcsCmdSet;
        cmd->is._1.size = size;
        ptr.ptr = cmd->is._1.value =
            flecs_stack_alloc(&stage->cmd->stack, size, ti->alignment);
        copy = ti->hooks.copy_ctor;
    } else {
        cmd->kind = EcsCmdAddModified;

        /* Call on_replace hook before copying the new value. */
        if (ti->hooks.on_replace) {
            flecs_invoke_replace_hook(
                world, r->table, entity, id, ptr.ptr, value, ti);
        }

        copy = ti->hooks.copy;
    }

    if (copy) {
        copy(ptr.ptr, value, 1, ti);
    } else {
        ecs_os_memcpy(ptr.ptr, value, size);
    }

    return ptr.ptr;
error:
    return NULL;
}

/* Same as flecs_defer_set, but doesn't copy value into storage. */
void* flecs_defer_cpp_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value)
{
    ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
    ecs_assert(cmd != NULL, ECS_INTERNAL_ERROR, NULL);
    cmd->entity = entity;
    cmd->id = id;

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t ptr = flecs_defer_get_existing(
        world, entity, r, id, size);

    const ecs_type_info_t *ti = ptr.ti;
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "provided component is not a type");
    ecs_assert(size == ti->size, ECS_INVALID_PARAMETER,
        "mismatching size specified for component in ensure/emplace/set");

    /* Handle trivial set command (no hooks, OnSet observers) */
    if (id < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[id]) {
            if (!ptr.ptr) {
                ptr.ptr = flecs_stack_alloc(
                    &stage->cmd->stack, size, ti->alignment);
                
                /* No OnSet observers, so ensure is enough */
                cmd->kind = EcsCmdEnsure;
                cmd->is._1.size = size;
                cmd->is._1.value = ptr.ptr;
            } else {
                /* No OnSet observers, so only thing we need to do is make sure
                 * that a preceding remove command doesn't cause the entity to
                 * end up without the component. */
                cmd->kind = EcsCmdAdd;
            }

            ecs_os_memcpy(ptr.ptr, value, size);
            return ptr.ptr;
        }
    }
    
    if (!ptr.ptr) {
        cmd->kind = EcsCmdSet;
        cmd->is._1.size = size;
        ptr.ptr = cmd->is._1.value =
            flecs_stack_alloc(&stage->cmd->stack, size, ti->alignment);

        ecs_xtor_t ctor = ti->hooks.ctor;
        if (ctor) {
            ctor(ptr.ptr, 1, ti);
        }
    } else {
        cmd->kind = EcsCmdAddModified;

        /* Call on_replace hook before copying the new value. */
        if (ti->hooks.on_replace) {
            flecs_invoke_replace_hook(
                world, r->table, entity, id, ptr.ptr, value, ti);
        }
    }

    return ptr.ptr;
error:
    return NULL;
}

void* flecs_defer_cpp_assign(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value)
{
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_component_ptr_t ptr = flecs_get_mut(world, entity, id, r, size);
    ecs_assert(ptr.ptr != NULL, ECS_INVALID_PARAMETER,
        "entity does not have component, use set() instead");

    if (id < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[id]) {
            return ptr.ptr; /* Nothing more to do */
        }
    }

    /* Call on_replace hook before copying the new value. */
    ecs_iter_action_t on_replace = ptr.ti->hooks.on_replace;
    if (on_replace) {
        flecs_invoke_replace_hook(
            world, r->table, entity, id, ptr.ptr, value, ptr.ti);
    }

    ecs_cmd_t *cmd = flecs_cmd_new(stage);
    cmd->kind = EcsCmdModified;
    cmd->entity = entity;
    cmd->id = id;

    return ptr.ptr;
}

void flecs_enqueue(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_event_desc_t *desc)
{
    ecs_cmd_t *cmd = flecs_cmd_new(stage);
    cmd->kind = EcsCmdEvent;
    cmd->entity = desc->entity;

    ecs_stack_t *stack = &stage->cmd->stack;
    ecs_event_desc_t *desc_cmd = flecs_stack_alloc_t(stack, ecs_event_desc_t);
    ecs_os_memcpy_t(desc_cmd, desc, ecs_event_desc_t);

    if (desc->ids && desc->ids->count != 0) {
        ecs_type_t *type_cmd = flecs_stack_alloc_t(stack, ecs_type_t);
        int32_t id_count = desc->ids->count;
        type_cmd->count = id_count;
        type_cmd->array = flecs_stack_alloc_n(stack, ecs_id_t, id_count);
        ecs_os_memcpy_n(type_cmd->array, desc->ids->array, ecs_id_t, id_count);
        desc_cmd->ids = type_cmd;
    } else {
        desc_cmd->ids = NULL;
    }

    cmd->is._1.value = desc_cmd;
    cmd->is._1.size = ECS_SIZEOF(ecs_event_desc_t);

    if (desc->param || desc->const_param) {
        ecs_assert(!(desc->const_param && desc->param), ECS_INVALID_PARAMETER, 
            "cannot set param and const_param at the same time");

        const ecs_type_info_t *ti = ecs_get_type_info(world, desc->event);
        ecs_assert(ti != NULL, ECS_INVALID_PARAMETER, 
            "can only enqueue events with data for events that are components");

        void *param_cmd = flecs_stack_alloc(stack, ti->size, ti->alignment);
        ecs_assert(param_cmd != NULL, ECS_INTERNAL_ERROR, NULL);
        if (desc->param) {
            if (ti->hooks.move_ctor) {
                ti->hooks.move_ctor(param_cmd, desc->param, 1, ti);
            } else {
                ecs_os_memcpy(param_cmd, desc->param, ti->size);
            }
        } else {
            if (ti->hooks.copy_ctor) {
                ti->hooks.copy_ctor(param_cmd, desc->const_param, 1, ti);
            } else {
                ecs_os_memcpy(param_cmd, desc->const_param, ti->size);
            }
        }

        desc_cmd->param = param_cmd;
        desc_cmd->const_param = NULL;
    }
}

static
void flecs_flush_bulk_new(
    ecs_world_t *world,
    ecs_cmd_t *cmd)
{
    ecs_entity_t *entities = cmd->is._n.entities;

    if (cmd->id) {
        int i, count = cmd->is._n.count;
        for (i = 0; i < count; i ++) {
            ecs_record_t *r = flecs_entities_ensure(world, entities[i]);
            if (!r->table) {
                flecs_add_to_root_table(world, entities[i]);
            }
            flecs_add_id(world, entities[i], cmd->id);
        }
    }

    ecs_os_free(entities);
}

static
void flecs_dtor_value(
    ecs_world_t *world,
    ecs_id_t id,
    void *value)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_xtor_t dtor = ti->hooks.dtor;
    if (dtor) {
        dtor(value, 1, ti);
    }
}

static
void flecs_free_cmd_event(
    ecs_world_t *world,
    ecs_event_desc_t *desc)
{
    if (desc->ids) {
        flecs_stack_free_n(desc->ids->array, ecs_id_t, desc->ids->count);
    }

    /* Safe const cast, command makes a copy of type object */
    flecs_stack_free_t(ECS_CONST_CAST(ecs_type_t*, desc->ids), 
        ecs_type_t);

    if (desc->param) {
        flecs_dtor_value(world, desc->event, 
            /* Safe const cast, command makes copy of value */
            ECS_CONST_CAST(void*, desc->param));
    }
}

static
void flecs_discard_cmd(
    ecs_world_t *world,
    ecs_cmd_t *cmd)
{
    if (cmd->kind == EcsCmdBulkNew) {
        ecs_os_free(cmd->is._n.entities);
    } else if (cmd->kind == EcsCmdEvent) {
        flecs_free_cmd_event(world, cmd->is._1.value);
    } else {
        ecs_assert(cmd->kind != EcsCmdEvent, ECS_INTERNAL_ERROR, NULL);
        void *value = cmd->is._1.value;
        if (value) {
            flecs_dtor_value(world, cmd->id, value);
            flecs_stack_free(value, cmd->is._1.size);
        }
    }
}

static
bool flecs_remove_invalid(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_t *id_out)
{
    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        if (!flecs_entities_is_valid(world, rel)) {
            /* After relationship is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        } else {
            ecs_entity_t tgt = ECS_PAIR_SECOND(id);
            if (!flecs_entities_is_valid(world, tgt)) {
                /* Check the relationship's policy for deleted objects */
                ecs_component_record_t *cr = flecs_components_get(world, 
                    ecs_pair(rel, EcsWildcard));
                if (cr) {
                    ecs_entity_t action = ECS_ID_ON_DELETE_TARGET(cr->flags);
                    if (action == EcsDelete) {
                        /* Entity should be deleted, don't bother checking
                         * other ids */
                        return false;
                    } else if (action == EcsPanic) {
                        /* If policy is throw this object should not have
                         * been deleted */
                        flecs_throw_invalid_delete(world, id);
                    } else {
                        *id_out = 0;
                        return true;
                    }
                } else {
                    *id_out = 0;
                    return true;
                }
            }
        }
    } else {
        id &= ECS_COMPONENT_MASK;
        if (!flecs_entities_is_valid(world, id)) {
            /* After relationship is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        }
    }

    return true;
}

static
void flecs_cmd_batch_for_entity(
    ecs_world_t *world,
    ecs_table_diff_builder_t *diff,
    ecs_entity_t entity,
    ecs_cmd_t *cmds,
    int32_t start)
{
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    world->info.cmd.batched_entity_count ++;

    bool has_set = false;
    ecs_table_t *start_table = table;
    ecs_table_diff_t table_diff; /* Keep track of diff for observers/hooks */
    int32_t cur = start;
    int32_t next_for_entity;

    do {
        ecs_cmd_t *cmd = &cmds[cur];
        ecs_id_t id = cmd->id;
        next_for_entity = cmd->next_for_entity;
        if (next_for_entity < 0) {
            /* First command for an entity has a negative index, flip sign */
            next_for_entity *= -1;
        }

        /* Check if added id is still valid (like is the parent of a ChildOf 
         * pair still alive), if not run cleanup actions for entity */
        if (id) {
            ecs_component_record_t *cr = NULL;
            if ((id < FLECS_HI_COMPONENT_ID)) {
                if (world->non_trivial_lookup[id]) {
                    cr = flecs_components_get(world, id);
                }
            } else {
                cr = flecs_components_get(world, id);
            }
                    
            if (cr && cr->flags & EcsIdDontFragment) {
                /* Nothing to batch for non-fragmenting components */
                continue;
            }

            if (flecs_remove_invalid(world, id, &id)) {
                if (!id) {
                    /* Entity should remain alive but id should not be added */
                    cmd->kind = EcsCmdSkip;
                    continue;
                }
                /* Entity should remain alive and id is still valid */
            } else {
                /* Id was no longer valid and had a Delete policy */
                cmd->kind = EcsCmdSkip;
                ecs_delete(world, entity);
                flecs_table_diff_builder_clear(diff);
                return;
            }
        }

        ecs_cmd_kind_t kind = cmd->kind;
        switch(kind) {
        case EcsCmdAddModified:
            /* Add is batched, but keep Modified */
            cmd->kind = EcsCmdModified;
            table = flecs_find_table_add(world, table, id, diff);
            world->info.cmd.batched_command_count ++;
            break;
        case EcsCmdAdd:
            table = flecs_find_table_add(world, table, id, diff);
            world->info.cmd.batched_command_count ++;
            cmd->kind = EcsCmdSkip;
            break;
        case EcsCmdSet:
        case EcsCmdEnsure: {
            table = flecs_find_table_add(world, table, id, diff);
            world->info.cmd.batched_command_count ++;
            has_set = true;
            break;
        }
        case EcsCmdEmplace:
            /* Don't add for emplace, as this requires a special call to ensure
             * the constructor is not invoked for the component */
            break;
        case EcsCmdRemove: {
            table = flecs_find_table_remove(world, table, id, diff);
            world->info.cmd.batched_command_count ++;
            cmd->kind = EcsCmdSkip;
            break;
        }
        case EcsCmdClear:
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
            if (table->type.count) {
                ecs_id_t *ids = ecs_vec_grow_t(&world->allocator, 
                    &diff->removed, ecs_id_t, table->type.count);
                ecs_os_memcpy_n(ids, table->type.array, ecs_id_t, 
                    table->type.count);
                diff->removed_flags |= table->flags & EcsTableRemoveEdgeFlags;
            }
            table = &world->store.root;
            world->info.cmd.batched_command_count ++;
            cmd->kind = EcsCmdSkip;
            break;
        case EcsCmdClone:
        case EcsCmdBulkNew:
        case EcsCmdPath:
        case EcsCmdDelete:
        case EcsCmdOnDeleteAction:
        case EcsCmdEnable:
        case EcsCmdDisable:
        case EcsCmdEvent:
        case EcsCmdSkip:
        case EcsCmdModifiedNoHook:
        case EcsCmdModified:
            break;
        }
    } while ((cur = next_for_entity));

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Invoke OnAdd handlers after commit. This ensures that observers with 
     * mixed OnAdd/OnSet events won't get called with uninitialized values for
     * an OnSet field. */
    ecs_type_t added = { diff->added.array, diff->added.count };
    diff->added.array = NULL;
    diff->added.count = 0;

    /* Move entity to destination table in single operation */
    flecs_table_diff_build_noalloc(diff, &table_diff);
    flecs_defer_begin(world, world->stages[0]);
    flecs_commit(world, entity, r, table, &table_diff, true, 0);
    flecs_defer_end(world, world->stages[0]);

    /* If destination table has new sparse components, make sure they're created
     * for the entity. */
    if ((table_diff.added_flags & (EcsTableHasSparse|EcsTableHasDontFragment)) && added.count) {
        flecs_sparse_on_add(
            world, table, ECS_RECORD_TO_ROW(r->row), 1, &added, true);
    }

    /* If the batch contains set commands, copy the component value from the 
     * temporary command storage to the actual component storage before OnSet
     * observers are invoked. This ensures that for multi-component OnSet 
     * observers all components are assigned a valid value before the observer
     * is invoked. 
     * This only happens for entities that didn't have the assigned component
     * yet, as for entities that did have the component already the value will
     * have been assigned directly to the component storage. */
    if (has_set) {
        cur = start;
        do {
            ecs_cmd_t *cmd = &cmds[cur];
            next_for_entity = cmd->next_for_entity;
            if (next_for_entity < 0) {
                next_for_entity *= -1;
            }

            switch(cmd->kind) {
            case EcsCmdSet:
            case EcsCmdEnsure: {
                flecs_component_ptr_t dst = flecs_get_mut(
                    world, entity, cmd->id, r, cmd->is._1.size);

                /* It's possible that even though the component was set, the
                 * command queue also contained a remove command, so before we
                 * do anything ensure the entity actually has the component. */
                if (dst.ptr) {
                    void *ptr = cmd->is._1.value;
                    const ecs_type_info_t *ti = dst.ti;
                    if (ti->hooks.on_replace) {
                        ecs_table_t *prev_table = r->table;
                        flecs_invoke_replace_hook(world, prev_table, entity, 
                            cmd->id, dst.ptr, ptr, ti);
                        if (prev_table != r->table) {
                            if (!r->table) {
                                /* Entity was deleted */
                                goto done;
                            }
                            dst = flecs_get_mut(
                                world, entity, cmd->id, r, cmd->is._1.size);
                        }
                    }

                    ecs_move_t move = ti->hooks.move;
                    if (move) {
                        move(dst.ptr, ptr, 1, ti);
                        ecs_xtor_t dtor = ti->hooks.dtor;
                        if (dtor) {
                            dtor(ptr, 1, ti);
                        }
                    } else {
                        ecs_os_memcpy(dst.ptr, ptr, ti->size);
                    }

                    flecs_stack_free(ptr, cmd->is._1.size);
                    cmd->is._1.value = NULL;

                    if (cmd->kind == EcsCmdSet) {
                        /* A set operation is add + copy + modified. We just did
                         * the add and copy, so the only thing that's left is a 
                         * modified command, which will call the OnSet 
                         * observers. */
                        cmd->kind = EcsCmdModified;
                    } else {
                        /* If this was an ensure, nothing's left to be done */
                        cmd->kind = EcsCmdSkip;
                    }
                } else {
                    /* The entity no longer has the component which means that
                     * there was a remove command for the component in the
                     * command queue. In that case skip the command. */
                    cmd->kind = EcsCmdSkip;
                }
                break;
            }
            case EcsCmdRemove:
            case EcsCmdClone:
            case EcsCmdBulkNew:
            case EcsCmdAdd:
            case EcsCmdEmplace:
            case EcsCmdModified:
            case EcsCmdModifiedNoHook:
            case EcsCmdAddModified:
            case EcsCmdPath:
            case EcsCmdDelete:
            case EcsCmdClear:
            case EcsCmdOnDeleteAction:
            case EcsCmdEnable:
            case EcsCmdDisable:
            case EcsCmdEvent:
            case EcsCmdSkip:
                break;
            }
        } while ((cur = next_for_entity));
    }

    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (added.count) {
        ecs_table_diff_t add_diff = ECS_TABLE_DIFF_INIT;
        add_diff.added = added;
        add_diff.added_flags = diff->added_flags;

        if (r->row & EcsEntityIsTraversable) {
            /* Update monitors since we didn't do this in flecs_commit. Do this
             * before calling flecs_actions_move_add() since this can trigger 
             * prefab instantiation logic. When that happens, prefab children
             * can be created for this instance which would mean that the table
             * count of r->cr would always be >0.
             * Since those tables are new, we don't have to invoke component 
             * monitors since queries will have correctly matched them. */
            ecs_component_record_t *cr = flecs_components_get(
                world, ecs_pair(EcsWildcard, entity));
            ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
            if (ecs_map_count(&cr->cache.index)) {
                flecs_update_component_monitors(world, &added, NULL);
            }
        }

        flecs_defer_begin(world, world->stages[0]);
        flecs_actions_move_add(world, r->table, start_table,
            ECS_RECORD_TO_ROW(r->row), 1, &add_diff, 0, true, false);
        flecs_defer_end(world, world->stages[0]);
    }

done:
    diff->added.array = added.array;
    diff->added.count = added.count;
    flecs_table_diff_builder_clear(diff);
}

/* Leave safe section. Run all deferred commands. */
bool flecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    flecs_poly_assert(world, ecs_world_t);
    flecs_poly_assert(stage, ecs_stage_t);

    flecs_check_exclusive_world_access_write(world);

    if (stage->defer < 0) {
        /* Defer suspending makes it possible to do operations on the storage
         * without flushing the commands in the queue */
        return false;
    }

    ecs_assert(stage->defer > 0, ECS_INTERNAL_ERROR, NULL);

    if (!--stage->defer && !stage->cmd_flushing) {
        ecs_os_perf_trace_push("flecs.commands.merge");

        /* Test whether we're flushing to another queue or whether we're 
         * flushing to the storage */
        bool merge_to_world = false;
        if (flecs_poly_is(world, ecs_world_t)) {
            merge_to_world = world->stages[0]->defer == 0;
        }

        do {
            ecs_stage_t *dst_stage = flecs_stage_from_world(&world);
            ecs_commands_t *commands = stage->cmd;
            ecs_vec_t *queue = &commands->queue;

            if (stage->cmd == &stage->cmd_stack[0]) {
                stage->cmd = &stage->cmd_stack[1];
            } else {
                stage->cmd = &stage->cmd_stack[0];
            }

            if (!ecs_vec_count(queue)) {
                break;
            }

            stage->cmd_flushing = true;

            /* Internal callback for capturing commands */
            if (world->on_commands_active) {
                world->on_commands_active(stage, queue, 
                    world->on_commands_ctx_active);
            }

            ecs_cmd_t *cmds = ecs_vec_first(queue);
            int32_t i, count = ecs_vec_count(queue);

            ecs_table_diff_builder_t diff = {0};
            bool diff_builder_used = false;

            for (i = 0; i < count; i ++) {
                ecs_cmd_t *cmd = &cmds[i];
                ecs_entity_t e = cmd->entity;
                bool is_alive = flecs_entities_is_alive(world, e);

                /* A negative index indicates the first command for an entity */
                if (merge_to_world && (cmd->next_for_entity < 0)) {
                    diff.added_flags = 0;
                    diff.removed_flags = 0;

                    /* Batch commands for entity to limit archetype moves */
                    if (is_alive) {
                        if (!diff_builder_used) {
                            flecs_table_diff_builder_init(world, &diff);
                            diff_builder_used = true;
                        }

                        flecs_cmd_batch_for_entity(world, &diff, e, cmds, i);
                    } else {
                        world->info.cmd.discard_count ++;
                    }
                }

                /* Invalidate entry */
                if (cmd->entry) {
                    cmd->entry->first = -1;
                }

                /* If entity is no longer alive, this could be because the queue
                 * contained both a delete and a subsequent add/remove/set which
                 * should be ignored. */
                ecs_cmd_kind_t kind = cmd->kind;
                if ((kind != EcsCmdPath) && ((kind == EcsCmdSkip) || (e && !is_alive))) {
                    world->info.cmd.discard_count ++;
                    flecs_discard_cmd(world, cmd);
                    continue;
                }

                ecs_id_t id = cmd->id;

                switch(kind) {
                case EcsCmdAdd:
                    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
                    if (flecs_remove_invalid(world, id, &id)) {
                        if (id) {
                            world->info.cmd.add_count ++;
                            flecs_add_id(world, e, id);
                        } else {
                            world->info.cmd.discard_count ++;
                        }
                    } else {
                        world->info.cmd.discard_count ++;
                        ecs_delete(world, e);
                    }
                    break;
                case EcsCmdRemove:
                    flecs_remove_id(world, e, id);
                    world->info.cmd.remove_count ++;
                    break;
                case EcsCmdClone:
                    ecs_clone(world, e, id, cmd->is._1.clone_value);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdSet:
                    flecs_set_id_move(world, dst_stage, e, 
                        cmd->id, flecs_itosize(cmd->is._1.size), 
                        cmd->is._1.value, kind);
                    world->info.cmd.set_count ++;
                    break;
                case EcsCmdEmplace:
                    if (merge_to_world) {
                        bool is_new;
                        ecs_emplace_id(world, e, id, 
                            flecs_itosize(cmd->is._1.size), &is_new);
                        if (!is_new) {
                            kind = EcsCmdEnsure;
                        }
                    }
                    flecs_set_id_move(world, dst_stage, e, 
                        cmd->id, flecs_itosize(cmd->is._1.size), 
                        cmd->is._1.value, kind);
                    world->info.cmd.ensure_count ++;
                    break;
                case EcsCmdEnsure:
                    flecs_set_id_move(world, dst_stage, e, 
                        cmd->id, flecs_itosize(cmd->is._1.size), 
                        cmd->is._1.value, kind);
                    world->info.cmd.ensure_count ++;
                    break;
                case EcsCmdModified:
                    flecs_modified_id_if(world, e, id, true);
                    world->info.cmd.modified_count ++;
                    break;
                case EcsCmdModifiedNoHook:
                    flecs_modified_id_if(world, e, id, false);
                    world->info.cmd.modified_count ++;
                    break;
                case EcsCmdAddModified:
                    flecs_add_id(world, e, id);
                    flecs_modified_id_if(world, e, id, true);
                    world->info.cmd.set_count ++;
                    break;
                case EcsCmdDelete: {
                    ecs_delete(world, e);
                    world->info.cmd.delete_count ++;
                    break;
                }
                case EcsCmdClear:
                    ecs_clear(world, e);
                    world->info.cmd.clear_count ++;
                    break;
                case EcsCmdOnDeleteAction:
                    ecs_defer_begin(world);
                    flecs_on_delete(world, id, e, false, false);
                    ecs_defer_end(world);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdEnable:
                    ecs_enable_id(world, e, id, true);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdDisable:
                    ecs_enable_id(world, e, id, false);
                    world->info.cmd.other_count ++;
                    break;
                case EcsCmdBulkNew:
                    flecs_flush_bulk_new(world, cmd);
                    world->info.cmd.other_count ++;
                    continue;
                case EcsCmdPath: {
                    bool keep_alive = true;
                    ecs_make_alive(world, e);
                    if (cmd->id) {
                        if (ecs_is_alive(world, cmd->id)) {
                            ecs_add_pair(world, e, EcsChildOf, cmd->id);
                        } else {
                            ecs_delete(world, e);
                            keep_alive = false;
                        }
                    }
                    if (keep_alive) {
                        ecs_set_name(world, e, cmd->is._1.value);
                    }
                    ecs_os_free(cmd->is._1.value);
                    cmd->is._1.value = NULL;
                    world->info.cmd.other_count ++;
                    break;
                }
                case EcsCmdEvent: {
                    ecs_event_desc_t *desc = cmd->is._1.value;
                    ecs_assert(desc != NULL, ECS_INTERNAL_ERROR, NULL);
                    ecs_emit((ecs_world_t*)stage, desc);
                    flecs_free_cmd_event(world, desc);
                    world->info.cmd.event_count ++;
                    break;
                }
                case EcsCmdSkip:
                    break;
                }

                if (cmd->is._1.value) {
                    flecs_stack_free(cmd->is._1.value, cmd->is._1.size);
                }
            }

            stage->cmd_flushing = false;

            flecs_stack_reset(&commands->stack);
            ecs_vec_clear(queue);

            if (diff_builder_used) {
                flecs_table_diff_builder_fini(world, &diff);
            }

            /* Internal callback for capturing commands, signal queue is done */
            if (world->on_commands_active) {
                world->on_commands_active(stage, NULL, 
                    world->on_commands_ctx_active);
            }
        } while (true);

        ecs_os_perf_trace_pop("flecs.commands.merge");

        return true;
    }

    return false;
}

/* Delete operations from queue without executing them. */
bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!--stage->defer) {
        ecs_vec_t commands = stage->cmd->queue;

        if (ecs_vec_count(&commands)) {
            ecs_cmd_t *cmds = ecs_vec_first(&commands);
            int32_t i, count = ecs_vec_count(&commands);
            for (i = 0; i < count; i ++) {
                flecs_discard_cmd(world, &cmds[i]);
            }

            ecs_vec_fini_t(&stage->allocator, &stage->cmd->queue, ecs_cmd_t);

            ecs_vec_clear(&commands);
            flecs_stack_reset(&stage->cmd->stack);
            flecs_sparse_clear(&stage->cmd->entries);
        }

        return true;
    }

error:
    return false;
}

void flecs_commands_init(
    ecs_stage_t *stage,
    ecs_commands_t *cmd)
{
    flecs_stack_init(&cmd->stack);
    ecs_vec_init_t(&stage->allocator, &cmd->queue, ecs_cmd_t, 0);
    flecs_sparse_init_t(&cmd->entries, &stage->allocator,
        &stage->allocators.cmd_entry_chunk, ecs_cmd_entry_t);
}

void flecs_commands_fini(
    ecs_stage_t *stage,
    ecs_commands_t *cmd)
{
    /* Make sure stage has no unmerged data */
    ecs_assert(ecs_vec_count(&cmd->queue) == 0, ECS_INTERNAL_ERROR, NULL);

    flecs_stack_fini(&cmd->stack);
    ecs_vec_fini_t(&stage->allocator, &cmd->queue, ecs_cmd_t);
    flecs_sparse_fini(&cmd->entries);
}

bool ecs_defer_begin(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    return flecs_defer_begin(world, stage);
error:
    return false;
}

bool ecs_defer_end(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    return flecs_defer_end(world, stage);
error:
    return false;
}

void ecs_defer_suspend(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_is_deferred(world), ECS_INVALID_OPERATION, 
        "world/stage must be deferred before it can be suspended");
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_check(stage->defer > 0, ECS_INVALID_OPERATION, 
        "world/stage is already suspended");
    stage->defer = -stage->defer;
error:
    return;
}

void ecs_defer_resume(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_check(stage->defer < 0, ECS_INVALID_OPERATION,
        "world/stage must be suspended before it can be resumed");
    stage->defer = -stage->defer;
error:
    return;
}
