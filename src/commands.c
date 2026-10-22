/**
 * @file commands.c
 * @brief Command queue implementation.
 */

#include "private_api.h"

#define FLECS_CMD_REMOVED_MANY ((ecs_id_t)-1)

static ecs_table_t* flecs_find_table_remove(
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

static ecs_cmd_t* flecs_cmd_new(
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

static ecs_cmd_t* flecs_cmd_new_batched(
    ecs_stage_t *stage, 
    ecs_entity_t e,
    ecs_cmd_entry_t **entry_out)
{
    ecs_commands_t *commands = stage->cmd;
    ecs_vec_t *cmds = &commands->queue;
    int32_t cur = ecs_vec_count(cmds);
    if (!cur) {
        commands->entity = e;
        commands->entry.first = -1;
        commands->second_entity = 0;
        commands->last_entity = 0;
    }
    ecs_cmd_entry_t *entry;
    if (e == commands->last_entity) {
        entry = commands->last_entry;
    } else if (e == commands->entity) {
        entry = &commands->entry;
    } else if (e == commands->second_entity) {
        entry = &commands->second_entry;
    } else if (cur == 1) {
        commands->second_entity = e;
        commands->second_entry.first = -1;
        entry = &commands->second_entry;
    } else {
        entry = flecs_sparse_get_t(&commands->entries, ecs_cmd_entry_t, e);
    }

    ecs_cmd_t *cmd = flecs_cmd_new(stage);
    bool is_new = false;
    if (entry) {
        if (entry->first == -1) {
            /* Existing but invalidated entry */
            entry->first = cur;
            entry->removed = 0;
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
        entry->removed = 0;
    }

    entry->last = cur;
    commands->last_entity = e;
    commands->last_entry = entry;
    if (entry_out) {
        *entry_out = entry;
    }

    return cmd;
}

void flecs_commands_grow(
    ecs_stage_t *stage)
{
    ecs_commands_t *cmd = stage->cmd;
    ecs_assert(cmd->next == NULL, ECS_INTERNAL_ERROR, NULL);
    cmd->next = ecs_os_calloc_t(ecs_commands_t);
    flecs_commands_init(stage, cmd->next);
    cmd->next->prev = cmd;
}

bool flecs_defer_cmd(
    ecs_stage_t *stage,
    bool deferred)
{
    if (deferred) {
        return true;
    }
    flecs_check_exclusive_world_access_write(stage->world);
    flecs_commands_begin(stage->world, stage);
    return false;
}

bool flecs_defer_modified(
    ecs_stage_t *stage,
    bool deferred,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage, deferred)) {
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
    bool deferred,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value)
{   
    if (flecs_defer_cmd(stage, deferred)) {
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
    ecs_cmd_t *cmd = flecs_cmd_new(stage);
    cmd->kind = EcsCmdPath;
    cmd->entity = entity;
    cmd->id = parent;
    cmd->is._1.value = ecs_os_strdup(name);
    return true;
}

bool flecs_defer_delete(
    ecs_stage_t *stage,
    bool deferred,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage, deferred)) {
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity, NULL);
        cmd->kind = EcsCmdDelete;
        cmd->entity = entity;
        return true;
    }

    return false;
}

bool flecs_defer_clear(
    ecs_stage_t *stage,
    bool deferred,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage, deferred)) {
        ecs_cmd_entry_t *entry;
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity, &entry);
        cmd->kind = EcsCmdClear;
        cmd->entity = entity;
        entry->removed = FLECS_CMD_REMOVED_MANY;
        stage->cmd->has_remove = true;
        return true;
    }

    return false;
}

bool flecs_defer_on_delete_action(
    ecs_stage_t *stage,
    bool deferred,
    ecs_id_t id,
    ecs_entity_t action,
    bool force_delete)
{
    if (flecs_defer_cmd(stage, deferred)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdOnDeleteAction;
        cmd->id = id;
        cmd->entity = action;
        cmd->is._1.force_delete = force_delete;
        return true;
    }

    return false;
}

