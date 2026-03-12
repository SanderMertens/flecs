/**
 * @file commands.h
 * @brief Command queue implementation.
 */

#ifndef FLECS_COMMANDS_H
#define FLECS_COMMANDS_H

/* Deferred command types */
typedef enum ecs_cmd_kind_t {
    EcsCmdClone,
    EcsCmdBulkNew,
    EcsCmdAdd,
    EcsCmdRemove,   
    EcsCmdSet,
    EcsCmdSetDontFragment,
    EcsCmdEmplace,
    EcsCmdEnsure,
    EcsCmdModified,
    EcsCmdModifiedNoHook,
    EcsCmdAddModified,
    EcsCmdPath,
    EcsCmdDelete,
    EcsCmdClear,
    EcsCmdOnDeleteAction,
    EcsCmdEnable,
    EcsCmdDisable,
    EcsCmdEvent,
    EcsCmdSkip
} ecs_cmd_kind_t;

/* Per-entity metadata linking commands in a batch chain */
typedef struct ecs_cmd_entry_t {
    int32_t first;                   /* If -1, entry invalidated (e.g. by delete) */
    int32_t last;
} ecs_cmd_entry_t;

typedef struct ecs_cmd_1_t {
    void *value;                     /* Component value (used by set / ensure) */
    ecs_size_t size;                 /* Size of value */
    bool clone_value;                /* Clone entity with value (used for clone) */ 
} ecs_cmd_1_t;

typedef struct ecs_cmd_n_t {
    ecs_entity_t *entities;  
    int32_t count;
} ecs_cmd_n_t;

typedef struct ecs_cmd_t {
    ecs_cmd_kind_t kind;             /* Command kind */
    int32_t next_for_entity;         /* Next operation for entity */    
    ecs_id_t id;                     /* (Component) id */
    ecs_cmd_entry_t *entry;
    ecs_entity_t entity;             /* Entity id */

    union {
        ecs_cmd_1_t _1;              /* Data for single entity operation */
        ecs_cmd_n_t _n;              /* Data for multi entity operation */
    } is;

    ecs_entity_t system;             /* System that enqueued the command */
} ecs_cmd_t;

/* Callback invoked when commands are flushed, used for diagnostics/replay */
typedef void (*ecs_on_commands_action_t)(
    const ecs_stage_t *stage,
    const ecs_vec_t *commands,
    void *ctx);

void flecs_commands_init(
    ecs_stage_t *stage,
    ecs_commands_t *cmd);

void flecs_commands_fini(
    ecs_stage_t *stage,
    ecs_commands_t *cmd);

bool flecs_defer_cmd(
    ecs_stage_t *stage);

bool flecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_modified(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component);

bool flecs_defer_clone(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value);

bool flecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    ecs_id_t id,
    const ecs_entity_t **ids_out);

bool flecs_defer_path(
    ecs_stage_t *stage,
    ecs_entity_t parent,
    ecs_entity_t entity,
    const char *name);

bool flecs_defer_delete(
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_clear(
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_on_delete_action(
    ecs_stage_t *stage,
    ecs_id_t id,
    ecs_entity_t action);

bool flecs_defer_enable(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool enable);    

bool flecs_defer_add(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id);

bool flecs_defer_remove(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id);

void* flecs_defer_emplace(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    bool *is_new);

void* flecs_defer_ensure(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size);

void* flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    void *value);

void* flecs_defer_cpp_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value);

void* flecs_defer_cpp_assign(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value);

void flecs_enqueue(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_event_desc_t *desc);
 
#endif
