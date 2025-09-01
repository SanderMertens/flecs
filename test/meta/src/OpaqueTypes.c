#include <meta.h>

ECS_COMPONENT_DECLARE(Position);

static int serialize_invoked = 0;

typedef int32_t Int32;
typedef char* String;

typedef struct IntArray {
    Int32 array[3];
} IntArray;

typedef struct IntVec {
    size_t count;
    Int32 *array;
} IntVec;

typedef struct StringVec {
    size_t count;
    String *array;
} StringVec;

typedef struct StructVec {
    size_t count;
    Position *array;
} StructVec;

typedef struct {
    int32_t _dummy_1;
    int32_t x;
    int32_t _dummy_2;
    int32_t y;
} OpaqueStruct;

typedef struct {
    OpaqueStruct foo;
    OpaqueStruct bar;
} Struct_w_OpaqueStruct;

typedef struct {
    IntArray foo;
    IntArray bar;
} Struct_w_IntArray;

typedef struct {
    IntVec foo;
    IntVec bar;
} Struct_w_IntVec;

// Custom serializer functions
static
int Int32_serialize(const ecs_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);
    int result = ser->value(ser, ecs_id(ecs_i32_t), ptr);
    test_assert(result == 0);
    serialize_invoked ++;
    return result;
}

static
int String_serialize(const ecs_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);
    int result = ser->value(ser, ecs_id(ecs_string_t), ptr);
    test_assert(result == 0);
    serialize_invoked ++;
    return result;
}

static
int IntVec_serialize(const ecs_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);

    const IntVec *data = ptr;
    for (size_t i = 0; i < data->count; i ++) {
        int result = ser->value(ser, ecs_id(ecs_i32_t), &data->array[i]);
        test_assert(result == 0);
    }

    serialize_invoked ++;
    return 0;
}

static 
void* IntArray_ensure(void *ptr, size_t index) {
    IntArray *data = ptr;
    return &data->array[index];
}

static 
size_t IntVec_count(const void *ptr) {
    const IntVec *data = ptr;
    return data->count;
}

static 
void* IntVec_ensure(void *ptr, size_t index) {
    IntVec *data = ptr;
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, int32_t, data->count);
    }
    return &data->array[index];
}

static 
void IntVec_resize(void *ptr, size_t size) {
    IntVec *data = ptr;
    if (data->count != size) {
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, int32_t, size);
        }
    }
}

static
int StringVec_serialize(const ecs_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);

    const StringVec *data = ptr;
    for (size_t i = 0; i < data->count; i ++) {
        int result = ser->value(ser, ecs_id(ecs_string_t), &data->array[i]);
        test_assert(result == 0);
    }

    serialize_invoked ++;
    return 0;
}

static
size_t StringVec_count(const void *ptr) {
    const StringVec *data = ptr;
    return data->count;
}

static 
void* StringVec_ensure(void *ptr, size_t index) {
    StringVec *data = ptr;
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, char*, data->count);
        data->array[index] = NULL;
    }
    return &data->array[index];
}

static 
void StringVec_resize(void *ptr, size_t size) {
    StringVec *data = ptr;
    if (data->count != size) {
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, char*, size);
        }
    }
}

static
size_t StructVec_count(const void *ptr) {
    const StructVec *data = ptr;
    return data->count;
}

static 
void* StructVec_ensure(void *ptr, size_t index) {
    StructVec *data = ptr;
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, Position, data->count);
    }
    return &data->array[index];
}

static 
void StructVec_resize(void *ptr, size_t size) {
    StructVec *data = ptr;
    if (data->count != size) {
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, Position, size);
        }
    }
}

static
void* OpaqueStruct_member(void *ptr, const char *member) {
    OpaqueStruct *data = ptr;
    if (!strcmp(member, "x")) {
        return &data->x;
    } else if (!strcmp(member, "y")) {
        return &data->y;
    } else {
        return NULL;
    }
}

