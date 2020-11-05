
/** Direct access API.
 *
 * This API allows for low-level direct access to tables and their columns. The
 * APIs primary intent is to provide fast primitives for new operations. It is
 * not recommended to use the API directly in application code, as invoking the
 * API in an incorrect way can lead to a corrupted datastore.
 */

#ifdef FLECS_DIRECT_ACCESS

#ifndef FLECS_DIRECT_ACCESS_H_
#define FLECS_DIRECT_ACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Find or create table with specified component string. 
 * The provided string must be a comma-separated list of fully qualified 
 * component identifiers. The returned table will have the specified components.
 * Two lists that are the same but specify components in a different order will
 * return the same table.
 *
 * @param world The world.
 * @param type The components.
 * @return The new or existing table, or NULL if the string contains an error.
 */
FLECS_API
ecs_table_t* ecs_table_from_str(
    ecs_world_t *world,
    const char *type);

/** Find or create table from type.
 * Same as ecs_table_from_str, but provides the type directly.
 *
 * @param world The world.
 * @param type The type.
 * @return The new or existing table.
 */
FLECS_API
ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_type_t type);

/** Get type for table.
 *
 * @param table The table.
 * @return The type of the table.
 */
FLECS_API
ecs_type_t ecs_table_get_type(
    ecs_table_t *table);

/** Insert record into table.
 * This will create a new record for the table, which inserts a value for each
 * component. An optional entity and record can be provided.
 *
 * If a non-zero entity id is provided, a record must also be provided and vice
 * versa. The record must be created by the entity index. If the provided record 
 * is not created for the specified entity, the behavior will be undefined.
 *
 * If the provided record is not managed by the entity index, the behavior will
 * be undefined.
 *
 * The returned record contains a reference to the table and the table row. The
 * data pointed to by the record is guaranteed not to move unless one or more
 * rows are removed from this table. A row can be removed as result of a delete,
 * or by adding/removing components from an entity stored in the table.
 *
 * @param world The world.
 * @param table The table.
 * @param entity The entity.
 * @param record The entity-index record for the specified entity.
 * @return A record containing the table and table row.
 */
FLECS_API
ecs_record_t ecs_table_insert(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_record_t *record);

/** Returns the number of records in the table. 
 * This operation returns the number of records that have been populated through
 * the regular (entity) API as well as the number of records that have been
 * inserted using the direct access API.
 *
 * @param world The world.
 * @param table The table.
 * @return The number of records in a table.
 */
FLECS_API
int32_t ecs_table_count(
    ecs_table_t *table);

/** Find the index of a column in a table.
 * Table columns are stored in the order of their respective component ids. As
 * this is not trivial for an application to deduce, this operation returns the
 * index of a column in a table for a given component. This index can be used
 * in other table operations to identify a column.
 *
 * The returned index is determined separately for each table. Indices obtained
 * for one table should not be used for another table.
 *
 * @param table The table.
 * @param component The component for which to retrieve the column index.
 * @return The column index, or -1 if the table does not have the component.
 */
FLECS_API
int32_t ecs_table_find_column(
    ecs_table_t *table,
    ecs_entity_t component);

/** Get table column.
 * This operation returns the pointer to a column array. A column contains all
 * the data for a component for the provided table in a contiguous array.
 *
 * The returned pointer is not stable, and may change when a table needs to
 * resize its arrays, for example in order to accomodate for more records.
 *
 * @param table The table.
 * @param column The column index.
 * @return Vector that contains the column array.
 */
FLECS_API
ecs_vector_t* ecs_table_get_column(
    ecs_table_t *table,
    int32_t column);

/** Set table column.
 * This operation enables an application to set a component column for a table.
 * After the operation the column is owned by the table. Any operations that
 * change the column after this operation can cause undefined behavior.
 *
 * Care must be taken that all columns in a table have the same number of
 * elements. If one column has less elements than another, the behavior is
 * undefined. The operation will not check if the assigned column is of the same
 * size as other columns, as this would prevent an application from assigning
 * a set of different columns to a table of a different size.
 *
 * Setting a column will not delete the previous column. It is the 
 * responsibility of the application to ensure that the old column is deleted
 * properly (using ecs_table_delete_column).
 *
 * The provided vector must have the same element size and alignment as the
 * target column. If the size and/or alignment do not match, the behavior will
 * be undefined. In debug mode the operation may assert.
 *
 * @param world The world.
 * @param table The table.
 * @param column The column index.
 * @param vector The column data to assing.
 */
FLECS_API
void ecs_table_set_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column,
    ecs_vector_t *vector);

/** Get the vector containing entity ids for the table.
 * This operation obtains the vector with entity ids for the current table. Each
 * entity id is associated with one record, and ids are stored in the same order
 * as the table records. The element type of the vector is ecs_entity_t.
 *
 * @param table The table.
 * @return The vector containing the table's entities.
 */
FLECS_API
ecs_vector_t* ecs_table_get_entities(
    ecs_table_t *table);

/** Get the vector containing pointers to entity records.
 * A table stores cached pointers to entity records for fast access. This 
 * operation provides direct access to the vector. The element type of the
 * vector is ecs_record_t*.
 *
 * @param table The table.
 * @return The vector containing the entity records.
 */ 
