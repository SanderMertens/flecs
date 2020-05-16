#include "flecs_private.h"

static
void ecs_name_writer_alloc(
    ecs_name_writer_t *writer,
    int32_t len)
{
    writer->len = len;
    if (writer->len > writer->max_len) {
        ecs_os_free(writer->name);
        writer->name = ecs_os_malloc(writer->len);
        writer->max_len = writer->len;
    }
    writer->written = 0;
}

static
bool ecs_name_writer_write(
    ecs_name_writer_t *writer,
    const char *buffer)
{
    size_t written = writer->len - writer->written;
    char *name_ptr = ECS_OFFSET(writer->name, writer->written);

    if (written >= sizeof(int32_t)) {
        *(int32_t*)name_ptr = *(int32_t*)buffer;
        writer->written += sizeof(int32_t);
        return writer->written != writer->len;
    } else {
        memcpy(name_ptr, buffer, written);
        writer->written += written;
        return false;
    }
}

static
void ecs_name_writer_reset(
    ecs_name_writer_t *writer)
{
    writer->name = NULL;
    writer->max_len = 0;
    writer->len = 0;
}

static
void ecs_table_writer_register_table(
    ecs_writer_t *stream)
{
    ecs_world_t *world = stream->world;
    ecs_table_writer_t *writer = &stream->table;
    ecs_type_t type = ecs_type_find(world, writer->type_array, writer->type_count);

    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    writer->table = ecs_table_from_type(world, &world->stage, type);
    ecs_assert(writer->table != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, writer->table);
    if (data->entities) {
        /* Remove any existing entities from entity index */
        ecs_vector_t *entity_vector = data->entities;
        ecs_entity_t *entities = ecs_vector_first(entity_vector);
        int32_t i, count = ecs_vector_count(entity_vector);
        for (i = 0; i < count; i ++) {
            ecs_eis_delete(&world->stage, entities[i]);
        }        
        return;
    } else {
        /* Set size of table to 0. This will initialize columns */
        ecs_table_set_size(world, writer->table, data, 0);
    }

    ecs_assert(writer->table != NULL, ECS_INTERNAL_ERROR, NULL);
}

static
void ecs_table_writer_finalize_table(
    ecs_writer_t *stream)
{
    ecs_world_t *world = stream->world;
    ecs_table_writer_t *writer = &stream->table;

    /* Register entities in table in entity index */
    ecs_data_t *data = ecs_table_get_data(world, writer->table);
    ecs_vector_t *entity_vector = data->entities;
    ecs_entity_t *entities = ecs_vector_first(entity_vector);
    int32_t i, count = ecs_vector_count(entity_vector);

    for (i = 0; i < count; i ++) {
        ecs_record_t *record_ptr = ecs_eis_get(&world->stage, entities[i]);

        if (record_ptr) {
            if (record_ptr->table != writer->table) {
                ecs_table_t *table = record_ptr->table;                
                ecs_data_t *data = ecs_table_get_data(world, table);

                ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

                ecs_table_delete(world, &world->stage, 
                    table, data, record_ptr->row - 1);
            }
        }

        ecs_record_t record = (ecs_record_t){
            .row = i + 1,
            .table = writer->table
        };

        ecs_eis_set(&world->stage, entities[i], &record);

        if (entities[i] >= world->last_handle) {
            world->last_handle = entities[i] + 1;
        }
    }   
}

static
void ecs_table_writer_prepare_column(
    ecs_writer_t *stream,
    int32_t size)
{
    ecs_table_writer_t *writer = &stream->table;
    ecs_world_t *world = stream->world;
    ecs_data_t *data = ecs_table_get_or_create_data(
        world, &world->stage, writer->table);
        
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (writer->column_index) {
        ecs_column_t *column = &data->columns[writer->column_index - 1];

        if (size) {
            _ecs_vector_set_count(&column->data, size, writer->row_count);
        }

        writer->column_vector = column->data;
        writer->column_size = size;
    } else {
        ecs_vector_set_count(
            &data->entities, ecs_entity_t, writer->row_count);

        writer->column_vector = data->entities;
        writer->column_size = sizeof(ecs_entity_t);      
    }

    writer->column_data = ecs_vector_first(writer->column_vector);
    writer->column_written = 0;
}

static
void ecs_table_writer_next(
    ecs_writer_t *stream)
{
    ecs_table_writer_t *writer = &stream->table;

    switch(writer->state) {
    case EcsTableTypeSize:
        writer->state = EcsTableType;
        break;

    case EcsTableType:
        writer->state = EcsTableSize;
        break;

    case EcsTableSize:
        writer->state = EcsTableColumn;
        break;

    case EcsTableColumnHeader:
        writer->state = EcsTableColumnSize;
        break;

    case EcsTableColumnSize:
        writer->state = EcsTableColumnData;
        break;

    case EcsTableColumnNameHeader:
        writer->state = EcsTableColumnNameLength;
        break;

    case EcsTableColumnNameLength:
        writer->state = EcsTableColumnName;
        break;

    case EcsTableColumnName:
        writer->row_index ++;
        if (writer->row_index < writer->row_count) {
            writer->state = EcsTableColumnNameLength;
            break;
        } else {
            /* Fallthrough on purpose */
        }

    case EcsTableColumnData:
        writer->column_index ++;

        if (writer->column_index > writer->type_count) {
            ecs_table_writer_finalize_table(stream);
            stream->state = EcsStreamHeader;
            writer->column_written = 0;
            writer->state = 0;
            writer->column_index = 0;
            writer->row_index = 0;
        } else {
            writer->state = EcsTableColumn;
        }
        break;

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
        break;
    }
}