bool flecs_defer_enable(
    ecs_stage_t *stage,
    bool deferred,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    if (flecs_defer_cmd(stage, deferred)) {
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
    bool deferred,
    int32_t count,
    ecs_id_t id,
    const ecs_entity_t **ids_out)
{
    if (flecs_defer_cmd(stage, deferred)) {
        ecs_entity_t *ids = ecs_os_malloc(count * ECS_SIZEOF(ecs_entity_t));

        /* Use ecs_new as this is thread safe */
        int i;
        for (i = 0; i < count; i ++) {
            ids[i] = ecs_new(world);
        }

        *ids_out = ids;

        /* Store data in cmd */
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
    bool deferred,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage, deferred)) {
        ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity, NULL);
        cmd->kind = EcsCmdAdd;
        cmd->id = id;
        cmd->entity = entity;
        return true;
    }

    return false;
}

bool flecs_defer_remove(
    ecs_stage_t *stage,
    bool deferred,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage, deferred)) {
        ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_cmd_entry_t *entry;
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity, &entry);
        cmd->kind = EcsCmdRemove;
        cmd->id = id;
        cmd->entity = entity;
        if ((!entry->removed || entry->removed == id) &&
            !ecs_id_is_wildcard(id))
        {
            entry->removed = id;
        } else {
            entry->removed = FLECS_CMD_REMOVED_MANY;
        }
        stage->cmd->has_remove = true;
        return true;
    }

    return false;
}

static ecs_cmd_entry_t* flecs_cmd_entry_get(
    ecs_commands_t *commands,
    ecs_entity_t entity)
{
    ecs_cmd_entry_t *entry = entity == commands->last_entity ?
        commands->last_entry :
        entity == commands->entity ? &commands->entry :
        entity == commands->second_entity ? &commands->second_entry :
        flecs_sparse_get_t(&commands->entries, ecs_cmd_entry_t, entity);
    if (!entry || entry->first == -1) {
        return NULL;
    }

    ecs_cmd_t *cmds = ecs_vec_first_t(&commands->queue, ecs_cmd_t);
    if (cmds[entry->last].entity != entity) {
        return NULL;
    }

    return entry;
}

static bool flecs_defer_has_remove(
    ecs_commands_t *commands,
    const ecs_cmd_entry_t *entry,
    ecs_id_t id)
{
    ecs_cmd_t *cmds = ecs_vec_first_t(&commands->queue, ecs_cmd_t);
    int32_t cur = entry->first;
    do {
        ecs_cmd_t *cmd = &cmds[cur];
        if (cmd->kind == EcsCmdClear) {
            return true;
        }
        if (cmd->kind == EcsCmdRemove && ecs_id_match(id, cmd->id)) {
            return true;
        }

        int32_t next = cmd->next_for_entity;
        cur = next < 0 ? -next : next;
    } while (cur);

    return false;
}

static FLECS_ALWAYS_INLINE bool flecs_defer_skip_removed(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    flecs_component_ptr_t *ptr)
{
    ecs_commands_t *commands = stage->cmd;
    if (!ptr->ptr || !commands->has_remove) {
        return false;
    }

    const ecs_cmd_entry_t *entry = flecs_cmd_entry_get(commands, entity);
    if (!entry || !entry->removed) {
        return false;
    }

    ecs_id_t removed = entry->removed;
    if (removed != FLECS_CMD_REMOVED_MANY) {
        if (id != removed) {
            return false;
        }
    } else if (!flecs_defer_has_remove(commands, entry, id)) {
        return false;
    }

    ptr->ptr = NULL;
    return true;
}

