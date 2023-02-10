#include <ser_opaque_vector.h>
#include <stdio.h>

// Simple integer vector type
typedef struct {
    size_t count;
    int32_t *array;
} IntVector;

// Reflection support for IntVector
int IntVector_serialize(const ecs_serializer_t *ser, const void *ptr) {
    const IntVector *data = ptr;
    for (size_t i = 0; i < data->count; i ++) {
        ser->value(ser, ecs_id(ecs_i32_t), &data->array[i]);
    }
    return 0;
}

size_t IntVector_count(const void *ptr) {
    const IntVector *data = ptr;
    return data->count;
}

static void IntVector_resize(void *ptr, size_t size) {
    IntVector *data = ptr;
    if (data->count != size) {
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, int32_t, (ecs_size_t)size);
        }
    }
}

static void* IntVector_ensure_element(void *ptr, size_t index) {
    IntVector *data = ptr;
    if (data->count <= index) {
        IntVector_resize(ptr, index + 1);
    }
    return &data->array[index];
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register type as component so the size is known
    ECS_COMPONENT(ecs, IntVector);

    // Register IntVector as opaque type. An opaque type is a type of which the
    // reflection framework doesn't know the layout, but which it can serialize.
    ecs_opaque(ecs, {
        // Link reflection with component
        .entity = ecs_id(IntVector),
        .type = {
            // Let reflection framework know the type represents a vector<i32>
            .as_type = ecs_vector(ecs, { .type = ecs_id(ecs_i32_t) }),
            // Register functions for de/serializing vector contents
            .serialize = IntVector_serialize,
            .count = IntVector_count,
            .resize = IntVector_resize,
            .ensure_element = IntVector_ensure_element
        }
    });

    IntVector v = {0};

    // Deserialize new value into vector
    ecs_ptr_from_json(ecs, ecs_id(IntVector), &v, "[10, 20, 30]", NULL);

    // Print vector contents
    printf("[");
    for (size_t i = 0; i < v.count; i ++) {
        if (i) printf(", ");
        printf("%d", v.array[i]);
    }
    printf("]\n");

    // Serialize vector contents to JSON
    char *json = ecs_ptr_to_json(ecs, ecs_id(IntVector), &v);
    printf("%s\n", json);
    ecs_os_free(json);

    // Free array of vector. If this type were used as a component, we could
    // register a dtor to do this for us (see the entity/hooks example).
    ecs_os_free(v.array);

    ecs_fini(ecs);

    // Output
    //   [10, 20, 30]
    //   [10, 20, 30]
}