void OpaqueTypes_ser_i32_type_to_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Int32);

    ecs_opaque(world, {
        .entity = ecs_id(Int32),
        .type.as_type = ecs_primitive(world, { .kind = EcsI32 }),
        .type.serialize = Int32_serialize
    });

    Int32 v = 10;

    char *json = ecs_ptr_to_json(world, ecs_id(Int32), &v);
    test_assert(json != NULL);
    test_str(json, "10");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void OpaqueTypes_ser_string_type_to_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);

    ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = ecs_primitive(world, { .kind = EcsString }),
        .type.serialize = String_serialize
    });

    String v = "Hello World";

    char *json = ecs_ptr_to_json(world, ecs_id(String), &v);
    test_assert(json != NULL);
    test_str(json, "\"Hello World\"");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void OpaqueTypes_ser_vec_i32_type_to_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.serialize = IntVec_serialize
    });

    IntVec v = {3, (int[]){1, 2, 3}};

    char *json = ecs_ptr_to_json(world, ecs_id(IntVec), &v);
    test_assert(json != NULL);
    test_str(json, "[1, 2, 3]");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void OpaqueTypes_ser_vec_string_type_to_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringVec);

    ecs_opaque(world, {
        .entity = ecs_id(StringVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_string_t) }),
        .type.serialize = StringVec_serialize
    });

    StringVec v = {2, (String[]){"Hello", "World"}};

    char *json = ecs_ptr_to_json(world, ecs_id(StringVec), &v);
    test_assert(json != NULL);
    test_str(json, "[\"Hello\", \"World\"]");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

typedef struct Struct_1_member {
    int32_t x;
} Struct_1_member;

typedef struct Struct_2_member {
    int32_t x, y;
} Struct_2_member;

typedef struct Struct_3_member {
    int32_t x, y, z;
} Struct_3_member;

int Struct_1_member_serialize(const ecs_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);

    const Struct_1_member *data = ptr;
    int result;

    result = ser->member(ser, "x");
    test_assert(result == 0);
    result = ser->value(ser, ecs_id(ecs_i32_t), &data->x);
    test_assert(result == 0);

    serialize_invoked ++;
    return 0;
}

int Struct_2_member_serialize(const ecs_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);

    const Struct_2_member *data = ptr;
    int result;

    result = ser->member(ser, "x");
    test_assert(result == 0);
    result = ser->value(ser, ecs_id(ecs_i32_t), &data->x);
    test_assert(result == 0);

    result = ser->member(ser, "y");
    test_assert(result == 0);
    result = ser->value(ser, ecs_id(ecs_i32_t), &data->y);
    test_assert(result == 0);

    serialize_invoked ++;
    return 0;
}

int Struct_3_member_serialize(const ecs_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);

    const Struct_3_member *data = ptr;
    int result;

    result = ser->member(ser, "x");
    test_assert(result == 0);
    result = ser->value(ser, ecs_id(ecs_i32_t), &data->x);
    test_assert(result == 0);

    result = ser->member(ser, "y");
    test_assert(result == 0);
    result = ser->value(ser, ecs_id(ecs_i32_t), &data->y);
    test_assert(result == 0);

    result = ser->member(ser, "z");
    test_assert(result == 0);
    result = ser->value(ser, ecs_id(ecs_i32_t), &data->z);
    test_assert(result == 0);

    serialize_invoked ++;
    return 0;
}

void OpaqueTypes_ser_struct_1_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Struct_1_member);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(Struct_1_member),
        .type.as_type = s,
        .type.serialize = Struct_1_member_serialize
    });

    Struct_1_member v = { 1 };

    char *json = ecs_ptr_to_json(world, ecs_id(Struct_1_member), &v);
    test_assert(json != NULL);
    test_str(json, "{\"x\":1}");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void OpaqueTypes_ser_struct_2_members(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Struct_2_member);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(Struct_2_member),
        .type.as_type = s,
        .type.serialize = Struct_2_member_serialize
    });

    Struct_2_member v = { 1, 2 };

    char *json = ecs_ptr_to_json(world, ecs_id(Struct_2_member), &v);
    test_assert(json != NULL);
    test_str(json, "{\"x\":1, \"y\":2}");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void OpaqueTypes_ser_struct_3_members(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Struct_3_member);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(Struct_3_member),
        .type.as_type = s,
        .type.serialize = Struct_3_member_serialize
    });

    Struct_3_member v = { 1, 2, 3 };

    char *json = ecs_ptr_to_json(world, ecs_id(Struct_3_member), &v);
    test_assert(json != NULL);
    test_str(json, "{\"x\":1, \"y\":2, \"z\":3}");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

