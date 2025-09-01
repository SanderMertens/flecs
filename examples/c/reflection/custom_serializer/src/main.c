#include <custom_serializer.h>
#include <stdio.h>
#include <inttypes.h>

// This example demonstrates how to create a serializer for a custom format.

static int indent = 0;

#define printIndented(...)\
    printf("\n%*s", indent * 2, ""); printf(__VA_ARGS__)

void serializeOps(ecs_world_t *world, ecs_meta_op_t *ops, 
    int32_t op_count, const void *ptr);

// Serialize a pointer of a specified type.
int serialize(ecs_world_t *world, ecs_entity_t type, const void *ptr) {
    // The TypeSerializer component contains a vector of instructions that tells
    // us how to serialize the type.
    const EcsTypeSerializer *s = ecs_get(world, type, EcsTypeSerializer);
    if (!s) {
        printf("type does not have reflection data\n");
        return -1;
    }

    serializeOps(world, ecs_vec_first(&s->ops), ecs_vec_count(&s->ops), ptr);

    return 0;
}

// Serialize contents of struct or collection scope
void serializeScope(ecs_world_t *world, ecs_meta_op_t *ops, const void *ptr) {
    // A scope starts with a Push and ends with a Pop, so trim the first and 
    // last instruction before forwarding.
    serializeOps(world, ops + 1, ops->op_count - 2, ptr);
}

// Serialize a struct scope
void serializeStruct(ecs_world_t *world, ecs_meta_op_t *ops, 
    const void *ptr) 
{
    printf("{");
    indent ++;

    serializeScope(world, ops, ptr);

    indent --;
    printIndented("}");
}

// Serialize an array scope
void serializeArray(ecs_world_t *world, ecs_meta_op_t *ops, 
    int32_t elem_count, const void *ptr) 
{
    printf("[");

    // Iterate elements of the array. Skip the first and last instruction 
    // since they are PushStruct and Pop.
    for (int i = 0; i < elem_count; i ++) {
        if (i) {
            printf(", ");
        }

        serializeScope(world, ops, ptr);

        ptr = ECS_OFFSET(ptr, ops->elem_size);
    }

    printf("]");
}

// Serialize a vector scope
void serializeVector(ecs_world_t *world, ecs_meta_op_t *ops, 
    const void *ptr) 
{
    const ecs_vec_t *vec = ptr;
    serializeArray(world, ops, vec->count, vec->array);
}

// Serialize enum
void serializeEnum(ecs_meta_op_t *op, const void *ptr) 
{
    ecs_map_key_t value;
    ecs_meta_op_kind_t kind = op->underlying_kind;

    if (kind == EcsOpU8 || kind == EcsOpI8) {
        value = *(const uint8_t*)ptr;
    } else if (kind == EcsOpU16 || kind == EcsOpI16) {
        value = *(const uint16_t*)ptr;
    } else if (kind == EcsOpU32 || kind == EcsOpI32) {
        value = *(const uint32_t*)ptr;
    } else if (kind == EcsOpUPtr || kind == EcsOpIPtr) {
        value = *(const uintptr_t*)ptr;
    } else if (kind == EcsOpU64 || kind == EcsOpI64) {
        value = *(const uint64_t*)ptr;
    } else {
        printf("<<invalid underlying enum type>>");
        return;
    }

    ecs_enum_constant_t *c = ecs_map_get_deref(op->is.constants, 
        ecs_enum_constant_t, value);
    printf("%s", c->name);
}