/* Return existing component pointer & type info */
static flecs_component_ptr_t flecs_defer_get_existing(
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
                /* When not in multithreaded mode, it's safe to find or
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
    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t ptr = flecs_defer_get_existing(
        world, entity, r, id, size);
    flecs_defer_skip_removed(stage, entity, id, &ptr);

    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity, NULL);
    cmd->entity = entity;
    cmd->id = id;

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

static
void* flecs_defer_find_cmd_value(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_commands_t *commands = stage->cmd;
    ecs_cmd_entry_t *entry = flecs_cmd_entry_get(commands, entity);
    if (!entry) {
        return NULL;
    }

    ecs_cmd_t *cmds = ecs_vec_first_t(&commands->queue, ecs_cmd_t);
    void *result = NULL;
    int32_t cur = entry->first;
    do {
        ecs_cmd_t *cmd = &cmds[cur];
        switch(cmd->kind) {
        case EcsCmdSet:
        case EcsCmdSetDontFragment:
        case EcsCmdEmplace:
        case EcsCmdEnsure:
        case EcsCmdEnsureDontFragment:
            if (cmd->id == id) {
                result = cmd->is._1.value;
            }
            break;
        case EcsCmdRemove:
            if (ecs_id_match(id, cmd->id)) {
                result = NULL;
            }
            break;
        case EcsCmdClone:
        case EcsCmdClear:
        case EcsCmdDelete:
            result = NULL;
            break;
        case EcsCmdBulkNew:
        case EcsCmdAdd:
        case EcsCmdModified:
        case EcsCmdModifiedNoHook:
        case EcsCmdAddModified:
        case EcsCmdPath:
        case EcsCmdOnDeleteAction:
        case EcsCmdEnable:
        case EcsCmdDisable:
        case EcsCmdEvent:
        case EcsCmdSkip:
            break;
        }

        int32_t next = cmd->next_for_entity;
        cur = next < 0 ? -next : next;
    } while (cur);

    return result;
}

void* flecs_defer_ensure(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size)
{
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t ptr = flecs_defer_get_existing(
        world, entity, r, id, size);
    bool removed = flecs_defer_skip_removed(stage, entity, id, &ptr);

    const ecs_type_info_t *ti = ptr.ti;
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER,
        "provided component is not a type");
    ecs_assert(size == ti->size, ECS_INVALID_PARAMETER,
        "bad size for component in ensure");

    bool use_cmd_storage = !ptr.ptr || ti->hooks.on_replace != NULL;

    if (use_cmd_storage) {
        void *existing = flecs_defer_find_cmd_value(stage, entity, id);
        if (existing) {
            return existing;
        }
    }

    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity, NULL);
    cmd->entity = entity;
    cmd->id = id;

    ecs_table_t *table = r->table;
    if (use_cmd_storage) {
        ecs_stack_t *stack = &stage->cmd->stack;
        void *value = flecs_stack_alloc(stack, size, ti->alignment);
        cmd->kind = EcsCmdEnsure;
        cmd->is._1.size = size;
        cmd->is._1.value = value;

        if (ptr.ptr) {
            flecs_type_info_copy_ctor(value, ptr.ptr, 1, ti);
        } else {
            /* Check if entity inherits component */
            void *base = NULL;
            if (table && (table->flags & EcsTableHasIsA)) {
                ecs_component_record_t *cr = flecs_components_get(world, id);
                if (removed) {
                    ecs_ref_t storage;
                    const ecs_ref_t *o = flecs_table_get_override(
                        world, table, id, cr, &storage);
                    if (o) {
                        base = ecs_ref_get_id(
                            world, ECS_CONST_CAST(ecs_ref_t*, o), id);
                    }
                }

                if (!base) {
                    base = flecs_get_base_component(world, table, id, cr, 0);
                }
            }

            if (!base) {
                /* Normal ctor */
                flecs_type_info_ctor(value, 1, ti);
            } else {
                /* Override */
                flecs_type_info_copy_ctor(value, base, 1, ti);
            }
        }

        ptr.ptr = value;
    } else {
        cmd->kind = EcsCmdAdd;
    }

    return ptr.ptr;
error:
    return NULL;
}

