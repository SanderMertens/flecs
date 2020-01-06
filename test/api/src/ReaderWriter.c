#include <api.h>

void Dummy(ecs_rows_t *rows) {
    ProbeSystem(rows);
}

static
ecs_vector_t* serialize_reader_to_vector(
    ecs_world_t *world, 
    int buffer_size,
    ecs_reader_t *reader) 
{
    ecs_vector_params_t params = {.element_size = 1};
    ecs_vector_t *v = ecs_vector_new(&params, 0);

    char *buffer = ecs_os_malloc(buffer_size);
    int read;

    while ((read = ecs_reader_read(buffer, buffer_size, reader))) {
        void *ptr = ecs_vector_addn(&v, &params, read);
        memcpy(ptr, buffer, read);
    }

    ecs_os_free(buffer);

    return v;
}

static
ecs_vector_t* serialize_to_vector(
    ecs_world_t *world, 
    int buffer_size) 
{
    ecs_reader_t reader = ecs_reader_init(world);
    return serialize_reader_to_vector(world, buffer_size, &reader);
}

static
ecs_vector_t* serialize_snapshot_to_vector(
    ecs_world_t *world, 
    int buffer_size,
    ecs_snapshot_t *snapshot) 
{
    ecs_reader_t reader = ecs_snapshot_reader_init(world, snapshot);
    return serialize_reader_to_vector(world, buffer_size, &reader);
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
ecs_world_t* deserialize_from_vector_to_existing_expect(
    ecs_vector_t *v,
    int buffer_size,
    ecs_world_t *world,
    int expect)
{
    ecs_writer_t writer = ecs_writer_init(world);
    int v_ptr = 0, read;
    char *buffer = ecs_os_malloc(buffer_size);

    while ((read = vector_read(buffer, buffer_size, v, &v_ptr))) {
        if (ecs_writer_write(buffer, read, &writer)) {
            if (expect) {
                test_assert(expect == writer.error);
                goto error;
            } else {
                printf("deserialize_from_vector: %s\n", ecs_strerror(writer.error));
                test_assert(false);
            }
        }
    }

    ecs_os_free(buffer);
    
    return world;
error:
    return NULL;
}

static
ecs_world_t* deserialize_from_vector_to_existing(
    ecs_vector_t *v,
    int buffer_size,
    ecs_world_t *world)
{    
    return deserialize_from_vector_to_existing_expect(v, buffer_size, world, 0);
}

static
ecs_world_t* deserialize_from_vector(
    ecs_vector_t *v,
    int buffer_size)
{
    ecs_world_t *world = ecs_init();
    return deserialize_from_vector_to_existing(v, buffer_size, world);
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

    test_int( ecs_count(world, Position), 3);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_simple() {
    int i, total = simple_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        simple_test(i);
    }
}

static
int id_test(int buffer_size) {
    ecs_world_t *world = ecs_init();
    
    int base_id_count = ecs_count(world, EcsId);

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

    int id_count = ecs_count(world, EcsId);

    test_assert(id_count == (base_id_count + 8));

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, EcsId), id_count);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, EcsId);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, id_count + 1); /* System itself also has EcsId */

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_id() {
    int i, total = id_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        id_test(i);
    }
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

    test_int( ecs_count(world, Position), 8);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position, EcsId);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 8);

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_id_w_simple() {
    int i, total = id_w_simple_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        id_w_simple_test(i);
    }
}

typedef char Byte;

static
int unaligned_test(int buffer_size, int entity_count) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Byte);

    ecs_entity_t first = 0;
    int i;
    for (i = 0; i < entity_count; i ++) {
        ecs_entity_t e = ecs_set(world, 0, Byte, {i});
        if (!i) {
            first = e;
        }
    }

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Byte), entity_count);

    for (i = 0; i < entity_count; i ++) {
        test_assert( !ecs_is_empty(world, first + i));
        test_assert( ecs_has(world, first + i, Byte));
        Byte *b = ecs_get_ptr(world, first + i, Byte);
        test_int(*b, i);
    }

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Byte);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, entity_count);

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_unaligned() {
    int size, count;
    for (count = 0; count < 64; count ++) {
        int total = unaligned_test(4, count);

        for (size = 8; size < total * 2; size += 8) {
            unaligned_test(size, count);
        }
    }
}