// Iterate over instruction array
void serializeOps(ecs_world_t *world, ecs_meta_op_t *ops, int32_t op_count, 
    const void *base) 
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_op_t *op = &ops[i];

        // Member name
        if (op->name) {
            if (i) {
                printf(",");
            }

            printIndented("%s: ", op->name);
        }

        // Get pointer for current field
        void *ptr = ECS_OFFSET(base, op->offset);

        switch(op->kind) {
        // Instructions that forward to a type scope, like a (nested) struct or 
        // collection
        case EcsOpPushStruct:
            serializeStruct(world, op, ptr);
            break;
        case EcsOpPushArray:
            serializeArray(world, op, ecs_meta_op_get_elem_count(ops, ptr), ptr);
            break;
        case EcsOpPushVector:
            serializeVector(world, op, ptr);
            break;

        // Opaque types have in-memory representations that are opaque to 
        // the reflection framework and cannot be serialized by just taking
        // a pointer + an offset. See src/addons/script/serialize.c for an
        // example of how to handle opaque types.
        case EcsOpOpaqueStruct:
        case EcsOpOpaqueArray:
        case EcsOpOpaqueVector:
        case EcsOpOpaqueValue:
            break;

        // Forward to type. Used for members of array/vector types.
        case EcsOpForward:
            serialize(world, op->type, ptr);
            break;

        // Serialize single values
        case EcsOpEnum:
            serializeEnum(op, ptr);
            break;
        case EcsOpBitmask:
            // Bitmask serialization requires iterating all the bits in a value
            // and looking up the corresponding constant. For an example, see
            // src/addons/script/serialize.c.
            break;
        case EcsOpBool:
            printf("%s", (*(const bool*)ptr) ? "true" : "false");
            break;
        case EcsOpChar:
            printf("'%c'", *(const char*)ptr);
            break;
        case EcsOpByte:
        case EcsOpU8:
            printf("%" PRIu8, *(const uint8_t*)ptr);
            break;
        case EcsOpU16:
            printf("%" PRIu16, *(const uint16_t*)ptr);
            break;
        case EcsOpU32:
            printf("%" PRIu32, *(const uint32_t*)ptr);
            break;
        case EcsOpU64:
            printf("%" PRIu64, *(const uint64_t*)ptr);
            break;
        case EcsOpI8:
            printf("%" PRIi8, *(const int8_t*)ptr);
            break;
        case EcsOpI16:
            printf("%" PRIi16, *(const int16_t*)ptr);
            break;
        case EcsOpI32:
            printf("%" PRIi32, *(const int32_t*)ptr);
            break;
        case EcsOpI64:
            printf("%" PRIi64, *(const int64_t*)ptr);
            break;
        case EcsOpF32:
            printf("%.2f", (double)*(const float*)ptr);
            break;
        case EcsOpF64:
            printf("%.2f", *(const double*)ptr);
            break;
        case EcsOpUPtr:
            printf("%" PRIuPTR, *(const uintptr_t*)ptr);
            break;
        case EcsOpIPtr:
            printf("%" PRIiPTR, *(const intptr_t*)ptr);
            break;
        case EcsOpString:
            printf("\"%s\"", *(const char**)ptr);
            break;
        case EcsOpEntity: {
            char *name = ecs_get_path(world, *(ecs_entity_t*)ptr);
            printf("%s\n", name);
            ecs_os_free(name);
            break;
        }
        case EcsOpId: {
            char *name = ecs_id_str(world, *(ecs_id_t*)ptr);
            printf("%s\n", name);
            ecs_os_free(name);
            break;
        }
        case EcsOpPop:
        case EcsOpScope:
        case EcsOpPrimitive:
            // Not serializable
            break;
        }

        i += op->op_count - 1; // Skip over already processed instructions
    }
}

// Some types to test the serializer with

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Rgb;

typedef enum {
    Solid, Dashed
} StrokeKind;

typedef struct {
    float x, y;
} Point;

typedef struct {
    Rgb color;
    Rgb stroke_color;
    StrokeKind stroke_kind;
    ecs_vec_t points;
} Polygon;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // We need to register reflection data for our types before we can use them
    // with the serializer code. We'll do it manually here, but for an easier
    // way to insert reflection data see the auto_* examples.

    ECS_COMPONENT(world, Rgb);
    ECS_COMPONENT(world, StrokeKind);
    ECS_COMPONENT(world, Point);
    ECS_COMPONENT(world, Polygon);

    ecs_struct(world, {
        .entity = ecs_id(Rgb),
        .members = {
            { .name = "r", .type = ecs_id(ecs_u8_t) },
            { .name = "g", .type = ecs_id(ecs_u8_t) },
            { .name = "b", .type = ecs_id(ecs_u8_t) },
        }
    });

    ecs_enum(world, {
        .entity = ecs_id(StrokeKind),
        .constants = {
            { .name = "Solid", .value = Solid },
            { .name = "Dashed", .value = Dashed },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_entity_t PointVector = ecs_vector(world, { .type = ecs_id(Point) });

    ecs_struct(world, {
        .entity = ecs_id(Polygon),
        .members = {
            { .name = "color", .type = ecs_id(Rgb) },
            { .name = "stroke_color", .type = ecs_id(Rgb) },
            { .name = "stroke_kind", .type = ecs_id(StrokeKind) },
            { .name = "points", .type = PointVector }
        }
    });

    // Create the value
    ecs_vec_t points;
    ecs_vec_init_t(NULL, &points, Point, 0);
    *ecs_vec_append_t(NULL, &points, Point) = (Point){0, 0};
    *ecs_vec_append_t(NULL, &points, Point) = (Point){1, 1};
    *ecs_vec_append_t(NULL, &points, Point) = (Point){-1, 1};

    Polygon p = {
        .color = {0, 0, 255},
        .stroke_color = {255, 0, 0},
        .stroke_kind = Dashed,
        .points = points
    };

    // Serialize value
    serialize(world, ecs_id(Polygon), &p);
    printf("\n");

    // Free memory
    ecs_vec_fini_t(NULL, &points, Point);
    
    ecs_fini(world);

    // Output:
    // {
    //   color: {
    //     r: 0,
    //     g: 0,
    //     b: 255
    //   },
    //   stroke_color: {
    //     r: 255,
    //     g: 0,
    //     b: 0
    //   },
    //   stroke_kind: Dashed,
    //   points: [{
    //     x: 0.00,
    //     y: 0.00
    //   }, {
    //     x: 1.00,
    //     y: 1.00
    //   }, {
    //     x: -1.00,
    //     y: 1.00
    //   }]
    // }
}
