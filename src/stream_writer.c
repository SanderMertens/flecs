#include "flecs_private.h"

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
        writer->name_len = *(int32_t*)buffer;
        writer->name = ecs_os_malloc(writer->name_len);
        writer->name_written = 0;
        written = sizeof(int32_t);
        ecs_component_writer_next(stream);
        break;

    case EcsComponentName: {
        written = writer->name_len - writer->name_written;
        char *name_ptr = ECS_OFFSET(writer->name, writer->name_written);
        if (written >= sizeof(int32_t)) {
            *(int32_t*)name_ptr = *(int32_t*)buffer;
            writer->name_written += sizeof(int32_t);
        } else {
            memcpy(name_ptr, buffer, written);
            writer->name_written += written;
        }

        /* Buffer is aligned to multiples of 4 bytes */
        written = sizeof(int32_t);

        if (writer->name_written == writer->name_len) {
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

size_t ecs_stream_write(
    void *buffer,
    size_t size,
    ecs_stream_t *stream)
{
    ecs_stream_writer_t *writer = &stream->writer;
    size_t written, total_written = 0, remaining = size;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= sizeof(uint32_t), ECS_INVALID_PARAMETER, NULL);

    while (written && total_written < size) {
        if (writer->state == EcsStreamHeader) {
            writer->state = *(ecs_blob_header_kind_t*)ECS_OFFSET(buffer, total_written);
            written = sizeof(ecs_blob_header_kind_t);
            total_written += written;
        }

        if (writer->state == EcsComponentHeader) {
            written = ecs_component_writer(ECS_OFFSET(buffer, total_written), remaining, stream);
            remaining -= written;
            total_written += written;            
        } else
        if (writer->state == EcsTableHeader) {
            break;
        }
    }

    ecs_assert(total_written <= size, ECS_INTERNAL_ERROR, NULL);

    return total_written;
}
