#include "../flecs_private.h"
#include "flecs/addon/reader_writer.h"

static
bool iter_table(
    ecs_world_t *world,
    ecs_table_reader_t *reader,
    ecs_iter_t *it,
    ecs_iter_next_action_t next,
    bool skip_builtin)    
{
    bool table_found = false;

    while (next(it)) {
        ecs_table_t *table = it->table;

        reader->table = table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_data_t *data = ecs_table_get_data(world, table);
        reader->data = data;
        reader->table_index ++;

        if (skip_builtin && reader->table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!data || !it->count) {
            continue;
        }

        table_found = true;
        break;
    }

    return table_found;
}

static
void next_table(
    ecs_world_t *world,
    ecs_reader_t *stream,
    ecs_table_reader_t *reader)
{    
    int32_t count;

    /* First iterate all component tables, as component data must always be
     * stored in a blob before anything else */
    bool table_found = iter_table(
        world, reader, &stream->component_iter, stream->component_next, 
        false);

    /* If all components have been added, add the regular data tables. Make sure
     * to not add component tables again, in case the provided iterator also
     * matches component tables. */
    if (!table_found) {
        table_found = iter_table(
            world, reader, &stream->data_iter, stream->data_next, true);
        count = stream->data_iter.count;
    } else {
        count = stream->component_iter.count;
    }

    if (!table_found) {
        stream->state = EcsFooterSegment;
    } else {
        reader->type = reader->table->type;
        reader->total_columns = reader->table->column_count + 1;
        reader->column_index = 0;
        reader->row_count = count;
    }
}

