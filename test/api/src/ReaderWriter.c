#include <api.h>

static
int align(int value, int align_to) {
    return (((value - 1) / align_to) + 1) * align_to;
}

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

static
int simple_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {3, 4});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 6});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    Position *
    p = ecs_get_ptr(world, e1, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get_ptr(world, e2, Position);
    test_int(p->x, 3);
    test_int(p->y, 4);

    p = ecs_get_ptr(world, e3, Position);
    test_int(p->x, 5);
    test_int(p->y, 6);        

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_simple_w_4_byte_buffer() {
    simple_test(4);
}

void ReaderWriter_simple_w_exact_buffer() {
    int total = simple_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    simple_test(total);
}

void ReaderWriter_simple_w_smaller_buffer() {
    int total = simple_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    int smaller = align(total / 2, 4);
    simple_test(smaller);
}

void ReaderWriter_simple_w_larger_buffer() {
    int total = simple_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    simple_test(total * 2);
}

static
int id_test(int buffer_size) {
    ecs_world_t *world = ecs_init();
    
    /* Entities with different name lengths to test if alignment & fragmentation
     * works properly */
    ecs_entity_t e1 = ecs_set(world, 0, EcsId, {""});
    ecs_entity_t e2 = ecs_set(world, 0, EcsId, {"E"});
    ecs_entity_t e3 = ecs_set(world, 0, EcsId, {"E3"});
    ecs_entity_t e4 = ecs_set(world, 0, EcsId, {"E4E"});
    ecs_entity_t e5 = ecs_set(world, 0, EcsId, {"E5E5"});
    ecs_entity_t e6 = ecs_set(world, 0, EcsId, {"E6E6E"});
    ecs_entity_t e7 = ecs_set(world, 0, EcsId, {"E7E7E7E"});
    ecs_entity_t e8 = ecs_set(world, 0, EcsId, {"E8E8E8E8"});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));
    test_assert( !ecs_is_empty(world, e4));
    test_assert( !ecs_is_empty(world, e5));
    test_assert( !ecs_is_empty(world, e6));
    test_assert( !ecs_is_empty(world, e7));
    test_assert( !ecs_is_empty(world, e8));

    test_assert( ecs_has(world, e1, EcsId));
    test_assert( ecs_has(world, e2, EcsId));
    test_assert( ecs_has(world, e3, EcsId));
    test_assert( ecs_has(world, e4, EcsId));
    test_assert( ecs_has(world, e5, EcsId));
    test_assert( ecs_has(world, e6, EcsId));
    test_assert( ecs_has(world, e7, EcsId));
    test_assert( ecs_has(world, e8, EcsId));    

    test_str( ecs_get_id(world, e1), "");
    test_str( ecs_get_id(world, e2), "E");
    test_str( ecs_get_id(world, e3), "E3");
    test_str( ecs_get_id(world, e4), "E4E");
    test_str( ecs_get_id(world, e5), "E5E5");
    test_str( ecs_get_id(world, e6), "E6E6E");
    test_str( ecs_get_id(world, e7), "E7E7E7E");
    test_str( ecs_get_id(world, e8), "E8E8E8E8");

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_id_w_4_byte_buffer() {
    id_test(4);
}

void ReaderWriter_id_w_exact_buffer() {
    int total = id_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    id_test(total);
}

void ReaderWriter_id_w_smaller_buffer() {
    int total = id_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    int smaller = align(total / 2, 4);
    id_test(smaller);
}

void ReaderWriter_id_w_larger_buffer() {
    int total = id_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    id_test(total * 2);
}

