/**
 * @file datastructures/entity_index.h
 * @brief Entity index data structure.
 *
 * The entity index stores the table, row for an entity id.
 */
 
#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H

#define FLECS_ENTITY_PAGE_SIZE (1 << FLECS_ENTITY_PAGE_BITS)
#define FLECS_ENTITY_PAGE_MASK (FLECS_ENTITY_PAGE_SIZE - 1)

typedef struct ecs_entity_index_page_t {
    ecs_record_t records[FLECS_ENTITY_PAGE_SIZE];
} ecs_entity_index_page_t;

typedef struct ecs_entity_index_t {
    ecs_vec_t dense;
    ecs_vec_t pages;
    int32_t alive_count;
    uint64_t max_id;
    ecs_block_allocator_t page_allocator;
    ecs_allocator_t *allocator;
} ecs_entity_index_t;

/** Initialize entity index. */
void flecs_entity_index_init(
    ecs_allocator_t *allocator,
    ecs_entity_index_t *index);

/** Deinitialize entity index. */
void flecs_entity_index_fini(
    ecs_entity_index_t *index);

/* Get entity (must exist/must be alive) */
ecs_record_t* flecs_entity_index_get(
    const ecs_entity_index_t *index,
    uint64_t entity);

/* Get entity (must exist/may not be alive) */
ecs_record_t* flecs_entity_index_get_any(
    const ecs_entity_index_t *index,
    uint64_t entity);

/* Get entity (may not exist/must be alive) */
ecs_record_t* flecs_entity_index_try_get(
    const ecs_entity_index_t *index,
    uint64_t entity);

/* Get entity (may not exist/may not be alive) */
ecs_record_t* flecs_entity_index_try_get_any(
    const ecs_entity_index_t *index,
    uint64_t entity);

/** Ensure entity exists. */
ecs_record_t* flecs_entity_index_ensure(
    ecs_entity_index_t *index,
    uint64_t entity);

/* Remove entity */
void flecs_entity_index_remove(
    ecs_entity_index_t *index,
    uint64_t entity);

/* Set generation of entity */
void flecs_entity_index_make_alive(
    ecs_entity_index_t *index,
    uint64_t entity);

/* Get current generation of entity */
uint64_t flecs_entity_index_get_alive(
    const ecs_entity_index_t *index,
    uint64_t entity);

/* Return whether entity is alive */
bool flecs_entity_index_is_alive(
    const ecs_entity_index_t *index,
    uint64_t entity);

/* Return whether entity is valid */
bool flecs_entity_index_is_valid(
    const ecs_entity_index_t *index,
    uint64_t entity);

/* Return whether entity exists */
bool flecs_entity_index_exists(
    const ecs_entity_index_t *index,
    uint64_t entity);

/* Create or recycle entity id */
uint64_t flecs_entity_index_new_id(
    ecs_entity_index_t *index);

/* Bulk create or recycle new entity ids */
uint64_t* flecs_entity_index_new_ids(
    ecs_entity_index_t *index,
    int32_t count);

/* Set size of index */
void flecs_entity_index_set_size(
    ecs_entity_index_t *index,
    int32_t size);

/* Return number of entities in index */
int32_t flecs_entity_index_count(
    const ecs_entity_index_t *index);

/* Return number of allocated entities in index */
int32_t flecs_entity_index_size(
    const ecs_entity_index_t *index);

/* Return number of not alive entities in index */
int32_t flecs_entity_index_not_alive_count(
    const ecs_entity_index_t *index);

/* Clear entity index */
void flecs_entity_index_clear(
    ecs_entity_index_t *index);

/* Return number of alive entities in index */
const uint64_t* flecs_entity_index_ids(
    const ecs_entity_index_t *index);

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
