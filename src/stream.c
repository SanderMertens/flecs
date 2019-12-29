#include "flecs_private.h"

ecs_stream_t ecs_stream_open(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    return (ecs_stream_t){
        .world = world,
        .reader.cur = EcsComponentSegment
    };
}

void ecs_stream_close(
    ecs_stream_t *stream)
{

}

static
void ecs_component_reader_fetch_component_data(
    ecs_stream_t *stream)
{
    ecs_component_reader_t *reader = &stream->reader.is.component;
    ecs_world_t *world = stream->world;
    ecs_chunked_t *tables = world->main_stage.tables;

    /* Component table is the first table in the world */
    ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, 0);
    reader->id_column = ecs_vector_first(table->columns[0].data);
    reader->data_column = ecs_vector_first(table->columns[1].data);
    reader->name_column = ecs_vector_first(table->columns[2].data);
    reader->count = ecs_vector_count(table->columns[0].data);
}

static
void ecs_component_reader_next(
    ecs_stream_t *stream)
{
    ecs_component_reader_t *reader = &stream->reader.is.component;

    switch(reader->cur) {
    case EcsComponentHeader:  
        reader->cur = EcsComponentId;
        if (!reader->id_column) {
            ecs_component_reader_fetch_component_data(stream);
            reader->index = 0;
        }
        break;
    case EcsComponentId:
        reader->cur = EcsComponentSize;
        break;
    case EcsComponentSize:
        reader->cur = EcsComponentNameLength;
        reader->name = reader->name_column[reader->index];
        reader->len = strlen(reader->name) + 1; 
        break;        
    case EcsComponentNameLength:
        reader->cur = EcsComponentName;
        reader->written = 0;
        break;
    case EcsComponentName:
        break;
    }
}

static
size_t ecs_component_reader(
    void *buffer,
    size_t size,
    ecs_stream_t *stream)
{
    if (!size) {
        return 0;
    }

    if (size < sizeof(int32_t)) {
        return -1;
    }

    ecs_component_reader_t *reader = &stream->reader.is.component;
    size_t read = 0;

    switch(reader->cur) {
    case EcsComponentHeader:  
        *(ecs_segment_kind_t*)buffer = EcsComponentSegment;
        read = sizeof(ecs_segment_kind_t);
        ecs_component_reader_next(stream);
        break;

    case EcsComponentId:
        *(int32_t*)buffer = (int32_t)reader->id_column[reader->index];
        read = sizeof(int32_t);
        ecs_component_reader_next(stream);
        break;

    case EcsComponentSize:
        *(int32_t*)buffer = (int32_t)reader->data_column[reader->index].size;
        read = sizeof(int32_t);
        ecs_component_reader_next(stream);
        break;

    case EcsComponentNameLength:
        *(int32_t*)buffer = (int32_t)reader->len;
        read = sizeof(int32_t);
        ecs_component_reader_next(stream);    
        break;

    case EcsComponentName: {
        read = reader->len - reader->written;
        if (read > size) {
            read = size;
        }

        memcpy(buffer, ECS_OFFSET(reader->name, reader->written), read);
        reader->written += read;

        ecs_assert(reader->written <= reader->len, ECS_INTERNAL_ERROR, NULL);

        if (reader->written == reader->len) {
            ecs_component_reader_next(stream);
        }
        break;
    }
    }

    return read;
}

static
size_t ecs_table_reader(
    void *buffer,
    size_t size,
    ecs_stream_t *stream)
{
    return 0;
}

size_t ecs_stream_read(
    void *buffer,
    size_t size,
    ecs_stream_t *stream)
{
    ecs_stream_reader_t *reader = &stream->reader;
    size_t read, total_read = 0, remaining = size;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= sizeof(int32_t), ECS_INVALID_PARAMETER, NULL);

    if (reader->cur == EcsComponentSegment) {
        while ((read = ecs_component_reader(ECS_OFFSET(buffer, total_read), remaining, stream))) {
            if (read == -1) {
                break;
            }

            remaining -= read;
            total_read += read;
        }

        if (read == -1) {
            return total_read;
        }

        if (!read && remaining) {
            reader->cur = EcsTableSegment;
        }
    }

    if (reader->cur == EcsTableSegment) {
        while ((read = ecs_table_reader(ECS_OFFSET(buffer, total_read), remaining, stream))) {
            remaining -= read;
            total_read += read;
        }
    }

    return total_read;
}