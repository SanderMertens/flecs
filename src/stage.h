/**
 * @file stage.h
 * @brief Stage functions.
 */

#ifndef FLECS_STAGE_H
#define FLECS_STAGE_H

/* Stage level allocators are for operations that can be multithreaded */
typedef struct ecs_stage_allocators_t {
    ecs_stack_t iter_stack;
    ecs_block_allocator_t cmd_entry_chunk;
    ecs_block_allocator_t query_impl;
    ecs_block_allocator_t query_cache;
} ecs_stage_allocators_t;

/** A stage is a context that allows for safely using the API from multiple 
 * threads. Stage pointers can be passed to the world argument of API 
 * operations, which causes the operation to be ran on the stage instead of the
 * world. The features provided by a stage are:
 * 
 *  - A command queue for deferred ECS operations and events
 *  - Thread specific allocators
 *  - Thread specific world state (like current scope, with, current system)
 *  - Thread specific buffers for preventing allocations
 */
struct ecs_stage_t {
    ecs_header_t hdr;

    /* Unique id that identifies the stage */
    int32_t id;

    /* Zero if not deferred, positive if deferred, negative if suspended */
    int32_t defer;

    /* Command queue */
    ecs_commands_t *cmd;
    ecs_commands_t cmd_stack[2];     /* Two so we can flush one & populate the other */
    bool cmd_flushing;               /* Ensures only one defer_end call flushes */

    /* Thread context */
    ecs_world_t *thread_ctx;         /* Points to stage when a thread stage */
    ecs_world_t *world;              /* Reference to world */
    ecs_os_thread_t thread;          /* Thread handle (0 if no threading is used) */

    /* One-shot actions to be executed after the merge */
    ecs_vec_t post_frame_actions;

    /* Namespacing */
    ecs_entity_t scope;              /* Entity of current scope */
    ecs_entity_t with;               /* Id to add by default to new entities */
    ecs_entity_t base;               /* Currently instantiated top-level base */
    const ecs_entity_t *lookup_path; /* Search path used by lookup operations */

    /* Running system */
    ecs_entity_t system;

    /* Thread specific allocators */
    ecs_stage_allocators_t allocators;
    ecs_allocator_t allocator;

    /* Caches for query creation */
    ecs_vec_t variables;
    ecs_vec_t operations;

#ifdef FLECS_SCRIPT
    /* Thread specific runtime for script execution */
    ecs_script_runtime_t *runtime;
#endif
};

/* Post-frame merge actions. */
void flecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage);  

/* Set system id for debugging which system inserted which commands. */
ecs_entity_t flecs_stage_set_system(
    ecs_stage_t *stage,
    ecs_entity_t system);

/* Get allocator from stage/world. */
ecs_allocator_t* flecs_stage_get_allocator(
    ecs_world_t *world);

/* Get stack allocator from stage/world. */
ecs_stack_t* flecs_stage_get_stack_allocator(
    ecs_world_t *world);

/* Shrink memory for stage data structures. */
void ecs_stage_shrink(
    ecs_stage_t *stage);

#endif