static FLECS_ALWAYS_INLINE void* flecs_defer_set_impl(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value,
    bool copy)
{
    ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *r = flecs_entities_get(world, entity);
    flecs_component_ptr_t ptr = flecs_defer_get_existing(
        world, entity, r, id, size);
    flecs_defer_skip_removed(stage, entity, id, &ptr);

    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity, NULL);
    ecs_assert(cmd != NULL, ECS_INTERNAL_ERROR, NULL);
    cmd->entity = entity;
    cmd->id = id;

    if (copy && world->stage_count != 1) {

        ptr.ptr = NULL;
    }

    const ecs_type_info_t *ti = ptr.ti;
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER,
        "provided component is not a type");
    ecs_assert(size == ti->size, ECS_INVALID_PARAMETER,
        "mismatching size specified for component in ensure/emplace/set (%u vs %u)",
            size, ti->size);

    if (id < FLECS_HI_COMPONENT_ID) {
        if (!world->non_trivial_set[id]) {
            if (!ptr.ptr) {
                ptr.ptr = flecs_stack_alloc(
                    &stage->cmd->stack, size, ti->alignment);

                cmd->kind = EcsCmdEnsure;
                cmd->is._1.size = size;
                cmd->is._1.value = ptr.ptr;
            } else {

                cmd->kind = EcsCmdAdd;
            }

            ecs_os_memcpy(ptr.ptr, value, size);
            return ptr.ptr;
        }
    }

    if (!ptr.ptr) {
        bool is_dont_fragment =
            flecs_component_get_flags(world, id) & EcsIdDontFragment;
        cmd->kind = is_dont_fragment ? EcsCmdSetDontFragment : EcsCmdSet;
        cmd->is._1.size = size;
        ptr.ptr = cmd->is._1.value =
            flecs_stack_alloc(&stage->cmd->stack, size, ti->alignment);
        if (copy) {
            flecs_type_info_copy_ctor(ptr.ptr, value, 1, ti);
        } else {
            flecs_type_info_ctor(ptr.ptr, 1, ti);
        }
    } else {
        cmd->kind = EcsCmdAddModified;

        if (ti->hooks.on_replace) {
            flecs_invoke_replace_hook(
                world, r->table, entity, id, ptr.ptr, value, ti);
        }

        if (copy) {
            ECS_TABLE_LOCK(world, r->table);
            flecs_type_info_copy(ptr.ptr, value, 1, ti);
            ECS_TABLE_UNLOCK(world, r->table);
        }
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
    return flecs_defer_set_impl(world, stage, entity, id, size, value, true);
}

void* flecs_defer_cpp_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value)
{
    return flecs_defer_set_impl(world, stage, entity, id, size, value, false);
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
            flecs_type_info_move_ctor(param_cmd, desc->param, 1, ti);
        } else {
            flecs_type_info_copy_ctor(param_cmd, desc->const_param, 1, ti);
        }

        desc_cmd->param = param_cmd;
        desc_cmd->const_param = NULL;
    }
}

static void flecs_flush_bulk_new(
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

static void flecs_dtor_value(
    ecs_world_t *world,
    ecs_id_t id,
    void *value)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, id);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_type_info_dtor(value, 1, ti);
}

static void flecs_free_cmd_event(
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
        const ecs_type_info_t *ti = ecs_get_type_info(world, desc->event);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Safe const cast, command makes copy of value */
        void *param = ECS_CONST_CAST(void*, desc->param);
        flecs_type_info_dtor(param, 1, ti);
        flecs_stack_free(param, ti->size);
    }
}

