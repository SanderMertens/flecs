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
    void *buffer)
{
    int32_t written = writer->len - writer->written;
    char *name_ptr = ECS_OFFSET(writer->name, writer->written);

    if (written >= sizeof(int32_t)) {
        *(int32_t*)name_ptr = *(int32_t*)buffer;
        writer->written += sizeof(int32_t);
        return true;
    } else {
        memcpy(name_ptr, buffer, written);
        writer->written += written;
        return false;
    }
}

static
void ecs_component_writer_register_component(
    ecs_stream_t *stream)
{
    ecs_world_t *world = stream->world;
    ecs_component_writer_t *writer = &stream->writer.component;
    const char *name = writer->name.name;
    ecs_entity_t world_id = ecs_lookup(stream->world, name);
    ecs_entity_t id = writer->id;

    if (!world_id) {
        /* If the component is not found, but the id is set in the world, the id
         * is used for something else. Cannot deserialize when there is a
         * conflict */
        if (ecs_get_type(world, id)) {
            ecs_abort(ECS_DESERIALIZE_COMPONENT_ID_CONFLICT, name);
        }

        _ecs_add(world, id, world->t_component);
        ecs_set(world, id, EcsComponent, {writer->size});
        ecs_set(world, id, EcsId, {name});
    } else {
        if (world_id != id) {
            ecs_abort(ECS_DESERIALIZE_COMPONENT_ID_CONFLICT, name);
        } else {
            EcsComponent *cdata = ecs_get_ptr(world, id, EcsComponent);
            if (cdata->size != writer->size) {
                ecs_abort(ECS_DESERIALIZE_COMPONENT_SIZE_CONFLICT, name);
            }
        }
    } 
}

static
void ecs_component_writer_next(
    ecs_stream_t *stream)
{
    ecs_component_writer_t *writer = &stream->writer.component;

    switch(writer->state) {
    case EcsComponentId:
        writer->state = EcsComponentSize;
        break;
    case EcsComponentSize:
        writer->state = EcsComponentNameLength;
        break;
    case EcsComponentNameLength:
        writer->state = EcsComponentName;
        break;
    case EcsComponentName:
        stream->writer.state = EcsStreamHeader;
        writer->state = 0;
        break;
    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
        break;
    }    
}

static
size_t ecs_component_writer(
    void *buffer,
    size_t size,
    ecs_stream_t *stream)
{
    ecs_component_writer_t *writer = &stream->writer.component;
    size_t written = 0;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= sizeof(uint32_t), ECS_INVALID_PARAMETER, NULL);

    if (!writer->state) {
        writer->state = EcsComponentId;
    }

    switch(writer->state) {
    case EcsComponentId:
        writer->id = *(int32_t*)buffer;
        written = sizeof(int32_t);
        ecs_component_writer_next(stream);
        break;

    case EcsComponentSize:
        writer->size = *(int32_t*)buffer;
        written = sizeof(int32_t);
        ecs_component_writer_next(stream);  
        break;

    case EcsComponentNameLength:
        ecs_name_writer_alloc(&writer->name, *(int32_t*)buffer);
        written = sizeof(int32_t);
        ecs_component_writer_next(stream);
        break;

    case EcsComponentName: {
        written = sizeof(int32_t);
        if (!ecs_name_writer_write(&writer->name, buffer)) {
            ecs_component_writer_register_component(stream);
            ecs_component_writer_next(stream);            
        }
        break;
    }

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
        break;
    }

    return written;
}

