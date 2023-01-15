/**
 * @file datastructures/entity_index.h
 * @brief Entity index data structure.
 *
 * The entity index stores the table, row for an entity id. It is implemented as
 * a sparse set. This file contains convenience macros for working with the
 * entity index.
 */

#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H

#define ecs_eis(world) (&((world)->store.entity_index))
#define flecs_entities_get(world, entity) flecs_sparse_get_t(ecs_eis(world), ecs_record_t, entity)
#define flecs_entities_try(world, entity) flecs_sparse_try_t(ecs_eis(world), ecs_record_t, entity)
#define flecs_entities_get_any(world, entity) flecs_sparse_get_any_t(ecs_eis(world), ecs_record_t, entity)
#define flecs_entities_ensure(world, entity) flecs_sparse_ensure_t(ecs_eis(world), ecs_record_t, entity)
#define flecs_entities_remove(world, entity) flecs_sparse_remove_t(ecs_eis(world), ecs_record_t, entity)
#define flecs_entities_set_generation(world, entity) flecs_sparse_set_generation(ecs_eis(world), entity)
#define flecs_entities_is_alive(world, entity) flecs_sparse_is_alive(ecs_eis(world), entity)
#define flecs_entities_is_valid(world, entity) flecs_sparse_is_valid(ecs_eis(world), entity)
#define flecs_entities_get_current(world, entity) flecs_sparse_get_current(ecs_eis(world), entity)
#define flecs_entities_exists(world, entity) flecs_sparse_exists(ecs_eis(world), entity)
#define flecs_entities_new_id(world) flecs_sparse_new_id(ecs_eis(world))
#define flecs_entities_new_ids(world, count) flecs_sparse_new_ids(ecs_eis(world), count)
#define flecs_entities_set_size(world, size) flecs_sparse_set_size(ecs_eis(world), size)
#define flecs_entities_count(world) flecs_sparse_count(ecs_eis(world))
#define flecs_entities_fini(world) flecs_sparse_fini(ecs_eis(world))

#endif