#define OpaqueType(t)\
    typedef struct { \
        t value; \
    } Opaque_##t; \
    \
    static void t##_set(void *ptr, t value) { \
        ((Opaque_##t*)ptr)->value = value; \
    }

OpaqueType(bool)
OpaqueType(char)
OpaqueType(int64_t)
OpaqueType(uint64_t)
OpaqueType(double)

typedef struct {
    ecs_entity_t value;
} Opaque_entity;

static void Opaque_entity_set(void *ptr, ecs_world_t *world, ecs_entity_t value) {
    ((Opaque_entity*)ptr)->value = value;
}

typedef struct {
    char *value;
} Opaque_string;

static void Opaque_string_set(void *ptr, const char *value) {
    ((Opaque_string*)ptr)->value = ecs_os_strdup(value);
}

void OpaqueTypes_deser_bool_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_bool);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_bool),
        .type.as_type = ecs_id(ecs_bool_t),
        .type.assign_bool = bool_set
    });

    Opaque_bool v = { false };
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_bool), &v, "true", NULL);
        test_str(r, "");
        test_bool(v.value, true);
    }
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_bool), &v, "false", NULL);
        test_str(r, "");
        test_bool(v.value, false);
    }

    ecs_fini(world);
}

void OpaqueTypes_deser_char_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_char);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_char),
        .type.as_type = ecs_id(ecs_char_t),
        .type.assign_char = char_set
    });

    Opaque_char v = { 0 };
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_char), &v, "\"a\"", NULL);
        test_str(r, "");
        test_int(v.value, 'a');
    }
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_char), &v, "\"b\"", NULL);
        test_str(r, "");
        test_int(v.value, 'b');
    }

    ecs_fini(world);
}

void OpaqueTypes_deser_int_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_int64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_int64_t),
        .type.as_type = ecs_id(ecs_i64_t),
        .type.assign_int = int64_t_set
    });

    Opaque_int64_t v = { 0 };
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_int64_t), &v, "10", NULL);
        test_str(r, "");
        test_int(v.value, 10);
    }
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_int64_t), &v, "20", NULL);
        test_str(r, "");
        test_int(v.value, 20);
    }

    ecs_fini(world);
}

void OpaqueTypes_deser_uint_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_uint64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_uint64_t),
        .type.as_type = ecs_id(ecs_u64_t),
        .type.assign_uint = uint64_t_set
    });

    Opaque_uint64_t v = { 0 };
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_uint64_t), &v, "10", NULL);
        test_str(r, "");
        test_int(v.value, 10);
    }
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_uint64_t), &v, "20", NULL);
        test_str(r, "");
        test_int(v.value, 20);
    }

    ecs_fini(world);
}

void OpaqueTypes_deser_float_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_double);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_double),
        .type.as_type = ecs_id(ecs_f64_t),
        .type.assign_float = double_set
    });

    Opaque_double v = { 0 };
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_double), &v, "10.5", NULL);
        test_str(r, "");
        test_int(v.value, 10); // avoid floating point comparison
    }
    {
        const char *r = ecs_ptr_from_json(world, ecs_id(Opaque_double), &v, "20.5", NULL);
        test_str(r, "");
        test_int(v.value, 20); // avoid floating point comparison
    }

    ecs_fini(world);
}

void OpaqueTypes_deser_string_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_string);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_string),
        .type.as_type = ecs_id(ecs_string_t),
        .type.assign_string = Opaque_string_set
    });

    Opaque_string v = { 0 };
    {
        const char *r = ecs_ptr_from_json(
            world, ecs_id(Opaque_string), &v, "\"Hello World\"", NULL);
        test_str(r, "");
        test_str(v.value, "Hello World");
        ecs_os_free(v.value);
    }
    {
        const char *r = ecs_ptr_from_json(
            world, ecs_id(Opaque_string), &v, "\"Foo Bar\"", NULL);
        test_str(r, "");
        test_str(v.value, "Foo Bar");
        ecs_os_free(v.value);
    }

    ecs_fini(world);
}

