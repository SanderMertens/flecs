/**
 * @file private/api_internals.h
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
    ecs_table_t *table;                        /**< Identifies a type (and table) in the world. */
    uint32_t row;                              /**< Table row of the entity. */
    int32_t dense;                             /**< Index in dense array of entity index. */
};

/** Header for table cache elements. */
typedef struct ecs_table_cache_hdr_t {
    struct ecs_component_record_t *cr;         /**< Component record for component. */
    ecs_table_t *table;                        /**< Table associated with element. */
} ecs_table_cache_hdr_t;

/** Record that stores the location of a component in a table.
 * Table records are registered with component records, which allows for quickly
 * finding all tables for a specific component. */
struct ecs_table_record_t {
    ecs_table_cache_hdr_t hdr;                 /**< Table cache header. */
    int16_t index;                             /**< First type index where ID occurs in table. */
    int16_t count;                             /**< Number of times ID occurs in table. */
    int16_t column;                            /**< First column index where ID occurs. */
};

/** Type that contains information about which components got added or removed on
 * a table edge. */
typedef struct ecs_table_diff_t {
    ecs_type_t added;                /* Components added between tables. */
    ecs_type_t removed;              /* Components removed between tables. */
    ecs_flags32_t added_flags;
    ecs_flags32_t removed_flags;
} ecs_table_diff_t;

/* Tracks which and how many non-fragmenting children are stored in a table for a parent. */
typedef struct ecs_parent_record_t {
    uint32_t entity;                /* If the table only contains a single entity for the parent, this will contain the entity ID (without generation). */
    int32_t count;                  /* The number of children for a parent in the table. */
} ecs_parent_record_t;

/** Find the record for an entity.
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

/** Get the entity corresponding to a record.
 * This operation only works for entities that are not empty.
 *
 * @param record The record for which to obtain the entity ID.
 * @return The entity ID for the record.
 */
FLECS_API
ecs_entity_t ecs_record_get_entity(
    const ecs_record_t *record);

/** Add multiple IDs to an entity.
 * This operation adds multiple IDs with at most one table move.
 *
 * @param world The world.
 * @param entity The entity.
 * @param ids The IDs to add.
 * @param count The number of IDs to add.
 */
FLECS_API
void flecs_add_ids(
    ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_id_t *ids,
    int32_t count);

/** Get a component from an entity record.
 * This operation returns a pointer to a component for the entity
 * associated with the provided record.
 *
 * Obtaining a component from a record is faster than obtaining it from the
 * entity handle, as it reduces the number of lookups required.
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) ID.
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
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) ID.
 * @return Pointer to component, or NULL if entity does not have the component.
 */
FLECS_API
void* ecs_record_ensure_id(
    ecs_world_t *world,
    ecs_record_t *record,
    ecs_id_t id);

/** Test if the entity for a record has a (component) ID.
 *
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) ID.
 * @return Whether the entity has the component.
 */
FLECS_API
bool ecs_record_has_id(
    ecs_world_t *world,
    const ecs_record_t *record,
    ecs_id_t id);

/** Get a component pointer from a column and record.
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

/** Get the component record for a component ID.
 *
 * @param world The world.
 * @param id The component ID.
 * @return The component record, or NULL if it doesn't exist.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_component_record_t* flecs_components_get(
    const ecs_world_t *world,
    ecs_id_t id);

/** Ensure a component record for a component ID.
 *
 * @param world The world.
 * @param id The component ID.
 * @return The new or existing component record.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_component_record_t* flecs_components_ensure(
    ecs_world_t *world,
    ecs_id_t id);

/** Get the component ID from a component record.
 *
 * @param cr The component record.
 * @return The component ID.
 */
FLECS_API
ecs_id_t flecs_component_get_id(
    const ecs_component_record_t *cr);

/** Get the component flags for a component.
 *
 * @param world The world.
 * @param id The component ID.
 * @return The flags for the component ID.
 */
FLECS_API
ecs_flags32_t flecs_component_get_flags(
    const ecs_world_t *world,
    ecs_id_t id);

/** Get the type info for a component record.
 * 
 * @param cr The component record.
 * @return The type info struct, or NULL if the component is a tag.
 */
FLECS_API
const ecs_type_info_t* flecs_component_get_type_info(
    const ecs_component_record_t *cr);

/** Get the sparse storage for a component record.
 * Returns the sparse set that stores values for components with the Sparse or
 * DontFragment trait, indexed by (unsigned 32 bit) entity id. */
FLECS_API
ecs_sparse_t* flecs_component_get_sparse(
    const ecs_component_record_t *cr);

/** Find the table record for a component record.
 * This operation returns the table record for the table and component record if it
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

/** Get the parent record for a component and table.
 * A parent record stores how many children for a parent are stored in the 
 * specified table. If the table only stores a single child, the parent record
 * will also store the entity ID of that child.
 * 
 * This information is used by queries to determine whether an O(n) search 
 * through the table is required to find all children for the parent. If the 
 * table only contains a single child, the query can use
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

/** Return the hierarchy depth for a component record.
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

/** Create a component record iterator.
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

/** Get the next table record for the iterator.
 * Returns the next table record, or NULL if there are no more results.
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

/** Get the table records.
 * This operation returns an array with all records for the specified table.
 * 
 * @param table The table.
 * @return The table records for the table.
 */
FLECS_API
ecs_table_records_t flecs_table_records(
    ecs_table_t* table);

/** Get the component record from a table record.
 * 
 * @param tr The table record.
 * @return The component record.
 */
FLECS_API
ecs_component_record_t* flecs_table_record_get_component(
    const ecs_table_record_t *tr);

/** Get the sparse storage for a row field.
 * Returns the sparse set that stores values for a field returned per-row (see
 * ecs_field_at()), or NULL when the field has a non-$this source. */
FLECS_API
ecs_sparse_t* flecs_field_sparse(
    const ecs_iter_t *it,
    int8_t index);

/** Get the table ID.
 * This operation returns a unique numerical identifier for a table.
 *
 * @param table The table.
 * @return The unique identifier for the table.
 */
FLECS_API
uint64_t flecs_table_id(
    ecs_table_t* table);

/** Get the table flags.
 * See include/flecs/private/api_flags.h for a list of table flags. */
FLECS_API
ecs_flags32_t flecs_table_flags(
    const ecs_table_t* table);

/** Find a table by adding an ID to the current table.
 * Same as ecs_table_add_id(), but with an additional diff parameter that contains
 * information about the traversed edge.
 *
 * @param world The world.
 * @param table The table.
 * @param id_ptr Pointer to the component ID to add.
 * @param diff Information about the traversed edge (out parameter).
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