static
int id_w_simple_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    /* Entities with different name lengths to test if alignment & fragmentation
     * works properly */
    ecs_entity_t e1 = 
    ecs_set(world, 0, EcsId, {""});
    ecs_set(world, e1, Position, {1, 2});

    ecs_entity_t e2 = 
    ecs_set(world, 0, EcsId, {"E"});
    ecs_set(world, e2, Position, {3, 4});

    ecs_entity_t e3 = 
    ecs_set(world, 0, EcsId, {"E3"});
    ecs_set(world, e3, Position, {5, 6});

    ecs_entity_t e4 = 
    ecs_set(world, 0, EcsId, {"E4E"});
    ecs_set(world, e4, Position, {7, 8});

    ecs_entity_t e5 = 
    ecs_set(world, 0, EcsId, {"E5E5"});
    ecs_set(world, e5, Position, {9, 10});

    ecs_entity_t e6 = 
    ecs_set(world, 0, EcsId, {"E6E6E"});
    ecs_set(world, e6, Position, {11, 12});   

    ecs_entity_t e7 = 
    ecs_set(world, 0, EcsId, {"E7E7E7E"});
    ecs_set(world, e7, Position, {13, 14});  

    ecs_entity_t e8 = 
    ecs_set(world, 0, EcsId, {"E8E8E8E8"});
    ecs_set(world, e8, Position, {15, 16});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));
    test_assert( !ecs_is_empty(world, e4));
    test_assert( !ecs_is_empty(world, e5));
    test_assert( !ecs_is_empty(world, e6));
    test_assert( !ecs_is_empty(world, e7));
    test_assert( !ecs_is_empty(world, e8));

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e4, Position));
    test_assert( ecs_has(world, e5, Position));
    test_assert( ecs_has(world, e6, Position));
    test_assert( ecs_has(world, e7, Position));
    test_assert( ecs_has(world, e8, Position));
    
    test_assert( ecs_has(world, e1, EcsId));
    test_assert( ecs_has(world, e2, EcsId));
    test_assert( ecs_has(world, e3, EcsId));
    test_assert( ecs_has(world, e4, EcsId));
    test_assert( ecs_has(world, e5, EcsId));
    test_assert( ecs_has(world, e6, EcsId));
    test_assert( ecs_has(world, e7, EcsId));
    test_assert( ecs_has(world, e8, EcsId));    

    Position *
    p = ecs_get_ptr(world, e1, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get_ptr(world, e2, Position);
    test_int(p->x, 3);
    test_int(p->y, 4);

    p = ecs_get_ptr(world, e3, Position);
    test_int(p->x, 5);
    test_int(p->y, 6);  

    p = ecs_get_ptr(world, e4, Position);
    test_int(p->x, 7);
    test_int(p->y, 8);  

    p = ecs_get_ptr(world, e5, Position);
    test_int(p->x, 9);
    test_int(p->y, 10);  

    p = ecs_get_ptr(world, e6, Position);
    test_int(p->x, 11);
    test_int(p->y, 12);  

    p = ecs_get_ptr(world, e7, Position);
    test_int(p->x, 13);
    test_int(p->y, 14);  

    p = ecs_get_ptr(world, e8, Position);
    test_int(p->x, 15);
    test_int(p->y, 16);

    test_str( ecs_get_id(world, e1), "");
    test_str( ecs_get_id(world, e2), "E");
    test_str( ecs_get_id(world, e3), "E3");
    test_str( ecs_get_id(world, e4), "E4E");
    test_str( ecs_get_id(world, e5), "E5E5");
    test_str( ecs_get_id(world, e6), "E6E6E");
    test_str( ecs_get_id(world, e7), "E7E7E7E");
    test_str( ecs_get_id(world, e8), "E8E8E8E8");

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_id_w_simple_4_byte_buffer() {
    id_w_simple_test(4);
}

void ReaderWriter_id_w_simple_exact_buffer() {
    int total = id_w_simple_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    id_w_simple_test(total);
}

void ReaderWriter_id_w_simple_smaller_buffer() {
    int total = id_w_simple_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    int smaller = align(total / 2, 4);
    id_w_simple_test(smaller);
}

void ReaderWriter_id_w_simple_larger_buffer() {
    int total = id_w_simple_test(4);
    test_assert(total > 4);
    test_assert(total % 4 == 0);
    id_w_simple_test(total * 2);
}