static void flecs_discard_cmd(
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

static bool flecs_remove_invalid(
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
                /* Check the relationship's policy for deleted targets */
                ecs_component_record_t *cr = flecs_components_get(world, 
                    ecs_pair(rel, EcsWildcard));
                if (cr) {
                    ecs_entity_t action = ECS_ID_ON_DELETE_TARGET(cr->flags);
                    if (action == EcsDelete) {
                        /* Entity should be deleted, don't bother checking
                         * other ids */
                        return false;
                    } else if (action == EcsPanic) {
                        /* If policy is Panic, this target should not have
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
            /* After component is deleted we can no longer see what its
             * delete action was, so pretend this never happened */
            *id_out = 0;
            return true;
        }
    }

    return true;
}

static bool flecs_cmd_table_has_id(
    const ecs_table_t *table,
    ecs_id_t id,
    ecs_component_record_t *cr)
{
    if (id < FLECS_HI_COMPONENT_ID) {
        return table->component_map[id] != 0;
    }

    return cr && flecs_component_get_table(cr, table) != NULL;
}

static void flecs_cmd_batch_for_entity(
    ecs_world_t *world,
    ecs_table_diff_builder_t *diff,
    ecs_vec_t *set_ids,
    ecs_entity_t entity,
    ecs_cmd_t *cmds,
    int32_t start)
{
    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    world->info.cmd.batched_entity_count ++;

    ecs_vec_clear(set_ids);

    bool has_set = false;
    ecs_table_t *start_table = table;
    ecs_table_diff_t table_diff; /* Keep track of diff for observers/hooks */
    int32_t cur = start;
    int32_t next_for_entity;
    int32_t split = 0;

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
        ecs_component_record_t *cr = NULL;
        if (id) {
            if ((id < FLECS_HI_COMPONENT_ID)) {
                if (world->non_trivial_lookup[id]) {
                    cr = flecs_components_get(world, id);
                }
            } else {
                cr = flecs_components_get(world, id);
            }
                    
            if (cr && cr->flags & EcsIdDontFragment) {
                /* Nothing to batch for non-fragmenting components */
                if (cmd->kind == EcsCmdSet) {
                    cmd->kind = EcsCmdSetDontFragment;
                } else if (cmd->kind == EcsCmdEnsure) {
                    cmd->kind = EcsCmdEnsureDontFragment;
                }

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
        bool in_table = false;
        if (kind == EcsCmdAdd || kind == EcsCmdAddModified ||
            kind == EcsCmdSet || kind == EcsCmdEnsure)
        {
            if (id < FLECS_HI_COMPONENT_ID) {
                in_table = table->component_map[id] != 0;
            } else if (diff->removed.count) {
                in_table = flecs_cmd_table_has_id(table, id, cr);
            }

            if (!in_table && diff->removed.count &&
                flecs_cmd_table_has_id(start_table, id, cr))
            {
                split = cur;
                break;
            }
        }

        switch(kind) {
        case EcsCmdAddModified:
            /* Add is batched, but keep Modified */
            cmd->kind = EcsCmdModified;
            if (!in_table) {
                table = flecs_find_table_add(world, table, id, diff);
            }
            world->info.cmd.batched_command_count ++;
            break;
        case EcsCmdAdd:
            if (!in_table) {
                table = flecs_find_table_add(world, table, id, diff);
            }
            world->info.cmd.batched_command_count ++;
            cmd->kind = EcsCmdSkip;
            break;
        case EcsCmdSet:
        case EcsCmdEnsure: {
            if (!in_table) {
                table = flecs_find_table_add(world, table, id, diff);
            }
            world->info.cmd.batched_command_count ++;
            ecs_vec_append_t(&world->allocator, set_ids, ecs_id_t)[0] = id;
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
        case EcsCmdDelete:
            /* Entity is deleted, don't batch commands after the delete */
            next_for_entity = 0;
            break;
        case EcsCmdClone:
        case EcsCmdBulkNew:
        case EcsCmdPath:
        case EcsCmdOnDeleteAction:
        case EcsCmdEnable:
        case EcsCmdDisable:
        case EcsCmdEvent:
        case EcsCmdSkip:
        case EcsCmdModifiedNoHook:
        case EcsCmdModified:
        case EcsCmdSetDontFragment:
        case EcsCmdEnsureDontFragment:
            break;
        }
    } while ((cur = next_for_entity));

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Save added ids and clear from diff so that they won't be emitted as
     * part of the commit below. OnAdd events will be emitted separately after
     * the commit, so that observers with mixed OnAdd/OnSet events won't get
     * called with uninitialized values for an OnSet field. */
    ecs_type_t added = { diff->added.array, diff->added.count };
    diff->added.array = NULL;
    diff->added.count = 0;

    /* Move entity to destination table in single operation */
    flecs_table_diff_build_noalloc(diff, &table_diff);
    ecs_stage_t *stage = world->stages[0];
    const ecs_stage_ensure_t *prev_ensure_add = stage->ensure_add;
    ecs_stage_ensure_t ensure_add_type = { entity, {
        ecs_vec_first_t(set_ids, ecs_id_t), ecs_vec_count(set_ids) }};
    if (ensure_add_type.ids.count) {
        stage->ensure_add = &ensure_add_type;
    }

    flecs_commands_begin(world, stage);
    flecs_commit(world, entity, r, table, &table_diff, 0, 0);
    flecs_commands_end(world, stage);
    stage->ensure_add = prev_ensure_add;

    /* If destination table has new sparse components, make sure they're created
     * for the entity. */
    if ((table_diff.added_flags & EcsTableHasSparse) && added.count) {
        if (flecs_sparse_on_add(
            world, table, ECS_RECORD_TO_ROW(r->row), 1, &added, 0))
        {
            table_diff.added_flags |= EcsTableHasOnAdd;
        }
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
                        flecs_commands_begin(world, world->stages[0]);
                        flecs_invoke_replace_hook(world, prev_table, entity,
                            cmd->id, dst.ptr, ptr, ti);
                        flecs_commands_end(world, world->stages[0]);
                        if (!r->table) {
                            /* Entity was deleted */
                            goto done;
                        }

                        /* Refetch pointer as the hook could have grown the
                         * table or moved the entity to a different table. */
                        dst = flecs_get_mut(
                            world, entity, cmd->id, r, cmd->is._1.size);
                        if (!dst.ptr) {
                            cmd->kind = EcsCmdSkip;
                            break;
                        }
                    }

                    bool move_hook = ti->hooks.move != NULL;
                    ECS_TABLE_LOCK(world, r->table);
                    flecs_type_info_move(dst.ptr, ptr, 1, ti);
                    if (move_hook) {
                        flecs_type_info_dtor(ptr, 1, ti);
                    }
                    ECS_TABLE_UNLOCK(world, r->table);

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
            case EcsCmdDelete:
                /* Entity is deleted, don't process commands after delete */
                next_for_entity = 0;
                break;
            case EcsCmdRemove:
            case EcsCmdClone:
            case EcsCmdBulkNew:
            case EcsCmdAdd:
            case EcsCmdEmplace:
            case EcsCmdModified:
            case EcsCmdModifiedNoHook:
            case EcsCmdAddModified:
            case EcsCmdSetDontFragment:
            case EcsCmdEnsureDontFragment:
            case EcsCmdPath:
            case EcsCmdClear:
            case EcsCmdOnDeleteAction:
            case EcsCmdEnable:
            case EcsCmdDisable:
            case EcsCmdEvent:
                case EcsCmdSkip:
                break;
            }
        } while ((cur = next_for_entity) && cur != split);
    }

    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (added.count) {
        ecs_table_diff_t add_diff = ECS_TABLE_DIFF_INIT;
        add_diff.added = added;
        add_diff.added_flags = table_diff.added_flags;

        flecs_commands_begin(world, world->stages[0]);
        flecs_actions_move_add(world, r->table, start_table,
            ECS_RECORD_TO_ROW(r->row), 1, &add_diff, 0, false, 0);
        flecs_commands_end(world, world->stages[0]);
    }

done:
    diff->added.array = added.array;
    diff->added.count = added.count;
    flecs_table_diff_builder_clear(diff);

    if (split) {
        cmds[split].next_for_entity *= -1;
    }
}