void OpaqueTypes_deser_entity_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_entity);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_entity),
        .type.as_type = ecs_id(ecs_string_t),
        .type.assign_entity = Opaque_entity_set
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    Opaque_entity v = { 0 };
    {
        const char *r = ecs_ptr_from_json(
            world, ecs_id(Opaque_entity), &v, "\"e1\"", NULL);
        test_str(r, "");
        test_uint(v.value, e1);
    }
    {
        const char *r = ecs_ptr_from_json(
            world, ecs_id(Opaque_entity), &v, "\"e2\"", NULL);
        test_str(r, "");
        test_uint(v.value, e2);
    }

    ecs_fini(world);
}

void OpaqueTypes_ser_deser_world_w_ser_opaque(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Int32);

    ecs_opaque(world, {
        .entity = ecs_id(Int32),
        .type.as_type = ecs_id(ecs_i32_t),
        .type.serialize = Int32_serialize
    });

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);
    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    {
        Int32 v = { 10 };
        char *json = ecs_ptr_to_json(world, ecs_id(Int32), &v);
        test_assert(json != NULL);
        test_str(json, "10");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

typedef struct {
    ecs_entity_t entity;
} Entity;

int Entity_serialize(const ecs_serializer_t *ser, const void *ptr) {
    const Entity *data = ptr;
    return ser->value(ser, ecs_id(ecs_entity_t), &data->entity);
}

void Entity_assign(void *ptr, ecs_world_t *world, ecs_entity_t value) {
    Entity *data = ptr;
    data->entity = value;
}

void OpaqueTypes_ser_deser_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Entity);

    ecs_opaque(world, {
        .entity = ecs_id(Entity),
        .type.as_type = ecs_id(ecs_entity_t),
        .type.serialize = Entity_serialize,
        .type.assign_entity = Entity_assign
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "ent2" });

    Entity v = { e1 };
    char *json = ecs_ptr_to_json(world, ecs_id(Entity), &v);
    test_assert(json != NULL);
    test_str(json, "\"ent1\"");

    const char *r = ecs_ptr_from_json(world, ecs_id(Entity), &v, "\"ent2\"", NULL);
    test_str(r, "");
    test_assert(v.entity == e2);
    ecs_os_free(json);

    ecs_fini(world);
}

void OpaqueTypes_ser_deser_0_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Entity);

    ecs_opaque(world, {
        .entity = ecs_id(Entity),
        .type.as_type = ecs_id(ecs_entity_t),
        .type.serialize = Entity_serialize,
        .type.assign_entity = Entity_assign
    });

    Entity v = { 0 };
    char *json = ecs_ptr_to_json(world, ecs_id(Entity), &v);
    test_assert(json != NULL);
    test_str(json, "\"#0\"");

    const char *r = ecs_ptr_from_json(world, ecs_id(Entity), &v, json, NULL);
    test_str(r, "");
    test_assert(v.entity == 0);
    ecs_os_free(json);

    ecs_fini(world);
}

void OpaqueTypes_deser_struct_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"x", .type = ecs_id(ecs_i32_t)},
                {"y", .type = ecs_id(ecs_i32_t)}
            }
        }),
        .type.ensure_member = OpaqueStruct_member
    });

    OpaqueStruct v = { 0 };

    const char *r = ecs_ptr_from_json(
        world, ecs_id(OpaqueStruct), &v, "{\"x\": 10, \"y\": 20}", NULL);
    test_str(r, "");
    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_fini(world);
}