void ReaderWriter_empty() {
    ecs_world_t *world = ecs_init();

    ecs_vector_t *v = serialize_to_vector(world, 4);

    ecs_fini(world);

    world = deserialize_from_vector(v, 4);     

    ecs_fini(world);

    test_assert(true); // Not much to test besides it not crashing

    ecs_vector_free(v);
}

static
int tag_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Tag), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, Tag));
    test_assert( ecs_has(world, e2, Tag));
    test_assert( ecs_has(world, e3, Tag)); 

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Tag);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);        

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_tag() {
    int i, total = tag_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        tag_test(i);
    }
}

static
int simple_w_tag_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    /* Component comes before tag */
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_set(world, e1, Position, {1, 2});

    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_set(world, e2, Position, {3, 4});

    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_set(world, e3, Position, {5, 6});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Tag), 3);
    test_int( ecs_count(world, Position), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, Tag));
    test_assert( ecs_has(world, e2, Tag));
    test_assert( ecs_has(world, e3, Tag));  

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position, Tag);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);      

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;    
}

void ReaderWriter_simple_w_tag() {
    int i, total = simple_w_tag_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        simple_w_tag_test(i);
    }
}

static
int tag_w_simple_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    /* Component comes after tag */
    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_set(world, e1, Position, {1, 2});

    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_set(world, e2, Position, {3, 4});

    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_set(world, e3, Position, {5, 6});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Tag), 3);
    test_int( ecs_count(world, Position), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, Tag));
    test_assert( ecs_has(world, e2, Tag));
    test_assert( ecs_has(world, e3, Tag));  

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position, Tag);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);   

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;    
}

void ReaderWriter_tag_w_simple() {
    int i, total = tag_w_simple_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        tag_w_simple_test(i);
    }
}

static
int empty_parent_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_type_t ecs_type(parent) = ecs_type_from_entity(world, parent);

    ecs_entity_t e1 = ecs_new_child(world, parent, 0);
    ecs_entity_t e2 = ecs_new_child(world, parent, 0);
    ecs_entity_t e3 = ecs_new_child(world, parent, 0);

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, parent), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, parent));
    test_assert( ecs_has(world, e2, parent));
    test_assert( ecs_has(world, e3, parent));        

    test_assert( ecs_contains(world, parent, e1));
    test_assert( ecs_contains(world, parent, e2));
    test_assert( ecs_contains(world, parent, e3));  

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_empty_parent() {
    int i, total = empty_parent_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        empty_parent_test(i);
    }
}

int parent_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_set(world, 0, Position, {1, 2});
    ecs_type_t ecs_type(parent) = ecs_type_from_entity(world, parent);

    ecs_entity_t e1 = ecs_new_child(world, parent, 0);
    ecs_entity_t e2 = ecs_new_child(world, parent, 0);
    ecs_entity_t e3 = ecs_new_child(world, parent, 0);

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Position), 1);
    test_int( ecs_count(world, parent), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, parent));
    test_assert( ecs_has(world, e2, parent));
    test_assert( ecs_has(world, e3, parent));        

    test_assert( ecs_contains(world, parent, e1));
    test_assert( ecs_contains(world, parent, e2));
    test_assert( ecs_contains(world, parent, e3)); 

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, CONTAINER.Position);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);      

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;    
}

void ReaderWriter_parent() {
    int i, total = parent_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        parent_test(i);
    }
}

int simple_w_parent_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_set(world, 0, Position, {1, 2});
    ecs_type_t ecs_type(parent) = ecs_type_from_entity(world, parent);

    ecs_entity_t e1 = ecs_new_child(world, parent, 0);
    ecs_set(world, e1, Position, {1, 2});

    ecs_entity_t e2 = ecs_new_child(world, parent, 0);
    ecs_set(world, e2, Position, {3, 4});

    ecs_entity_t e3 = ecs_new_child(world, parent, 0);
    ecs_set(world, e3, Position, {5, 6});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Position), 4);
    test_int( ecs_count(world, parent), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));   

    test_assert( ecs_has(world, e1, parent));
    test_assert( ecs_has(world, e2, parent));
    test_assert( ecs_has(world, e3, parent));        

    test_assert( ecs_contains(world, parent, e1));
    test_assert( ecs_contains(world, parent, e2));
    test_assert( ecs_contains(world, parent, e3)); 

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, CONTAINER.Position);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);      

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;    
}

void ReaderWriter_simple_w_parent() {
    int i, total = simple_w_parent_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        simple_w_parent_test(i);
    }
}

