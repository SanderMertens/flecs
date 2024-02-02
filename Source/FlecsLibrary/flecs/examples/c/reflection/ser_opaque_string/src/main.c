#include <ser_opaque_string.h>
#include <stdio.h>

// Simple string type
typedef struct {
    char *value;
} String;

// Reflection support for String
int String_serialize(const ecs_serializer_t *ser, const void *ptr) {
    const String *data = ptr;
    return ser->value(ser, ecs_id(ecs_string_t), &data->value);
}

static void String_assign_string(void *ptr, const char *value) {
    String *data = ptr;

    if (data->value) {
        ecs_os_free(data->value);
    }

    data->value = ecs_os_strdup(value);
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register type as component so the size is known
    ECS_COMPONENT(ecs, String);

    // Register String as opaque type. An opaque type is a type of which the
    // reflection framework doesn't know the layout, but which it can serialize.
    ecs_opaque(ecs, {
        // Link reflection with component
        .entity = ecs_id(String),
        // Let reflection framework know the type represents a string
        .type.as_type = ecs_id(ecs_string_t),
        // Register functions for de/serializing vector contents
        .type.serialize = String_serialize,
        .type.assign_string = String_assign_string
    });

    String v = {0};

    // Deserialize new value into string
    ecs_ptr_from_json(ecs, ecs_id(String), &v, "\"Hello World\"", NULL);

    // Print string contents
    printf("%s\n", v.value);

    // Serialize string contents to JSON
    char *json = ecs_ptr_to_json(ecs, ecs_id(String), &v);
    printf("%s\n", json);
    ecs_os_free(json);

    // Free string. If this type were used as a component, we could
    // register a dtor to do this for us (see the entity/hooks example).
    ecs_os_free(v.value);

    ecs_fini(ecs);

    // Output
    //   Hello World
    //   "Hello World"
}