FLECS_API
ecs_vector_t* ecs_table_get_records(
    ecs_table_t *table);

/** Set the vector containing entity ids for the table.
 * This operation sets the vector with entity ids for a table. In addition the
 * operation also requires setting a vector with pointers to records. The
 * record pointers in the vector need to be managed by the entity index. If they
 * are not, this can cause undefined behavior.
 *
 * The provided vectors must have the same number of elements as the number of
 * records in the table. If the element count is not the same, this causes
 * undefined behavior.
 *
 * A table must have an entity and record vector, even if the table does not
 * contain entities. For each record that is not an entity, the entity vector
 * should contain 0, and the record vector should contain NULL.
 *
 * @param table The table.
 * @param entities The entity vector.
 * @param records The record vector.
 */
FLECS_API
void ecs_table_set_entities(
    ecs_table_t *table,
    ecs_vector_t *entities,
    ecs_vector_t *records);

/** Delete a column.
 * This operation frees the memory of a table column and will invoke the
 * component destructor if registered.
 *
 * The provided vector does not need to be the same as the vector in the table.
 * The reason the table must be provided is so that the operation can retrieve
 * the correct destructor for the component. If the component does not have a
 * destructor, an application can alternatively delete the vector directly.
 *
 * If the specified vector is NULL, the column of the table will be removed and
 * the table will be updated to no longer point at the column. If an explicit
 * column is provided, the table is not modified. If a column is deleted that is
 * still being pointed to by a table, behavior is undefined. It is the
 * responsibility of the application to ensure that a table no longer points to
 * a deleted column, by using ecs_table_set_column.
 *
 * Simultaneously, if this operation is used to delete a table column, the
 * application should make sure that if the table contains other columns, they
 * are either also deleted, or that the deleted column is replaced by a column
 * of the same size. Note that this also goes for the entity and record vectors,
 * they should have the same number of elements as the other columns.
 *
 * The vector must be of the same component as the specified column. If the
 * vector is not of the same component, behavior will be undefined. In debug
 * mode the API may assert, though it may not always be able to detect a
 * mismatching vector/column.
 *
 * After this operation the vector should no longer be used by the application.
 *
 * @param table The table.
 * @param column The column index.
 * @param vector The column vector to delete.
 */
FLECS_API
void ecs_table_delete_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column,
    ecs_vector_t *vector);

/** Find a record for a given entity.
 * This operation finds an existing record in the entity index for a given
 * entity. The returned pointer is stable for the lifecycle of the world and can
 * be used as argument for the ecs_record_update operation.
 *
 * The returned record (if found) points to the adminstration that relates an
 * entity id to a table. Updating the value of the returned record will cause
 * operations like ecs_get and ecs_has to look in the updated table.
 *
 * Updating this record to a table in which the entity is not stored causes
 * undefined behavior.
 *
 * When the entity has never been created or is not alive this operation will
 * return NULL.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The record that belongs to the entity, or NULL if not found.
 */
FLECS_API
ecs_record_t* ecs_record_find(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Get value from record.
 * This operation gets a component value from a record. The provided column
 * index must match the table of the record.
 *
 * @param r The record.
 * @param column The column index of the component to get.
 */ 
FLECS_API
void* ecs_record_get_column(
    ecs_record_t *r,
    int32_t column,
    size_t size);

/** Copy value to a component for a record.
 * This operation sets the component value of a single component for a record.
 * If the component type has a copy action it will be used, otherwise the value
 * be memcpyd into the component array.
 *
 * The provided record does not need to be managed by the entity index but does
 * need to point to a valid record in the table. If the provided index is
 * outside of the range indicating the number of records in the table, behavior
 * is undefined. In debug mode it will cause the operation to assert.
 *
 * @param world The world.
 * @param r The record to set.
 * @param column The column index of the component to set.
 * @param size The size of the component.
 * @param value Pointer to the value to copy.
 */
FLECS_API
void ecs_record_copy_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t size,
    const void *value,
    int32_t count);

/** Memcpy value to a component for a record.
 * Same as ecs_record_copy_to, except that this operation will always use
 * memcpy. This operation should only be used for components that can be safely
 * memcpyd. If the operation is used for a component that has a copy or move
 * action, the behavior is undefined. In debug mode the operation may assert.
 *
 * @param world The world.
 * @param r The record to set.
 * @param column The column index of the component to set.
 * @param size The size of the component.
 * @param value Pointer to the value to move. 
 */
FLECS_API
void ecs_record_copy_pod_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t size,
    const void *value,
    int32_t count);

/** Move value to a component for a record.
 * Same as ecs_record_copy_to, except that it uses the move action. If the 
 * component has no move action the value will be memcpyd into the component 
 * array. After this operation the application can no longer assume that the 
 * value passed into the function is valid.
 *
 * @param world The world.
 * @param r The record to set.
 * @param column The column index of the component to set.
 * @param size The size of the component.
 * @param value Pointer to the value to move.
 */
FLECS_API
void ecs_record_move_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t size,
    void *value,
    int32_t count);

#ifdef __cplusplus
}
#endif

#endif

#endif