int inheritance_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_set(world, 0, Position, {1, 2});
    ecs_type_t ecs_type(base) = ecs_type_from_entity(world, base);

    ecs_entity_t e1 = ecs_new_instance(world, base, 0);
    ecs_entity_t e2 = ecs_new_instance(world, base, 0);
    ecs_entity_t e3 = ecs_new_instance(world, base, 0);

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Position), 4);
    test_int( ecs_count(world, base), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, base));
    test_assert( ecs_has(world, e2, base));
    test_assert( ecs_has(world, e3, base));

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( !ecs_has_owned(world, e1, Position));
    test_assert( !ecs_has_owned(world, e2, Position));
    test_assert( !ecs_has_owned(world, e3, Position));

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 4);

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;    
}

void ReaderWriter_inheritance() {
    int i, total = inheritance_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        inheritance_test(i);
    }
}

int simple_w_inheritance_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_set(world, 0, Position, {1, 2});
    ecs_type_t ecs_type(base) = ecs_type_from_entity(world, base);

    ecs_entity_t e1 = ecs_new_instance(world, base, 0);
    ecs_set(world, e1, Velocity, {3, 4});
    ecs_entity_t e2 = ecs_new_instance(world, base, 0);
    ecs_set(world, e2, Velocity, {5, 6});
    ecs_entity_t e3 = ecs_new_instance(world, base, 0);
    ecs_set(world, e3, Velocity, {7, 8});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Position), 4);
    test_int( ecs_count(world, Velocity), 3);
    test_int( ecs_count(world, base), 3);

    test_assert( !ecs_is_empty(world, e1));
    test_assert( !ecs_is_empty(world, e2));
    test_assert( !ecs_is_empty(world, e3));

    test_assert( ecs_has(world, e1, base));
    test_assert( ecs_has(world, e2, base));
    test_assert( ecs_has(world, e3, base));

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    test_assert( !ecs_has_owned(world, e1, Position));
    test_assert( !ecs_has_owned(world, e2, Position));
    test_assert( !ecs_has_owned(world, e3, Position));

    Velocity *
    p = ecs_get_ptr(world, e1, Velocity);
    test_int(p->x, 3);
    test_int(p->y, 4);

    p = ecs_get_ptr(world, e2, Velocity);
    test_int(p->x, 5);
    test_int(p->y, 6);

    p = ecs_get_ptr(world, e3, Velocity);
    test_int(p->x, 7);
    test_int(p->y, 8); 

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position, Velocity);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;    
}

void ReaderWriter_simple_w_inheritance() {
    int i, total = simple_w_inheritance_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        simple_w_inheritance_test(i);
    }
}

int deserialize_twice_test(int buffer_size) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {3, 4});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 6});

    ecs_vector_t *v = serialize_to_vector(world, buffer_size);

    ecs_fini(world);

    world = deserialize_from_vector(v, buffer_size);
    world = deserialize_from_vector(v, buffer_size);

    test_int( ecs_count(world, Position), 3);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);

    ecs_fini(world);

    int total = ecs_vector_count(v);
    ecs_vector_free(v);
    return total;
}

void ReaderWriter_deserialize_twice() {
    int i, total = deserialize_twice_test(4);

    test_assert(total > 4);
    test_assert(total % 4 == 0);

    for (i = 8; i < total * 2; i += 4) {
        deserialize_twice_test(i);
    }
}

void ReaderWriter_entity_conflict() {
    ecs_entity_t e1;
    ecs_vector_t *v;

    {
        ecs_world_t *world = ecs_init();

        ECS_COMPONENT(world, Position);
        ECS_COMPONENT(world, Velocity);
        
        e1 = ecs_set(world, 0, Position, {1, 2});
        v = serialize_to_vector(world, 32);

        ecs_fini(world);
    }

    {
        ecs_world_t *world = ecs_init();

        ECS_COMPONENT(world, Position);
        ECS_COMPONENT(world, Velocity);

        /* Add entity with different component */
        ecs_add(world, e1, Velocity);

        deserialize_from_vector_to_existing(v, 32, world);

        test_int( ecs_count(world, Position), 1);
        test_int( ecs_count(world, Velocity), 0);

        test_assert( !ecs_is_empty(world, e1));
        test_assert( ecs_has(world, e1, Position));
        test_assert( !ecs_has(world, e1, Velocity));

        Position *p = ecs_get_ptr(world, e1, Position);
        test_int(p->x, 1);
        test_int(p->y, 2);

        ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);
        SysTestData ctx = {0};
        ecs_set_context(world, &ctx);      
        ecs_progress(world, 0);
        test_int(ctx.count, 1);

        ecs_fini(world);
    }

    ecs_vector_free(v);
}