static
void ecs_table_writer_next(
    ecs_stream_t *stream)
{
    ecs_table_writer_t *writer = &stream->writer.table;

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
        writer->column_row ++;
        if (writer->column_row < writer->table_count) {
            writer->state = EcsTableColumnNameLength;
            break;
        } else {
            /* Fallthrough on purpose */
        }

    case EcsTableColumnData:
        writer->column_index ++;
        if (writer->column_index > writer->type_len) {
            stream->writer.state = EcsStreamHeader;
            writer->state = 0;
            writer->column_index = 0;
            writer->column_written = 0;
            writer->column_row = 0;
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
void ecs_table_writer_register_table(
    ecs_stream_t *stream)
{
    ecs_world_t *world = stream->world;
    ecs_table_writer_t *writer = &stream->writer.table;
    ecs_type_t type = ecs_type_find(world, writer->type_array, writer->type_len);

    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    writer->table = ecs_world_get_table(world, &world->main_stage, type);

    ecs_assert(writer->table != NULL, ECS_INTERNAL_ERROR, NULL);
}

static
void ecs_table_writer_prepare_column(
    ecs_stream_t *stream,
    int32_t size)
{
    ecs_table_writer_t *writer = &stream->writer.table;

    writer->column = &writer->table->columns[writer->column_index];
    writer->column_size = size;

    ecs_assert(writer->column_size == writer->column->size, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_params_t params = {.element_size = writer->column_size};
    ecs_vector_set_count(&writer->column->data, &params, writer->table_count);
    writer->column_data = ecs_vector_first(writer->column->data);
    writer->column_written = 0;

    ecs_table_writer_next(stream);
}

static
size_t ecs_table_writer(
    void *buffer,
    size_t size,
    ecs_stream_t *stream)
{
    ecs_table_writer_t *writer = &stream->writer.table;
    size_t written = 0;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= sizeof(uint32_t), ECS_INVALID_PARAMETER, NULL);

    if (!writer->state) {
        writer->state = EcsTableTypeSize;
    }

    switch(writer->state) {
    case EcsTableTypeSize:
        writer->type_len = *(int32_t*)buffer;
        if (writer->type_len > writer->type_max_len) {
            ecs_os_free(writer->type_array);
            writer->type_array = ecs_os_malloc(writer->type_len * sizeof(ecs_entity_t));
            writer->type_max_len = writer->type_len;
            writer->type_current = 0;
        }
        written = sizeof(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableType:
        writer->type_array[writer->type_current] = *(int32_t*)buffer;
        writer->type_current ++;
        written += sizeof(int32_t);
        if (writer->type_current == writer->type_len) {
            ecs_table_writer_register_table(stream);
            ecs_table_writer_next(stream);
        }
        break;

    case EcsTableSize:
        writer->table_count = *(int32_t*)buffer;
        written += sizeof(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableColumn:
        writer->state = *(int32_t*)buffer;
        ecs_assert(
            writer->state == EcsTableColumnHeader || 
            writer->state == EcsTableColumnNameHeader, 
                ECS_DESERIALIZE_FORMAT_ERROR, NULL);
        written += sizeof(int32_t);
        break;

    case EcsTableColumnHeader:
    case EcsTableColumnSize:
        ecs_table_writer_prepare_column(stream, *(int32_t*)buffer);
        written += sizeof(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableColumnData: {
        written = writer->table_count * writer->column_size - writer->column_written;
        if (written > size) {
            written = size;
        }

        memcpy(writer->column_data, ECS_OFFSET(buffer, writer->column_written), written);
        writer->column_written += written;
        if (writer->column_written == writer->table_count * writer->column_size) {
            ecs_table_writer_next(stream);
        }
        break;
    }

    case EcsTableColumnNameHeader:
        ecs_table_writer_prepare_column(stream, sizeof(EcsId));

    case EcsTableColumnNameLength:
        ecs_name_writer_alloc(&writer->name, *(int32_t*)buffer);
        written = sizeof(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableColumnName: {
        written = sizeof(int32_t);
        if (!ecs_name_writer_write(&writer->name, buffer)) {
            ((EcsId*)writer->column_data)[writer->column_row] = writer->name.name;
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
}

size_t ecs_stream_write(
    void *buffer,
    size_t size,
    ecs_stream_t *stream)
{
    ecs_stream_writer_t *writer = &stream->writer;
    size_t written = 0, total_written = 0, remaining = size;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= sizeof(uint32_t), ECS_INVALID_PARAMETER, NULL);

    while (total_written < size) {
        if (writer->state == EcsStreamHeader) {
            writer->state = *(ecs_blob_header_kind_t*)ECS_OFFSET(buffer, total_written);
            written = sizeof(ecs_blob_header_kind_t);
        } else
        if (writer->state == EcsComponentHeader) {
            written = ecs_component_writer(ECS_OFFSET(buffer, total_written), remaining, stream);          
        } else
        if (writer->state == EcsTableHeader) {
            written = ecs_table_writer(ECS_OFFSET(buffer, total_written), remaining, stream);
        }

        if (!written) {
            break;
        }

        remaining -= written;
        total_written += written;    
    }

    ecs_assert(total_written <= size, ECS_INTERNAL_ERROR, NULL);

    return total_written;
}
