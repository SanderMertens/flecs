#ifdef FLECS_READER_WRITER

/**
 * @file serializer.h
 * @brief Blob serializer API.
 */

#ifndef FLECS_READER_WRITER_H
#define FLECS_READER_WRITER_H

#ifdef __cplusplus
extern "C" {
#endif 

typedef enum ecs_blob_header_kind_t {
    EcsStreamHeader,

    /* Stream states */
    EcsTableSegment,
    EcsFooterSegment,

    /* Table segment */
    EcsTableHeader,
    EcsTableTypeSize,
    EcsTableType,
    EcsTableSize,
    EcsTableColumn,
    EcsTableColumnHeader,
    EcsTableColumnSize,
    EcsTableColumnData,

    /* Name column (EcsName) */
    EcsTableColumnNameHeader,
    EcsTableColumnNameLength,
    EcsTableColumnName,

    EcsStreamFooter  
} ecs_blob_header_kind_t;

typedef struct ecs_table_reader_t {
    ecs_blob_header_kind_t state;

    int32_t table_index;
    ecs_table_t *table;
    ecs_data_t *data;

    /* Current index in type */
    ecs_size_t type_written;
    ecs_type_t type;

    /* Current column */
    ecs_vector_t *column_vector;
    int32_t column_index;
    int32_t total_columns;

    /* Keep track of how much of the component column has been written */
    void *column_data;
    int16_t column_size;
    int16_t column_alignment;
    ecs_size_t column_written;

    /* Keep track of row when writing non-blittable data */
    int32_t row_index;
    int32_t row_count;

    /* Keep track of how much of an entity name has been written */
    const char *name;
    ecs_size_t name_len;
    ecs_size_t name_written;

    bool has_next_table;
} ecs_table_reader_t;

typedef struct ecs_reader_t {
    ecs_world_t *world;
    ecs_blob_header_kind_t state;
    ecs_iter_t data_iter;
    ecs_iter_next_action_t data_next;
    ecs_iter_t component_iter;
    ecs_iter_next_action_t component_next;
    ecs_table_reader_t table;
} ecs_reader_t;

typedef struct ecs_name_writer_t {
    char *name;
    int32_t written;
    int32_t len;
    int32_t max_len;
} ecs_name_writer_t;

typedef struct ecs_table_writer_t {
    ecs_blob_header_kind_t state;

    ecs_table_t *table;
    ecs_vector_t *column_vector;

    /* Keep state for parsing type */
    int32_t type_count;
    int32_t type_max_count;
    ecs_size_t type_written;
    ecs_entity_t *type_array;
    
    int32_t column_index;
    int16_t column_size;
    int16_t column_alignment;
    ecs_size_t column_written;
    void *column_data;

    int32_t row_count;
    int32_t row_index;
    ecs_name_writer_t name; 
} ecs_table_writer_t;

typedef struct ecs_writer_t {
    ecs_world_t *world;
    ecs_blob_header_kind_t state;
    ecs_table_writer_t table;
    int error;
} ecs_writer_t;

/** Initialize a reader.
 * A reader serializes data in a world to a sequence of bytes that can be stored
 * in a file or sent across a network. 
 *
 * @param world The world to serialize.
 * @return The reader.
 */
FLECS_EXPORT
ecs_reader_t ecs_reader_init(
    ecs_world_t *world);

/** Initialize a snapshot reader.
 * A snapshot reader serializes data in a snapshot to a sequence of bytes that 
 * can be stored in a file or sent across a network. A snapshot reader has as
 * advantage that serialization can take place asynchronously while the world
 * is progressing.
 *
 * @param world The world in which the snapshot is taken.
 * @param iter Iterator to the data to be serialized.
 * @return The reader.
 */
FLECS_EXPORT
ecs_reader_t ecs_reader_init_w_iter(
    ecs_iter_t *iter,
    ecs_iter_next_action_t next);

/** Read from a reader.
 * This operation reads a specified number of bytes from a reader and stores it
 * in the specified buffer. When there are no more bytes to read from the reader
 * the operation will return 0, otherwise it will return the number of bytes
 * read.
 *
 * The specified buffer must be at least as big as the specified size, and the
 * specified size must be a multiple of 4.
 *
 * @param buffer The buffer in which to store the read bytes.
 * @param size The maximum number of bytes to read.
 * @param reader The reader from which to read the bytes.
 * @return The number of bytes read.
 */ 
FLECS_EXPORT
ecs_size_t ecs_reader_read(
    char *buffer,
    ecs_size_t size,
    ecs_reader_t *reader);

/** Initialize a writer.
 * A writer deserializes data from a sequence of bytes into a world. This 
 * enables applications to restore data from disk or the network.
 *
 * The provided world must be either empty or compatible with the data to
 * deserialize, where compatible means that the serialized component ids and 
 * sizes must match exactly with those in the world. Errors can occur if a world
 * is provided in which components have been declared in a different order, or
 * when components have different type definitions.
 *
 * @param world The world in which to deserialize the data.
 * @return The writer. 
 */
FLECS_EXPORT
ecs_writer_t ecs_writer_init(
    ecs_world_t *world);

/** Write to a writer.
 * This operation writes a specified number of bytes from a specified buffer
 * into the writer. The writer will restore the deserialized data into the 
 * original serialized entities. The write operation may be invoked multiple
 * times with partial buffers, which allows applications to use static buffers
 * when reading from, for example, a file or the network.
 *
 * The data contained in the buffers must have been serialized with the
 * ecs_reader_read operation. If the data does not match the expected format, or
 * the data contains conflicts with the world, the operation will fail. The
 * data must be provided in the same order as produced by ecs_reader_read,
 * but the used buffer size does not have to be the same as the one used by
 * ecs_reader_read. The buffer size must be a multiple of 4.
 * 
 * @param buffer The buffer to deserialize.
 * @param size The maximum number of bytes.
 * @param writer The writer to write to.
 * @return Zero if success, non-zero if failed to deserialize.
 */
FLECS_EXPORT
int ecs_writer_write(
    const char *buffer,
    ecs_size_t size,
    ecs_writer_t *writer);

#ifdef __cplusplus
}
#endif     

#endif

#endif
