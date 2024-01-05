#include <ser_opaque_type.h>
#include <stdio.h>

// Use opaque reflection support to add a computed 'result' member to type
typedef struct {
    int32_t a;
    int32_t b;
} Sum;

// Reflection support for Sum
int Sum_serialize(const ecs_serializer_t *ser, const void *ptr) {
    const Sum *data = ptr;
    ser->member(ser, "a");
    ser->value(ser, ecs_id(ecs_i32_t), &data->a);
    ser->member(ser, "b");
    ser->value(ser, ecs_id(ecs_i32_t), &data->b);

    int32_t result = data->a + data->b; // Serialize fake member
    ser->member(ser, "result");
    ser->value(ser, ecs_id(ecs_i32_t), &result);
    return 0;
}

// Return address for requested member
void* Sum_ensure_member(void *ptr, const char *member) {
    Sum *data = ptr;
    if (!strcmp(member, "a")) {
        return &data->a;
    } else if (!strcmp(member, "b")) {
        return &data->b;
    } else {
        return NULL; // We can't serialize into fake result member
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register type as component so the size is known
    ECS_COMPONENT(ecs, Sum);

    // Create struct type that describes the structure of Sum
    ecs_entity_t sum_type_descriptor = ecs_struct(ecs, {
        .members = {
            { .name = "a", .type = ecs_id(ecs_i32_t) },
            { .name = "b", .type = ecs_id(ecs_i32_t) },
            { .name = "result", .type = ecs_id(ecs_i32_t) }
        }
    });

    // Register Sum as opaque type. An opaque type is a type of which the
    // reflection framework doesn't know the layout, but which it can serialize.
    ecs_opaque(ecs, {
        // Link reflection with component
        .entity = ecs_id(Sum),
        // Let reflection framework know the type represents a struct
        .type.as_type = sum_type_descriptor,
        // Register function for serializing sum contents
        .type.serialize = Sum_serialize,
        // Register function for getting access to members
        .type.ensure_member = Sum_ensure_member
    });

    Sum v = { .a = 10, .b = 20 };

    // Serialize value of Sum to JSON
    char *json = ecs_ptr_to_json(ecs, ecs_id(Sum), &v);
    printf("%s\n", json);
    ecs_os_free(json);

    // Deserialize new value into Sum
    ecs_ptr_from_json(ecs, ecs_id(Sum), &v, "{\"a\": 20, \"b\": 22}", NULL);

    // Serialize value again
    json = ecs_ptr_to_json(ecs, ecs_id(Sum), &v);
    printf("%s\n", json);
    ecs_os_free(json);

    // Output
    //  {"a":10, "b":20, "result":30}
    //  {"a":22, "b":20, "result":42}

    ecs_fini(ecs);
}