void OpaqueTypes_deser_struct_w_opaque_struct_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);
    ECS_COMPONENT(world, Struct_w_OpaqueStruct);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"x", .type = ecs_id(ecs_i32_t)},
                {"y", .type = ecs_id(ecs_i32_t)}
            }
        }),
        .type.ensure_member = OpaqueStruct_member
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_OpaqueStruct),
        .members = {
            {"foo", ecs_id(OpaqueStruct)},
            {"bar", ecs_id(OpaqueStruct)},
        }
    });

    Struct_w_OpaqueStruct v = { 0 };

    const char *r = ecs_ptr_from_json(world, ecs_id(Struct_w_OpaqueStruct), &v, 
        "{\"foo\": {\"x\": 10, \"y\": 20}, \"bar\": {\"x\": 30, \"y\": 40}}", 
            NULL);
    test_str(r, "");
    test_int(v.foo.x, 10);
    test_int(v.foo.y, 20);
    test_int(v.bar.x, 30);
    test_int(v.bar.y, 40);

    ecs_fini(world);
}

void OpaqueTypes_deser_struct_w_opaque_array_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntArray);
    ECS_COMPONENT(world, Struct_w_IntArray);

    ecs_opaque(world, {
        .entity = ecs_id(IntArray),
        .type.as_type = ecs_array(world, { .count = 3, .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntArray_ensure
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_IntArray),
        .members = {
            {"foo", ecs_id(IntArray)},
            {"bar", ecs_id(IntArray)},
        }
    });

    Struct_w_IntArray v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(Struct_w_IntArray), &v, 
        "{\"foo\": [10, 20, 30], \"bar\": [40, 50, 60]}", 
            NULL);
    test_str(r, "");

    test_int(v.foo.array[0], 10);
    test_int(v.foo.array[1], 20);
    test_int(v.foo.array[2], 30);

    test_int(v.bar.array[0], 40);
    test_int(v.bar.array[1], 50);
    test_int(v.bar.array[2], 60);

    ecs_fini(world);
}

void OpaqueTypes_deser_struct_w_opaque_vector_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);
    ECS_COMPONENT(world, Struct_w_IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_IntVec),
        .members = {
            {"foo", ecs_id(IntVec)},
            {"bar", ecs_id(IntVec)},
        }
    });

    Struct_w_IntVec v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(Struct_w_IntVec), &v, 
        "{\"foo\": [10, 20, 30], \"bar\": [40, 50, 60]}", 
            NULL);
    test_str(r, "");

    test_int(v.foo.count, 3);
    test_int(v.foo.array[0], 10);
    test_int(v.foo.array[1], 20);
    test_int(v.foo.array[2], 30);

    test_int(v.bar.count, 3);
    test_int(v.bar.array[0], 40);
    test_int(v.bar.array[1], 50);
    test_int(v.bar.array[2], 60);

    ecs_os_free(v.foo.array);
    ecs_os_free(v.bar.array);

    ecs_fini(world);
}

void OpaqueTypes_deser_array_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntArray);

    ecs_opaque(world, {
        .entity = ecs_id(IntArray),
        .type.as_type = ecs_array(world, { .count = 3, .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntArray_ensure
    });
    
    IntArray v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(IntArray), &v, 
        "[10, 20, 30]", NULL);
    test_str(r, "");

    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);

    ecs_fini(world);
}

void OpaqueTypes_deser_vector_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(IntVec), &v, 
        "[10, 20, 30]", NULL);
    test_str(r, "");

    test_int(v.count, 3);
    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);

    ecs_os_free(v.array);

    ecs_fini(world);
}

void OpaqueTypes_deser_0_vector_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(IntVec), &v, 
        "[]", NULL);
    test_str(r, "");

    test_int(v.count, 0);
    test_assert(v.array == NULL);

    ecs_fini(world);
}

void OpaqueTypes_deser_0_into_vector_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(IntVec), &v, 
        "[10, 20, 30]", NULL);
    test_str(r, "");

    test_int(v.count, 3);
    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);

    r = ecs_ptr_from_json(world, ecs_id(IntVec), &v, "[]", NULL);
    test_str(r, "");

    test_int(v.count, 0);
    test_assert(v.array == NULL);

    ecs_fini(world);
}

