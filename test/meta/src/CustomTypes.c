#include <meta.h>

static int serialize_invoked = 0;

typedef int32_t Int32;
typedef char* String;

typedef struct IntVec {
    int32_t count;
    Int32 *elems;
} IntVec;

typedef struct StringVec {
    int32_t count;
    String *elems;
} StringVec;

// Custom serializer functions
static
int Int32_serialize(const ecs_meta_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);
    int result = ser->value(ser, ecs_id(ecs_i32_t), ptr);
    test_assert(result == 0);
    serialize_invoked ++;
    return result;
}

static
int String_serialize(const ecs_meta_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);
    int result = ser->value(ser, ecs_id(ecs_string_t), ptr);
    test_assert(result == 0);
    serialize_invoked ++;
    return result;
}

int IntVec_serialize(const ecs_meta_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);

    const IntVec *data = ptr;
    for (int i = 0; i < data->count; i ++) {
        int result = ser->value(ser, ecs_id(ecs_i32_t), &data->elems[i]);
        test_assert(result == 0);
    }

    serialize_invoked ++;
    return 0;
}

int StringVec_serialize(const ecs_meta_serializer_t *ser, const void *ptr) {
    test_assert(ser != NULL);
    test_assert(ptr != NULL);

    const StringVec *data = ptr;
    for (int i = 0; i < data->count; i ++) {
        int result = ser->value(ser, ecs_id(ecs_string_t), &data->elems[i]);
        test_assert(result == 0);
    }

    serialize_invoked ++;
    return 0;
}

void CustomTypes_custom_i32_type_to_json() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Int32);

    ecs_custom_type(world, {
        .entity = ecs_id(Int32),
        .as_type = ecs_primitive(world, { .kind = EcsI32 }),
        .serialize = Int32_serialize
    });

    Int32 v = 10;

    char *json = ecs_ptr_to_json(world, ecs_id(Int32), &v);
    test_assert(json != NULL);
    test_str(json, "10");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void CustomTypes_custom_string_type_to_json() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);

    ecs_custom_type(world, {
        .entity = ecs_id(String),
        .as_type = ecs_primitive(world, { .kind = EcsString }),
        .serialize = String_serialize
    });

    String v = "Hello World";

    char *json = ecs_ptr_to_json(world, ecs_id(String), &v);
    test_assert(json != NULL);
    test_str(json, "\"Hello World\"");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void CustomTypes_custom_vec_i32_type_to_json() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_custom_type(world, {
        .entity = ecs_id(IntVec),
        .as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .serialize = IntVec_serialize
    });

    IntVec v = {3, (int[]){1, 2, 3}};

    char *json = ecs_ptr_to_json(world, ecs_id(IntVec), &v);
    test_assert(json != NULL);
    test_str(json, "[1, 2, 3]");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}

void CustomTypes_custom_vec_string_type_to_json() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringVec);

    ecs_custom_type(world, {
        .entity = ecs_id(StringVec),
        .as_type = ecs_vector(world, { .type = ecs_id(ecs_string_t) }),
        .serialize = StringVec_serialize
    });

    StringVec v = {2, (String[]){"Hello", "World"}};

    char *json = ecs_ptr_to_json(world, ecs_id(StringVec), &v);
    test_assert(json != NULL);
    test_str(json, "[\"Hello\", \"World\"]");
    ecs_os_free(json);

    test_int(serialize_invoked, 1);

    ecs_fini(world);
}
