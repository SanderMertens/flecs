/**
 * @file api_internals.h
 * @brief Access to internal data structures.
 * 
 * Operations may change without warning.
 */

#ifndef FLECS_API_INTERNALS_H
#define FLECS_API_INTERNALS_H

#include "api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Record for entity index. */
struct ecs_record_t {
    ecs_table_t *table;                        /**< Identifies a type (and table) in world */
    uint32_t row;                              /**< Table row of the entity */
    int32_t dense;                             /**< Index in dense array of entity index */    
};

/** Header for table cache elements. */
typedef struct ecs_table_cache_hdr_t {
    struct ecs_component_record_t *cr;         /**< Component record for component. */
    ecs_table_t *table;                        /**< Table associated with element. */
    struct ecs_table_cache_hdr_t *prev, *next; /**< Next/previous elements for id in table cache. */
} ecs_table_cache_hdr_t;

/** Record that stores location of a component in a table. 
 * Table records are registered with component records, which allows for quickly
 * finding all tables for a specific component. */
struct ecs_table_record_t {
    ecs_table_cache_hdr_t hdr;                 /**< Table cache header */
    int16_t index;                             /**< First type index where id occurs in table */
    int16_t count;                             /**< Number of times id occurs in table */
    int16_t column;                            /**< First column index where id occurs */
};

/** Type that contains information about which components got added/removed on
 * a table edge. */
typedef struct ecs_table_diff_t {
    ecs_type_t added;                /* Components added between tables */
    ecs_type_t removed;              /* Components removed between tables */
    ecs_flags32_t added_flags;
    ecs_flags32_t removed_flags;
} ecs_table_diff_t;

/* Tracks which/how many non-fragmenting children are stored in table for parent. */
typedef struct ecs_parent_record_t {
    uint32_t entity;                /* If table only contains a single entity for parent, this will contain the entity id (without generation). */
    int32_t count;                  /* The number of children for a parent in the table. */         
} ecs_parent_record_t;

/** Find record for entity. 
 * An entity record contains the table and row for the entity.
 * 
 * To use ecs_record_t::row as the record in the table, use:
 *   ECS_RECORD_TO_ROW(r->row)
 * 
 * This removes potential entity bitflags from the row field.
 * 
 * @param world The world.
 * @param entity The entity.
 * @return The record, NULL if the entity does not exist.
 */