void OpaqueTypes_deser_string_vector_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringVec);

    ecs_opaque(world, {
        .entity = ecs_id(StringVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_string_t) }),
        .type.ensure_element = StringVec_ensure,
        .type.count = StringVec_count,
        .type.resize = StringVec_resize
    });

    StringVec v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(StringVec), &v, 
        "[\"Hello\", \"World\", \"Foo\"]", NULL);
    test_str(r, "");

    test_int(v.count, 3);
    test_str(v.array[0], "Hello");
    test_str(v.array[1], "World");
    test_str(v.array[2], "Foo");

    ecs_os_free(v.array[0]);
    ecs_os_free(v.array[1]);
    ecs_os_free(v.array[2]);
    ecs_os_free(v.array);

    ecs_fini(world);
}

void OpaqueTypes_deser_struct_vector_from_json(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT(world, StructVec);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) },
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(StructVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(Position) }),
        .type.ensure_element = StructVec_ensure,
        .type.count = StructVec_count,
        .type.resize = StructVec_resize
    });

    StructVec v = {0};
    const char *r = ecs_ptr_from_json(world, ecs_id(StructVec), &v, 
        "[{\"x\": 10, \"y\": 20}, {\"y\": 30, \"x\": 40}]", NULL);
    test_str(r, "");

    test_int(v.count, 2);
    test_int(v.array[0].x, 10);
    test_int(v.array[0].y, 20);
    test_int(v.array[1].x, 40);
    test_int(v.array[1].y, 30);

    ecs_os_free(v.array);

    ecs_fini(world);
}

void OpaqueTypes_const_string(void) {
    typedef struct TestStringStruct {
        const char *value;
    } TestStringStruct;

    const char *test_string = "Const String. Don't try to free me!";

    ecs_world_t *world = ecs_init();

    // Lookup const string type:
    ecs_entity_t const_string = ecs_lookup(world, "flecs.core.const_string_t");

    // Create a reflection interface for `test_struct`:
    ecs_entity_t test_struct = ecs_struct(world, {
        .entity = ecs_entity(world, {
            .name = "test_struct", .root_sep = ""}),
            .members = {{
                .name = "value",
                .type = const_string 
            }}
        });

    ecs_entity_t e = ecs_entity(world, {.name = "MyEntity"});
    TestStringStruct *st = ecs_ensure_id(world, e, test_struct, sizeof(char*));
    st->value = test_string;

    // we should be able to retrieve the string with a cursor:
    ecs_meta_cursor_t cursor = ecs_meta_cursor(world, test_struct, st);
    ecs_meta_push(&cursor);
    ecs_meta_member(&cursor, "value");

    const char *retrieved_string = ecs_meta_get_string(&cursor);

    test_assert(!ecs_os_strcmp(retrieved_string, test_string));

    // When deleting the entity, Flecs must not try to free the above const 
    // string, otherwise the below will segfault:
    ecs_delete(world, e);

    test_assert(true);  // If we get to this point without a crash, we're good.

    ecs_fini(world);
}

void OpaqueTypes_anonymous_opaque_as_type_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);

    ecs_entity_t as_type = ecs_primitive(world, { .kind = EcsString });

    ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = as_type,
        .type.serialize = String_serialize
    });

    test_assert(ecs_get_parent(world, as_type) == ecs_id(String));

    ecs_fini(world);
}

void OpaqueTypes_named_opaque_as_type_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);

    ecs_entity_t as_type = ecs_primitive(world, { 
        .entity = ecs_entity(world, { .name = "Foo" }),
        .kind = EcsString 
    });

    ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = as_type,
        .type.serialize = String_serialize
    });

    test_assert(ecs_get_parent(world, as_type) == 0);

    ecs_fini(world);
}

void OpaqueTypes_parented_opaque_as_type_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);

    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });

    ecs_entity_t as_type = ecs_primitive(world, { 
        .entity = ecs_entity(world, { 
            .parent = foo 
        }),
        .kind = EcsString 
    });

    ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = as_type,
        .type.serialize = String_serialize
    });

    test_assert(ecs_get_parent(world, as_type) == foo);

    ecs_fini(world);
}