void ReaderWriter_snapshot_reader_simple() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {3, 4});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 6});

    ecs_snapshot_t *snapshot = ecs_snapshot_take(world, NULL);

    ecs_vector_t *v = serialize_snapshot_to_vector(world, 36, snapshot);

    ecs_snapshot_free(world, snapshot);

    ecs_fini(world);

    world = deserialize_from_vector(v, 36);

    test_int( ecs_count(world, Position), 3);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, 3);

    ecs_fini(world);

    ecs_vector_free(v);
}

void ReaderWriter_snapshot_reader_id() {
    ecs_world_t *world = ecs_init();
    
    int base_id_count = ecs_count(world, EcsId);

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

    ecs_snapshot_t *snapshot = ecs_snapshot_take(world, NULL);
    ecs_vector_t *v = serialize_snapshot_to_vector(world, 36, snapshot);
    ecs_snapshot_free(world, snapshot);

    int id_count = ecs_count(world, EcsId);

    test_assert(id_count == (base_id_count + 8));

    ecs_fini(world);

    world = deserialize_from_vector(v, 36);

    test_int( ecs_count(world, EcsId), id_count);

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

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, EcsId);
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);      
    ecs_progress(world, 0);
    test_int(ctx.count, id_count + 1); /* System itself also has EcsId */

    ecs_fini(world);

    ecs_vector_free(v);
}

void ReaderWriter_component_id_conflict_w_component() {
    ecs_world_t *world = ecs_init();
    {
        ECS_COMPONENT(world, Position);
        ECS_COMPONENT(world, Velocity);
    }

    ecs_vector_t *v = serialize_to_vector(world, 36);

    ecs_fini(world);

    world = ecs_init();
    {
        ECS_COMPONENT(world, Velocity);
        ECS_COMPONENT(world, Position);
    }

    ecs_world_t *result = deserialize_from_vector_to_existing_expect(
        v, 36, world, ECS_DESERIALIZE_COMPONENT_ID_CONFLICT);
    test_assert(result == NULL);

    ecs_fini(world);

    ecs_vector_free(v);
}

void ReaderWriter_component_id_conflict_w_entity() {
    ecs_world_t *world = ecs_init();
    {
        ECS_COMPONENT(world, Position);
        ECS_COMPONENT(world, Velocity);
    }

    ecs_vector_t *v = serialize_to_vector(world, 36);

    ecs_fini(world);

    world = ecs_init();
    {
        ECS_COMPONENT(world, Position);
        ecs_new(world, Position);
    }

    ecs_world_t *result = deserialize_from_vector_to_existing_expect(
        v, 36, world, ECS_DESERIALIZE_COMPONENT_ID_CONFLICT);
    test_assert(result == NULL);

    ecs_fini(world);

    ecs_vector_free(v);
}

void ReaderWriter_component_size_conflict() {
    ecs_world_t *world = ecs_init();
    {
        ECS_COMPONENT(world, Position);
    }

    ecs_vector_t *v = serialize_to_vector(world, 36);

    ecs_fini(world);

    world = ecs_init();
    {
        ecs_new_component(world, "Position", sizeof(bool));
    }

    ecs_world_t *result = deserialize_from_vector_to_existing_expect(
        v, 36, world, ECS_DESERIALIZE_COMPONENT_SIZE_CONFLICT);
    test_assert(result == NULL);

    ecs_fini(world);

    ecs_vector_free(v);
}

void ReaderWriter_read_zero_size() {
    ecs_world_t *world = ecs_init();

    ecs_reader_t reader = ecs_reader_init(world);

    test_assert( ecs_reader_read(NULL, 0, &reader) == 0);

    ecs_fini(world);
}

void ReaderWriter_write_zero_size() {
    ecs_world_t *world = ecs_init();

    ecs_writer_t writer = ecs_writer_init(world);

    test_assert( ecs_writer_write(NULL, 0, &writer) == 0);

    ecs_fini(world);
}

void ReaderWriter_invalid_header() {
    ecs_world_t *world = ecs_init();

    ecs_writer_t writer = ecs_writer_init(world);

    char b = 42; // nonsense data

    test_assert( ecs_writer_write(&b, 4, &writer) != 0);

    test_assert( writer.error == ECS_DESERIALIZE_FORMAT_ERROR);

    ecs_fini(world);
}
