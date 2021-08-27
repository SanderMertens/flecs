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

#define ecs_eis_get(world, entity) flecs_sparse_get((world->store).entity_index, ecs_record_t, entity)
#define ecs_eis_get_any(world, entity) flecs_sparse_get_any((world->store).entity_index, ecs_record_t, entity)
#define ecs_eis_set(world, entity, ...) (flecs_sparse_set((world->store).entity_index, ecs_record_t, entity, (__VA_ARGS__)))
#define ecs_eis_ensure(world, entity) flecs_sparse_ensure((world->store).entity_index, ecs_record_t, entity)
#define ecs_eis_delete(world, entity) flecs_sparse_remove((world->store).entity_index, entity)
#define ecs_eis_set_generation(world, entity) flecs_sparse_set_generation((world->store).entity_index, entity)
#define ecs_eis_is_alive(world, entity) flecs_sparse_is_alive((world->store).entity_index, entity)
#define ecs_eis_get_current(world, entity) flecs_sparse_get_alive((world->store).entity_index, entity)
#define ecs_eis_exists(world, entity) flecs_sparse_exists((world->store).entity_index, entity)
#define ecs_eis_recycle(world) flecs_sparse_new_id((world->store).entity_index)
#define ecs_eis_clear_entity(world, entity, is_watched) ecs_eis_set((world->store).entity_index, entity, &(ecs_record_t){NULL, is_watched})
#define ecs_eis_set_size(world, size) flecs_sparse_set_size((world->store).entity_index, size)
#define ecs_eis_count(world) flecs_sparse_count((world->store).entity_index)
#define ecs_eis_clear(world) flecs_sparse_clear((world->store).entity_index)
#define ecs_eis_copy(world) flecs_sparse_copy((world->store).entity_index)
#define ecs_eis_free(world) flecs_sparse_free((world->store).entity_index)
#define ecs_eis_memory(world, allocd, used) flecs_sparse_memory((world->store).entity_index, allocd, used)

#ifdef __cplusplus
}
#endif

#endif
