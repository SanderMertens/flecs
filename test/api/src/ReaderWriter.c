#include <api.h>

static
ecs_vector_t* serialize_to_vector(
    ecs_world_t *world, 
    int buffer_size) 
{
    ecs_vector_params_t params = {.element_size = 1};
    ecs_vector_t *v = ecs_vector_new(&params, 0);

    ecs_reader_t reader = ecs_reader_init(world);
    char buffer[buffer_size];
    int read;

    while ((read = ecs_reader_read(buffer, buffer_size, &reader))) {
        void *ptr = ecs_vector_addn(&v, &params, read);
        memcpy(ptr, buffer, read);
    }

    return v;
}

static
int vector_read(
    void *buffer,
    int size,
    ecs_vector_t *v,
    int *v_ptr)
{
    ecs_vector_params_t params = {.element_size = 1};
    int count = ecs_vector_count(v);
    int read = count - *v_ptr;

    if (read > size) {
        read = size;
    }

    if (!read) {
        return 0;
    }

    void *ptr = ecs_vector_get(v, &params, *v_ptr);
    memcpy(buffer, ptr, read);
    (*v_ptr) += read;

    return read;
}

static
ecs_world_t* deserialize_from_vector(
    ecs_vector_t *v,
    int buffer_size)
{
    ecs_world_t *world = ecs_init();
    ecs_writer_t writer = ecs_writer_init(world);
    int v_ptr = 0, read;
    char buffer[buffer_size];

    while ((read = vector_read(buffer, buffer_size, v, &v_ptr))) {
        if (ecs_writer_write(buffer, read, &writer)) {
            printf("deserialize_from_vector: %s\n", ecs_strerror(writer.error));
            test_assert(false);
        }
    }
    
    return world;
}

void ReaderWriter_simple_w_4_byte_buffer() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {3, 4});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 6});

    ecs_vector_t *v = serialize_to_vector(world, 4);

    ecs_fini(world);

    world = deserialize_from_vector(v, 4);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    ecs_fini(world);
}

void ReaderWriter_simple_w_exact_buffer() {
    // Implement testcase
}

void ReaderWriter_simple_w_smaller_buffer() {
    // Implement testcase
}

void ReaderWriter_simple_w_larger_buffer() {
    // Implement testcase
}
