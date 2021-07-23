/**
 * @file storage_plugin.h
 * @brief Storage plugin interface
 *
 * Allows for integration of pluggable storage types.
 */

#ifndef FLECS_STORAGE_H
#define FLECS_STORAGE_H

#include "flecs/private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_storage_t ecs_storage_t; 

typedef struct ecs_storage_iter_t {
    void *data;
    int32_t offset;
    int32_t count;
} ecs_storage_iter_t;

typedef void (*ecs_storage_fini_action_t)(
    ecs_storage_t *storage);

typedef void* (*ecs_storage_push_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    ecs_entity_t id);

typedef void (*ecs_storage_push_n_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t count,
    const ecs_entity_t *ids,
    const ecs_storage_t *src_storage,
    int32_t src_index);

typedef void* (*ecs_storage_emplace_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage);

typedef void (*ecs_storage_erase_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);

typedef void (*ecs_storage_pop_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage);

typedef void (*ecs_storage_merge_action_t)(
    ecs_world_t *world,
    ecs_storage_t *dst,
    ecs_storage_t *src);

typedef void (*ecs_storage_swap_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index_a,
    int32_t index_b);

typedef void (*ecs_storage_clear_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage);        

typedef void (*ecs_storage_resize_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    ecs_size_t size);

typedef void* (*ecs_storage_get_action_t)(
    const ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);

typedef bool (*ecs_storage_has_action_t)(
    const ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);

typedef int32_t (*ecs_storage_count_action_t)(
    const ecs_storage_t *storage);   

typedef void (*ecs_storage_ctor_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);   

typedef void (*ecs_storage_dtor_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index);

typedef void (*ecs_storage_copy_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    const void *ptr);

typedef void (*ecs_storage_move_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    void *ptr);

typedef void (*ecs_storage_copy_ctor_action_t)(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id,
    const void *ptr);

typedef ecs_storage_iter_t (*ecs_storage_iter_action_t)(
    const ecs_storage_t *storage);

typedef bool (*ecs_storage_next_action_t)(
    const ecs_storage_t *storage,
    ecs_storage_iter_t *iter);

typedef struct ecs_storage_plugin_t {
    ecs_storage_fini_action_t fini;

    /* Mutation */
    ecs_storage_push_action_t push;
    ecs_storage_push_n_action_t push_n;
    ecs_storage_emplace_action_t emplace;
    ecs_storage_erase_action_t erase;
    ecs_storage_pop_action_t pop;
    ecs_storage_swap_action_t swap;
    ecs_storage_merge_action_t merge;
    ecs_storage_clear_action_t clear;
    ecs_storage_clear_action_t destruct;

    /* Allocate space in advance */
    ecs_storage_resize_action_t reserve;
    ecs_storage_resize_action_t grow;

    /* Const operations */
    ecs_storage_get_action_t get;
    ecs_storage_has_action_t has;
    ecs_storage_count_action_t count;

    /* Lifecycle management */
    ecs_storage_ctor_action_t ctor;
    ecs_storage_dtor_action_t dtor;
    ecs_storage_copy_action_t copy;
    ecs_storage_move_action_t ctor_move_dtor;
    ecs_storage_copy_ctor_action_t ctor_copy;

    /* Iteration */
    ecs_storage_iter_action_t iter;
    ecs_storage_next_action_t next;

    /* Plugin traits */
    bool is_sparse;
} ecs_storage_plugin_t;

struct ecs_storage_t {
    const ecs_storage_plugin_t *plugin;
    void *impl;
    ecs_size_t size;
    ecs_size_t alignment;
    const ecs_type_info_t *info;
};

void ecs_storage_init(
    ecs_storage_t *storage);

void ecs_storage_fini(
    ecs_storage_t *storage);

void* ecs_storage_push(
    ecs_world_t *world,
    ecs_storage_t *storage,
    ecs_entity_t id);

void ecs_storage_push_n(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t count,
    const ecs_entity_t *ids,
    const ecs_storage_t *src_storage,
    int32_t src_index);

void* ecs_storage_emplace(
    ecs_world_t *world,
    ecs_storage_t *storage);

void ecs_storage_erase(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);

void ecs_storage_pop(
    ecs_world_t *world,
    ecs_storage_t *storage);  

void ecs_storage_swap(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index_a,
    int32_t index_b);

void ecs_storage_merge(
    ecs_world_t *world,
    ecs_storage_t *dst,
    ecs_storage_t *src);

void ecs_storage_clear(
    ecs_world_t *world,
    ecs_storage_t *storage);  

void ecs_storage_destruct(
    ecs_world_t *world,
    ecs_storage_t *storage); 

void ecs_storage_reserve(
    ecs_world_t *world,
    ecs_storage_t *storage,
    ecs_size_t size);

void ecs_storage_grow(
    ecs_world_t *world,
    ecs_storage_t *storage,
    ecs_size_t count);    

void* ecs_storage_get(
    const ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);

bool ecs_storage_has(
    const ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);

int32_t ecs_storage_count(
    const ecs_storage_t *storage);   

void ecs_storage_ctor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id);

void ecs_storage_dtor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index);

void ecs_storage_copy(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    const void *ptr);

void ecs_storage_ctor_move_dtor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    void *ptr);

void ecs_storage_ctor_copy(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id,
    const void *ptr);

ecs_storage_iter_t ecs_storage_iter(
    const ecs_storage_t *storage);

bool ecs_storage_next(
    const ecs_storage_t *storage,
    ecs_storage_iter_t *iter);

#ifdef __cplusplus
}
#endif

#endif // FLECS_STORAGE_H
