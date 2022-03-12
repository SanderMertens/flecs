/**
 * @file entity_index.h
 * @brief Entity index data structure.
 *
 * The entity index stores the table, row for an entity id. It is implemented as
 * a sparse set. This file contains convenience macro's for working with the
 * entity index.
 */

#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H

#ifdef __cplusplus
extern "C" {
#endif

#define ecs_eis(world) (&((world)->store.entity_index))
#define ecs_eis_get(world, entity) flecs_sparse_get(ecs_eis(world), ecs_record_t, entity)
#define ecs_eis_get_any(world, entity) flecs_sparse_get_any(ecs_eis(world), ecs_record_t, entity)
#define ecs_eis_set(world, entity, ...) (flecs_sparse_set(ecs_eis(world), ecs_record_t, entity, (__VA_ARGS__)))
#define ecs_eis_ensure(world, entity) flecs_sparse_ensure(ecs_eis(world), ecs_record_t, entity)
#define ecs_eis_delete(world, entity) flecs_sparse_remove(ecs_eis(world), entity)
#define ecs_eis_set_generation(world, entity) flecs_sparse_set_generation(ecs_eis(world), entity)
#define ecs_eis_is_alive(world, entity) flecs_sparse_is_alive(ecs_eis(world), entity)
#define ecs_eis_get_current(world, entity) flecs_sparse_get_alive(ecs_eis(world), entity)
#define ecs_eis_exists(world, entity) flecs_sparse_exists(ecs_eis(world), entity)
#define ecs_eis_recycle(world) flecs_sparse_new_id(ecs_eis(world))
#define ecs_eis_clear_entity(world, entity, is_watched) ecs_eis_set(ecs_eis(world), entity, &(ecs_record_t){NULL, is_watched})
#define ecs_eis_set_size(world, size) flecs_sparse_set_size(ecs_eis(world), size)
#define ecs_eis_count(world) flecs_sparse_count(ecs_eis(world))
#define ecs_eis_clear(world) flecs_sparse_clear(ecs_eis(world))
#define ecs_eis_copy(world) flecs_sparse_copy(ecs_eis(world))
#define ecs_eis_free(world) flecs_sparse_free(ecs_eis(world))
#define ecs_eis_memory(world, allocd, used) flecs_sparse_memory(ecs_eis(world), allocd, used)

#ifdef __cplusplus
}
#endif

#endif