/* Leave safe section. Run all deferred commands. */
bool flecs_commands_flush(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    flecs_poly_assert(world, ecs_world_t);
    flecs_poly_assert(stage, ecs_stage_t);
    ecs_commands_t *commands = stage->cmd;
    if (!ecs_vec_count(&commands->queue)) {
        return false;
    }
    flecs_check_exclusive_world_access_write(world);
    ecs_stage_t *dst_stage = world->stages[0];
    ecs_commands_t *root = commands;
    ecs_vec_t *queue;
    ecs_cmd_t *cmds;
    int32_t i, count;
    ecs_table_diff_builder_t diff = {0};
    ecs_vec_t set_ids = {0};
    bool diff_builder_used = false;

enter:
    queue = &commands->queue;
    ecs_os_perf_trace_push("flecs.commands.merge");
    flecs_commands_begin(world, stage);
    if (dst_stage != stage) {
        flecs_commands_begin(world, dst_stage);
    }

    if (world->on_commands_active) {
        world->on_commands_active(stage, queue,
            world->on_commands_ctx_active);
    }

    cmds = ecs_vec_first(queue);
    count = ecs_vec_count(queue);
    i = 0;

resume:
    for (; i < count; i ++) {
        ecs_cmd_t *cmd = &cmds[i];
        if (cmd->kind == EcsCmdSkip && !cmd->is._1.value) {
            if (cmd->entry) {
                cmd->entry->first = -1;
            }
            world->info.cmd.discard_count ++;
            continue;
        }
        ecs_entity_t e = cmd->entity;
        bool is_alive = flecs_entities_is_alive(world, e);

        if (cmd->next_for_entity < 0) {
            diff.added_flags = 0;
            diff.removed_flags = 0;

            if (is_alive) {
                if (!diff_builder_used) {
                    flecs_table_diff_builder_init(world, &diff);
                    ecs_vec_init_t(&world->allocator, &set_ids, ecs_id_t, 0);
                    diff_builder_used = true;
                }

                flecs_cmd_batch_for_entity(
                    world, &diff, &set_ids, e, cmds, i);

                is_alive = flecs_entities_is_alive(world, e);
            } else {
                world->info.cmd.discard_count ++;
            }
        }

        if (cmd->entry) {
            cmd->entry->first = -1;
        }

        ecs_cmd_kind_t kind = cmd->kind;
        if ((kind != EcsCmdPath) && ((kind == EcsCmdSkip) || (e && !is_alive))) {
            world->info.cmd.discard_count ++;
            flecs_discard_cmd(world, cmd);
            goto next;
        }

        ecs_id_t id = cmd->id;

        switch(kind) {
        case EcsCmdAdd:
            ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
            if (id < FLECS_HI_COMPONENT_ID) {
                ecs_record_t *r = flecs_entities_get(world, e);
                if (r->table->component_map[id]) {
                    world->info.cmd.add_count ++;
                    break;
                }
            }

            if (flecs_remove_invalid(world, id, &id)) {
                if (id) {
                    world->info.cmd.add_count ++;
                    flecs_add_id_now(world, e, id);
                } else {
                    world->info.cmd.discard_count ++;
                }
            } else {
                world->info.cmd.discard_count ++;
                ecs_delete(world, e);
            }
            break;
        case EcsCmdRemove:
            flecs_remove_id_now(world, e, id);
            world->info.cmd.remove_count ++;
            break;
        case EcsCmdClone:
            if (flecs_entities_is_alive(world, id)) {
                ecs_clone(world, e, id, cmd->is._1.clone_value);
                world->info.cmd.other_count ++;
            } else {
                world->info.cmd.discard_count ++;
            }
            break;
        case EcsCmdSet:
        case EcsCmdSetDontFragment:
            flecs_set_id_move(world, e,
                cmd->id, flecs_itosize(cmd->is._1.size),
                cmd->is._1.value, kind);
            world->info.cmd.set_count ++;
            break;
        case EcsCmdEmplace:
            {
                bool is_new;
                ecs_emplace_id(world, e, id,
                    flecs_itosize(cmd->is._1.size), &is_new);
                if (!is_new) {
                    kind = EcsCmdEnsure;
                }
            }
            flecs_set_id_move(world, e,
                cmd->id, flecs_itosize(cmd->is._1.size),
                cmd->is._1.value, kind);
            world->info.cmd.ensure_count ++;
            break;
        case EcsCmdEnsure:
        case EcsCmdEnsureDontFragment:
            flecs_set_id_move(world, e,
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
            flecs_commands_begin(world, flecs_stage_from_readonly_world(world));
            flecs_on_delete(world, id, e, false,
                cmd->is._1.force_delete);
            flecs_commands_end(world, flecs_stage_from_readonly_world(world));
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
            goto next;
        case EcsCmdPath: {
            bool keep_alive = true;
            ecs_make_alive(world, e);
            if (cmd->id) {
                if (ecs_is_alive(world, cmd->id)) {
                    if (ecs_get_parent(world, e) != cmd->id) {
                        ecs_add_pair(world, e, EcsChildOf, cmd->id);
                    }
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
next:
        if (dst_stage != stage && ecs_vec_count(&dst_stage->cmd->queue)) {
            flecs_commands_flush(world, dst_stage);
        }
        if (ecs_vec_count(&stage->cmd->queue)) {
            commands->cursor = i + 1;
            commands = stage->cmd;
            goto enter;
        }
    }

    if (dst_stage != stage) {
        flecs_commands_end(world, dst_stage);
    }
    stage->cmd = commands;

    flecs_stack_reset(&commands->stack);
    ecs_vec_clear(queue);
    commands->has_remove = false;

    if (world->on_commands_active) {
        world->on_commands_active(stage, NULL,
            world->on_commands_ctx_active);
    }

    ecs_os_perf_trace_pop("flecs.commands.merge");

    if (commands != root) {
        commands = commands->prev;
        queue = &commands->queue;
        cmds = ecs_vec_first(queue);
        count = ecs_vec_count(queue);
        i = commands->cursor;
        goto resume;
    }

    if (diff_builder_used) {
        flecs_table_diff_builder_fini(world, &diff);
        ecs_vec_fini_t(&world->allocator, &set_ids, ecs_id_t);
    }

    return true;
}

bool flecs_commands_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_commands_t *cmd = stage->cmd;
    ecs_assert(cmd->prev != NULL, ECS_INTERNAL_ERROR, NULL);
    if (ecs_vec_count(&cmd->queue)) {
        flecs_commands_flush(world, stage);
    }
    stage->cmd = cmd->prev;
    return true;
}

/* Discard commands from queue without executing them. */
bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

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
        stage->cmd->entry.first = -1;
        stage->cmd->second_entry.first = -1;
        stage->cmd->last_entity = 0;
        stage->cmd->has_remove = false;
    }

    return true;

error:
    return false;
}

void flecs_commands_init(
    ecs_stage_t *stage,
    ecs_commands_t *cmd)
{
    cmd->entity = 0;
    cmd->entry.first = -1;
    cmd->second_entity = 0;
    cmd->second_entry.first = -1;
    cmd->last_entity = 0;
    cmd->has_remove = false;
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

void flecs_commands_reset_entries(
    ecs_stage_t *stage,
    ecs_commands_t *cmd)
{
    ecs_assert(ecs_vec_count(&cmd->queue) == 0, ECS_INTERNAL_ERROR, NULL);

    flecs_sparse_fini(&cmd->entries);
    flecs_sparse_init_t(&cmd->entries, &stage->allocator,
        &stage->allocators.cmd_entry_chunk, ecs_cmd_entry_t);
    cmd->entry.first = -1;
    cmd->second_entry.first = -1;
    cmd->last_entity = 0;
}
