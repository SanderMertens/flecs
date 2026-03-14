/**
 * @file storage/entity_index.h
 * @brief Entity index data structure.
 *
 * The entity index stores the table and row for an entity id.
 */

#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H

#define FLECS_ENTITY_PAGE_SIZE (1 << FLECS_ENTITY_PAGE_BITS)
#define FLECS_ENTITY_PAGE_MASK (FLECS_ENTITY_PAGE_SIZE - 1)

typedef struct ecs_entity_index_page_t {
    ecs_record_t records[FLECS_ENTITY_PAGE_SIZE];
} ecs_entity_index_page_t;

/* Sparse set mapping entity ids to records. Dense array stores entity ids;
 * indices [1..alive_count) are alive, [alive_count..count) are recyclable. */
typedef struct ecs_entity_index_t {
    ecs_vec_t dense;             /* vec<uint64_t> entity ids */
    ecs_vec_t pages;             /* vec<ecs_entity_index_page_t*> */
    int32_t alive_count;         /* Partition index: alive entities before, dead after */
    uint64_t max_id;
    ecs_allocator_t *allocator;
} ecs_entity_index_t;

void flecs_entity_index_init(
    ecs_allocator_t *allocator,
    ecs_entity_index_t *index);

void flecs_entity_index_fini(
    ecs_entity_index_t *index);

ecs_record_t* flecs_entity_index_get(
    const ecs_entity_index_t *index,
    uint64_t entity);

ecs_record_t* flecs_entity_index_get_any(
    const ecs_entity_index_t *index,
    uint64_t entity);

ecs_record_t* flecs_entity_index_try_get(
    const ecs_entity_index_t *index,
    uint64_t entity);

ecs_record_t* flecs_entity_index_try_get_any(
    const ecs_entity_index_t *index,
    uint64_t entity);

ecs_record_t* flecs_entity_index_ensure(
    ecs_entity_index_t *index,
    uint64_t entity);

void flecs_entity_index_remove(
    ecs_entity_index_t *index,
    uint64_t entity);

void flecs_entity_index_make_alive(
    ecs_entity_index_t *index,
    uint64_t entity);

uint64_t flecs_entity_index_get_alive(
    const ecs_entity_index_t *index,
    uint64_t entity);

bool flecs_entity_index_is_alive(
    const ecs_entity_index_t *index,
    uint64_t entity);

bool flecs_entity_index_is_valid(
    const ecs_entity_index_t *index,
    uint64_t entity);

bool flecs_entity_index_exists(
    const ecs_entity_index_t *index,
    uint64_t entity);

uint64_t flecs_entity_index_new_id(
    ecs_entity_index_t *index);

uint64_t* flecs_entity_index_new_ids(
    ecs_entity_index_t *index,
    int32_t count);

void flecs_entity_index_set_size(
    ecs_entity_index_t *index,
    int32_t size);

int32_t flecs_entity_index_count(
    const ecs_entity_index_t *index);

int32_t flecs_entity_index_size(
    const ecs_entity_index_t *index);

int32_t flecs_entity_index_not_alive_count(
    const ecs_entity_index_t *index);

void flecs_entity_index_clear(
    ecs_entity_index_t *index);

void flecs_entity_index_shrink(
    ecs_entity_index_t *index);

const uint64_t* flecs_entity_index_ids(
    const ecs_entity_index_t *index);

/* Convenience macros that operate on the world's entity index */
#define ecs_eis(world) (&((world)->store.entity_index))
#define flecs_entities_init(world) flecs_entity_index_init(&world->allocator, ecs_eis(world))
#define flecs_entities_fini(world) flecs_entity_index_fini(ecs_eis(world))
#define flecs_entities_get(world, entity) flecs_entity_index_get(ecs_eis(world), entity)
#define flecs_entities_try(world, entity) flecs_entity_index_try_get(ecs_eis(world), entity)
#define flecs_entities_get_any(world, entity) flecs_entity_index_get_any(ecs_eis(world), entity)
#define flecs_entities_ensure(world, entity) flecs_entity_index_ensure(ecs_eis(world), entity)
#define flecs_entities_remove(world, entity) flecs_entity_index_remove(ecs_eis(world), entity)
#define flecs_entities_make_alive(world, entity) flecs_entity_index_make_alive(ecs_eis(world), entity)
#define flecs_entities_get_alive(world, entity) flecs_entity_index_get_alive(ecs_eis(world), entity)
#define flecs_entities_is_alive(world, entity) flecs_entity_index_is_alive(ecs_eis(world), entity)
#define flecs_entities_is_valid(world, entity) flecs_entity_index_is_valid(ecs_eis(world), entity)
#define flecs_entities_exists(world, entity) flecs_entity_index_exists(ecs_eis(world), entity)
#define flecs_entities_new_id(world) flecs_entity_index_new_id(ecs_eis(world))
#define flecs_entities_new_ids(world, count) flecs_entity_index_new_ids(ecs_eis(world), count)
#define flecs_entities_max_id(world) (ecs_eis(world)->max_id)
#define flecs_entities_set_size(world, size) flecs_entity_index_set_size(ecs_eis(world), size)
#define flecs_entities_count(world) flecs_entity_index_count(ecs_eis(world))
#define flecs_entities_size(world) flecs_entity_index_size(ecs_eis(world))
#define flecs_entities_not_alive_count(world) flecs_entity_index_not_alive_count(ecs_eis(world))
#define flecs_entities_clear(world) flecs_entity_index_clear(ecs_eis(world))
#define flecs_entities_ids(world) flecs_entity_index_ids(ecs_eis(world))

#endif