static
size_t ecs_table_writer(
    const char *buffer,
    size_t size,
    ecs_writer_t *stream)
{
    ecs_table_writer_t *writer = &stream->table;
    size_t written = 0;

    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size >= sizeof(int32_t), ECS_INVALID_PARAMETER, NULL);

    if (!writer->state) {
        writer->state = EcsTableTypeSize;
    }

    switch(writer->state) {
    case EcsTableTypeSize:
        writer->type_count = *(int32_t*)buffer;
        if (writer->type_count > writer->type_max_count) {
            ecs_os_free(writer->type_array);
            writer->type_array = ecs_os_malloc(writer->type_count * sizeof(ecs_entity_t));
            writer->type_max_count = writer->type_count;
        }
        writer->type_written = 0;
        written = sizeof(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableType:
        *(int32_t*)ECS_OFFSET(writer->type_array, writer->type_written) = *(int32_t*)buffer;
        written = sizeof(int32_t);
        writer->type_written += written;

        if (writer->type_written == writer->type_count * sizeof(ecs_entity_t)) {
            ecs_table_writer_register_table(stream);
            ecs_table_writer_next(stream);
        }
        break;

    case EcsTableSize:
        writer->row_count = *(int32_t*)buffer;
        written += sizeof(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableColumn:
        writer->state = *(int32_t*)buffer;
        if (writer->state != EcsTableColumnHeader &&
            writer->state != EcsTableColumnNameHeader)
        {
            stream->error = ECS_DESERIALIZE_FORMAT_ERROR;
            goto error;
        }
        written += sizeof(int32_t);
        break;

    case EcsTableColumnHeader:
    case EcsTableColumnSize:
        ecs_table_writer_prepare_column(stream, *(int32_t*)buffer);
        ecs_table_writer_next(stream);

        written += sizeof(int32_t);
        ecs_table_writer_next(stream);

        /* If column has no size, there will be no column data, so skip to the
         * next state. */
        if (!writer->column_size) {
            ecs_table_writer_next(stream);
        }

        break;

    case EcsTableColumnData: {
        written = writer->row_count * writer->column_size - writer->column_written;
        if (written > size) {
            written = size;
        }

        memcpy(ECS_OFFSET(writer->column_data, writer->column_written), buffer, written);
        writer->column_written += written;
        written = (((written - 1) / sizeof(int32_t)) + 1) * sizeof(int32_t);

        if (writer->column_written == writer->row_count * writer->column_size) {
            ecs_table_writer_next(stream);
        }
        break;
    }

    case EcsTableColumnNameHeader:
        ecs_table_writer_prepare_column(stream, sizeof(EcsName));
        ecs_table_writer_next(stream);

    case EcsTableColumnNameLength:
        ecs_name_writer_alloc(&writer->name, *(int32_t*)buffer);
        written = sizeof(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableColumnName: {
        written = sizeof(int32_t);
        if (!ecs_name_writer_write(&writer->name, buffer)) {
            ((EcsName*)writer->column_data)[writer->row_index] = writer->name.name;

            /* Don't overwrite entity name */
            ecs_name_writer_reset(&writer->name);   

            ecs_table_writer_next(stream);
        }
        break;
    }

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
        break;
    }

    ecs_assert(written <= size, ECS_INTERNAL_ERROR, NULL);

    return written;
error:
    return -1;
}

int ecs_writer_write(
    const char *buffer,
    size_t size,
    ecs_writer_t *writer)
{
    size_t written = 0, total_written = 0, remaining = size;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= sizeof(int32_t), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(size % 4 == 0, ECS_INVALID_PARAMETER, NULL);

    while (total_written < size) {
        if (writer->state == EcsStreamHeader) {
            writer->state = *(ecs_blob_header_kind_t*)ECS_OFFSET(buffer, total_written);

            if (writer->state != EcsTableHeader) {
                writer->error = ECS_DESERIALIZE_FORMAT_ERROR;
                goto error;
            }

            written = sizeof(ecs_blob_header_kind_t);
        } else
        if (writer->state == EcsTableHeader) {
            written = ecs_table_writer(ECS_OFFSET(buffer, total_written), remaining, writer);
        }

        if (!written) {
            break;
        }

        if (written == (size_t)-1) {
            goto error;
        }

        remaining -= written;
        total_written += written;    
    }

    ecs_assert(total_written <= size, ECS_INTERNAL_ERROR, NULL);

    return total_written == 0;
error:
    return -1;
}

ecs_writer_t ecs_writer_init(
    ecs_world_t *world)
{
    return (ecs_writer_t){
        .world = world,
        .state = EcsStreamHeader,
    };
}