FLECS_API
ecs_record_t* ecs_record_find(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get entity corresponding with record.
 * This operation only works for entities that are not empty.
 *
 * @param record The record for which to obtain the entity id.
 * @return The entity id for the record.
 */
FLECS_API
ecs_entity_t ecs_record_get_entity(
    const ecs_record_t *record);

/** Begin exclusive write access to entity.
 * This operation provides safe exclusive access to the components of an entity
 * without the overhead of deferring operations.
 *
 * When this operation is called simultaneously for the same entity more than
 * once it will throw an assert. Note that for this to happen, asserts must be
 * enabled. It is up to the application to ensure that access is exclusive, for
 * example by using a read-write mutex.
 *
 * Exclusive access is enforced at the table level, so only one entity can be
 * exclusively accessed per table. The exclusive access check is thread safe.
 *
 * This operation must be followed up with ecs_write_end().
 *
 * @param world The world.
 * @param entity The entity.
 * @return A record to the entity.
 */
FLECS_API
ecs_record_t* ecs_write_begin(
    ecs_world_t *world,
    ecs_entity_t entity);

/** End exclusive write access to entity.
 * This operation ends exclusive access, and must be called after
 * ecs_write_begin().
 *
 * @param record Record to the entity.
 */
FLECS_API
void ecs_write_end(
    ecs_record_t *record);

/** Begin read access to entity.
 * This operation provides safe read access to the components of an entity.
 * Multiple simultaneous reads are allowed per entity.
 *
 * This operation ensures that code attempting to mutate the entity's table will
 * throw an assert. Note that for this to happen, asserts must be enabled. It is
 * up to the application to ensure that this does not happen, for example by
 * using a read-write mutex.
 *
 * This operation does *not* provide the same guarantees as a read-write mutex,
 * as it is possible to call ecs_read_begin() after calling ecs_write_begin(). It is
 * up to application has to ensure that this does not happen.
 *
 * This operation must be followed up with ecs_read_end().
 *
 * @param world The world.
 * @param entity The entity.
 * @return A record to the entity.
 */
FLECS_API
const ecs_record_t* ecs_read_begin(
    ecs_world_t *world,
    ecs_entity_t entity);

/** End read access to entity.
 * This operation ends read access, and must be called after ecs_read_begin().
 *
 * @param record Record to the entity.
 */
FLECS_API
void ecs_read_end(
    const ecs_record_t *record);

/** Get component from entity record.
 * This operation returns a pointer to a component for the entity
 * associated with the provided record. For safe access to the component, obtain
 * the record with ecs_read_begin() or ecs_write_begin().
 *
 * Obtaining a component from a record is faster than obtaining it from the
 * entity handle, as it reduces the number of lookups required.
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) id.
 * @return Pointer to component, or NULL if entity does not have the component.
 *
 * @see ecs_record_ensure_id()
 */
FLECS_API
const void* ecs_record_get_id(
    const ecs_world_t *world,
    const ecs_record_t *record,
    ecs_id_t id);

/** Same as ecs_record_get_id(), but returns a mutable pointer.
 * For safe access to the component, obtain the record with ecs_write_begin().
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) id.
 * @return Pointer to component, or NULL if entity does not have the component.
 */
FLECS_API
void* ecs_record_ensure_id(
    ecs_world_t *world,
    ecs_record_t *record,
    ecs_id_t id);

/** Test if entity for record has a (component) id.
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) id.
 * @return Whether the entity has the component.
 */
FLECS_API
bool ecs_record_has_id(
    ecs_world_t *world,
    const ecs_record_t *record,
    ecs_id_t id);

/** Get component pointer from column/record. 
 * This returns a pointer to the component using a table column index. The
 * table's column index can be found with ecs_table_get_column_index().
 * 
 * Usage:
 * @code
 * ecs_record_t *r = ecs_record_find(world, entity);
 * int32_t column = ecs_table_get_column_index(world, table, ecs_id(Position));
 * Position *ptr = ecs_record_get_by_column(r, column, sizeof(Position));
 * @endcode
 * 
 * @param record The record.
 * @param column The column index in the entity's table.
 * @param size The component size.
 * @return The component pointer.
 */
FLECS_API
void* ecs_record_get_by_column(
    const ecs_record_t *record,
    int32_t column,
    size_t size);

/** Get component record for component id.
 * 
 * @param world The world.
 * @param id The component id.
 * @return The component record, or NULL if it doesn't exist.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_component_record_t* flecs_components_get(
    const ecs_world_t *world,
    ecs_id_t id);

/* Ensure component record for component id 
 * 
 * @param world The world.
 * @param id The component id.
 * @return The new or existing component record.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_component_record_t* flecs_components_ensure(
    ecs_world_t *world,
    ecs_id_t id);

/** Get component id from component record.
 * 
 * @param cr The component record.
 * @return The component id.
 */
FLECS_API
ecs_id_t flecs_component_get_id(
    const ecs_component_record_t *cr);

/** Get component flags for component.
 * 
 * @param id The component id.
 * @return The flags for the component id.
 */
FLECS_API
ecs_flags32_t flecs_component_get_flags(
    const ecs_world_t *world,
    ecs_id_t id);

/** Get type info for component record.
 * 
 * @param cr The component record.
 * @return The type info struct, or NULL if component is a tag.
 */
FLECS_API
const ecs_type_info_t* flecs_component_get_type_info(
    const ecs_component_record_t *cr);

/** Find table record for component record.
 * This operation returns the table record for the table/component record if it
 * exists. If the record exists, it means the table has the component.
 * 
 * @param cr The component record.
 * @param table The table.
 * @return The table record if the table has the component, or NULL if not.
 */
FLECS_API
FLECS_ALWAYS_INLINE const ecs_table_record_t* flecs_component_get_table(
    const ecs_component_record_t *cr,
    const ecs_table_t *table);

/** Ger parent record for component/table. 
 * A parent record stores how many children for a parent are stored in the 
 * specified table. If the table only stores a single child, the parent record
 * will also store the entity id of that child.
 * 
 * This information is used by queries to determine whether an O(n) search 
 * through the table is required to find all children for the parent. If the 
 * table only contains a single child the query can use 
 * ecs_parent_record_t::entity directly, otherwise it has to do a scan.
 * 
 * The component record specified to this function must be a ChildOf pair. Only
 * tables with children that use the non-fragmenting hierarchy storage will have
 * parent records.
 * 
 * @param cr The ChildOf component record.
 * @param table The table to check the number of children for.
 * @return The parent record if it exists, NULL if it does not.
*/
FLECS_API
FLECS_ALWAYS_INLINE ecs_parent_record_t* flecs_component_get_parent_record(
    const ecs_component_record_t *cr,
    const ecs_table_t *table);

/** Return hierarchy depth for component record.
 * The specified component record must be a ChildOf pair. This function does not
 * compute the depth, it just returns the precomputed depth that is updated 
 * automatically when hierarchy changes happen.
 * 
 * @param cr The ChildOf component record.
 * @return The depth of the parent's children in the hierarchy.
 */
FLECS_API
FLECS_ALWAYS_INLINE int32_t flecs_component_get_childof_depth(
    const ecs_component_record_t *cr);

/** Create component record iterator.
 * A component record iterator iterates all tables for the specified component
 * record.
 * 
 * The iterator should be used like this:
 * 
 * @code
 * ecs_table_cache_iter_t it;
 * if (flecs_component_iter(cr, &it)) {
 *   const ecs_table_record_t *tr;
 *   while ((tr = flecs_component_next(&it))) {
 *     ecs_table_t *table = tr->hdr.table;
 *     // ...
 *   }
 * }
 * @endcode
 * 
 * @param cr The component record.
 * @param iter_out Out parameter for the iterator.
 * @return True if there are results, false if there are no results.
 */
FLECS_API
bool flecs_component_iter(
    const ecs_component_record_t *cr,
    ecs_table_cache_iter_t *iter_out);

/** Get next table record for iterator.
 * Returns next table record for iterator.
 * 
 * @param iter The iterator.
 * @return The next table record, or NULL if there are no more results.
 */
FLECS_API
const ecs_table_record_t* flecs_component_next(
    ecs_table_cache_iter_t *iter);

/** Struct returned by flecs_table_records(). */
typedef struct ecs_table_records_t {
    const ecs_table_record_t *array;
    int32_t count;
} ecs_table_records_t;

/** Get table records. 
 * This operation returns an array with all records for the specified table.
 * 
 * @param table The table.
 * @return The table records for the table.
 */
FLECS_API
ecs_table_records_t flecs_table_records(
    ecs_table_t* table);

/** Get component record from table record.
 * 
 * @param tr The table record.
 * @return The component record.
 */
FLECS_API
ecs_component_record_t* flecs_table_record_get_component(
    const ecs_table_record_t *tr);

/** Get table id. 
 * This operation returns a unique numerical identifier for a table.
 * 
 * @param table The table.
 * @return The table records for the table.
 */
FLECS_API
uint64_t flecs_table_id(
    ecs_table_t* table);

/** Find table by adding id to current table. 
 * Same as ecs_table_add_id, but with additional diff parameter that contains
 * information about the traversed edge.
 * 
 * @param world The world.
 * @param table The table.
 * @param id_ptr Pointer to component id to add.
 * @param diff Information about traversed edge (out parameter).
 * @return The table that was traversed to.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_table_t *flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff);

#ifdef __cplusplus
}
#endif

#endif