static
void ecs_table_reader_next(
    ecs_reader_t *stream)
{
    ecs_table_reader_t *reader = &stream->table;
    ecs_world_t *world = stream->world;

    switch(reader->state) {
    case EcsTableHeader:
        reader->state = EcsTableTypeSize;
        break;
    case EcsTableTypeSize:
        reader->state = EcsTableType;
        reader->type_written = 0;
        break;

    case EcsTableType:
        reader->state = EcsTableSize;
        break;

    case EcsTableSize: {
        reader->state = EcsTableColumnHeader;
        break;
    }

    case EcsTableColumnHeader:
        reader->state = EcsTableColumnSize;
        if (!reader->column_index) {
            reader->column_vector = reader->data->entities;
            reader->column_size = sizeof(ecs_entity_t);
        } else {
            ecs_column_t *column = 
                &reader->data->columns[reader->column_index - 1];
            reader->column_vector = column->data;
            reader->column_size = column->size;
            reader->column_alignment = column->alignment;
        }
        break;

    case EcsTableColumnSize:
        reader->state = EcsTableColumnData;
        reader->column_data = ecs_vector_first_t(reader->column_vector, 
            reader->column_size, reader->column_alignment);
        reader->column_written = 0;
        break;

    case EcsTableColumnNameHeader: {
        reader->state = EcsTableColumnNameLength;
        ecs_column_t *column = 
                &reader->data->columns[reader->column_index - 1];
        reader->column_vector = column->data;                
        reader->column_data = ecs_vector_first(reader->column_vector, EcsName);
        reader->row_index = 0;
        break;
    }

    case EcsTableColumnNameLength:
        reader->state = EcsTableColumnName;
        reader->row_index ++;
        break;

    case EcsTableColumnName:
        if (reader->row_index < reader->row_count) {
            reader->state = EcsTableColumnNameLength;
            break;
        }
        // fall through

    case EcsTableColumnData:
        reader->column_index ++;
        if (reader->column_index == reader->total_columns) {
            reader->state = EcsTableHeader;
            next_table(world, stream, reader);
        } else {
            ecs_entity_t *type_buffer = ecs_vector_first(reader->type, ecs_entity_t);
            if (reader->column_index >= 1) {
                ecs_entity_t e = type_buffer[reader->column_index - 1];
                
                if (e != ecs_entity(EcsName)) {
                    reader->state = EcsTableColumnHeader;
                } else {
                    reader->state = EcsTableColumnNameHeader;
                }
            } else {
                reader->state = EcsTableColumnHeader;
            }            
        }
        break;

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

    return;
}

static
size_t ecs_table_reader(
    char *buffer,
    size_t size,
    ecs_reader_t *stream)
{
    if (!size) {
        return 0;
    }

    if (size < sizeof(int32_t)) {
        return -1;
    }

    ecs_table_reader_t *reader = &stream->table;
    size_t read = 0;

    if (!reader->state) {
        next_table(stream->world, stream, reader);
        reader->state = EcsTableHeader;
    }

    switch(reader->state) {
    case EcsTableHeader:  
        *(ecs_blob_header_kind_t*)buffer = EcsTableHeader;
        read = sizeof(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);
        break;

    case EcsTableTypeSize:
        *(int32_t*)buffer = ecs_vector_count(reader->type);
        read = sizeof(int32_t);
        ecs_table_reader_next(stream);
        break;  

    case EcsTableType: {
        ecs_entity_t *type_array = ecs_vector_first(reader->type, ecs_entity_t);
        *(int32_t*)buffer = *(int32_t*)ECS_OFFSET(type_array, reader->type_written);
        reader->type_written += sizeof(int32_t);
        read = sizeof(int32_t);

        if (reader->type_written == ecs_vector_count(reader->type) * sizeof(ecs_entity_t)) {
            ecs_table_reader_next(stream);
        }
        break;                
    }

    case EcsTableSize:
        *(int32_t*)buffer = ecs_table_count(reader->table);
        read = sizeof(int32_t);
        ecs_table_reader_next(stream);
        break;

    case EcsTableColumnHeader:
        *(ecs_blob_header_kind_t*)buffer = EcsTableColumnHeader;
        read = sizeof(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);
        break; 

    case EcsTableColumnSize:
        *(int32_t*)buffer = reader->column_size;
        read = sizeof(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);

        if (!reader->column_size) {
            ecs_table_reader_next(stream);
        }
        break; 

    case EcsTableColumnData: {
        size_t column_bytes = reader->column_size * reader->row_count;
        read = column_bytes - reader->column_written;
        if (read > size) {
            read = size;
        }

        memcpy(buffer, ECS_OFFSET(reader->column_data, reader->column_written), read);
        reader->column_written += read;
        ecs_assert(reader->column_written <= column_bytes, ECS_INTERNAL_ERROR, NULL);

        size_t align = (((read - 1) / sizeof(int32_t)) + 1) * sizeof(int32_t);
        if (align != read) {
            /* Initialize padding bytes to 0 to keep valgrind happy */
            memset(ECS_OFFSET(buffer, read), 0, align - read);

            /* Set read to align so that data is always aligned to 4 bytes */
            read = align;

            /* Buffer sizes are expected to be aligned to 4 bytes and the rest
             * of the serialized data is aligned to 4 bytes. Should never happen
             * that adding padding bytes exceeds the size. */
            ecs_assert(read <= size, ECS_INTERNAL_ERROR, NULL);
        }

        if (reader->column_written == column_bytes) {
            ecs_table_reader_next(stream);
        }
        break;
    }

    case EcsTableColumnNameHeader:
        *(ecs_blob_header_kind_t*)buffer = EcsTableColumnNameHeader;
        read = sizeof(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);
        break;

    case EcsTableColumnNameLength:
        reader->name = ((EcsName*)reader->column_data)[reader->row_index].value;
        reader->name_len = strlen(reader->name) + 1;
        reader->name_written = 0;
        *(int32_t*)buffer = reader->name_len;
        read = sizeof(int32_t);
        ecs_table_reader_next(stream);    
        break;

    case EcsTableColumnName:   
        read = reader->name_len - reader->name_written;
        if (read >= sizeof(int32_t)) {

            int32_t i;
            for (i = 0; i < 4; i ++) {
                *(char*)ECS_OFFSET(buffer, i) = 
                    *(char*)ECS_OFFSET(reader->name, reader->name_written + i);
            }

            reader->name_written += sizeof(int32_t);
        } else {
            memcpy(buffer, ECS_OFFSET(reader->name, reader->name_written), read);
            memset(ECS_OFFSET(buffer, read), 0, sizeof(int32_t) - read);
            reader->name_written += read;
        }

        /* Always align buffer to multiples of 4 bytes */
        read = sizeof(int32_t);

        if (reader->name_written == reader->name_len) {
            ecs_table_reader_next(stream);
        }

        break;

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

    ecs_assert(read % 4 == 0, ECS_INTERNAL_ERROR, NULL);

    return read;
}

size_t ecs_reader_read(
    char *buffer,
    size_t size,
    ecs_reader_t *reader)
{
    size_t read, total_read = 0, remaining = size;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= sizeof(int32_t), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(size % 4 == 0, ECS_INVALID_PARAMETER, NULL);

    if (reader->state == EcsTableSegment) {
        while ((read = ecs_table_reader(ECS_OFFSET(buffer, total_read), remaining, reader))) {
            remaining -= read;
            total_read += read;

            if (reader->state != EcsTableSegment) {
                break;
            }

            ecs_assert(remaining % 4 == 0, ECS_INTERNAL_ERROR, NULL);        
        }
    }  
    
    return total_read;
}

ecs_reader_t ecs_reader_init(
    ecs_world_t *world)
{
    ecs_reader_t result = {
        .world = world,
        .state = EcsTableSegment,
        .component_iter = ecs_filter_iter(world, &(ecs_filter_t){
            .include = ecs_type(EcsComponent)
        }),
        .component_next = ecs_filter_next,
        .data_iter = ecs_filter_iter(world, NULL),
        .data_next = ecs_filter_next
    };

    return result;
}

ecs_reader_t ecs_reader_init_w_iter(
    ecs_iter_t *it,
    ecs_iter_next_action_t next)
{
    ecs_world_t *world = it->world;

    ecs_reader_t result = {
        .world = world,
        .state = EcsTableSegment,
        .component_iter = ecs_filter_iter(world, &(ecs_filter_t){
            .include = ecs_type(EcsComponent)
        }),
        .component_next = ecs_filter_next,
        .data_iter = *it,
        .data_next = next
    };

    return result;
}
