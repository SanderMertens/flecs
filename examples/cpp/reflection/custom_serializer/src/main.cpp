#include <custom_serializer.h>
#include <iostream>
#include <vector>
#include <inttypes.h>

// This example demonstrates how to create a serializer for a custom format.

static int indent = 0;

#define printIndented(...)\
    printf("\n%*s", indent * 2, ""); printf(__VA_ARGS__)

class Serializer {
public:
    template <typename T>
    static void serialize(flecs::world& ecs, const T& value) {
        serialize(ecs, ecs.component<T>(), &value);
    }

    static void serialize(
        flecs::world& ecs, flecs::entity type, const void *ptr) 
    {
        // The TypeSerializer component contains a vector of instructions that 
        // tells us how to serialize the type.
        const flecs::TypeSerializer& ts = type.get<flecs::TypeSerializer>();

        Serializer{ecs}.serializeOps(
            ecs_vec_first_t(&ts.ops, flecs::meta::op_t), 
            ecs_vec_count(&ts.ops), ptr);
    }

private:
    Serializer(flecs::world& w) : ecs(w) { }

    // Serialize contents of struct or collection scope
    void serializeScope(flecs::meta::op_t *ops, const void *ptr) {
        // A scope starts with a Push and ends with a Pop, so trim the first and 
        // last instruction before forwarding.
        serializeOps(ops + 1, ops->op_count - 2, ptr);
    }

    // Serialize a struct scope
    void serializeStruct(flecs::meta::op_t *ops, 
        const void *ptr) 
    {
        printf("{");
        indent ++;

        serializeScope(ops, ptr);

        indent --;
        printIndented("}");
    }

    // Serialize an array scope
    void serializeArray(flecs::meta::op_t *ops, 
        int32_t elem_count, const void *ptr) 
    {
        printf("[");

        // Iterate elements of the array. Skip the first and last instruction 
        // since they are PushStruct and Pop.
        for (int i = 0; i < elem_count; i ++) {
            if (i) {
                printf(", ");
            }

            serializeScope(ops, ptr);

            ptr = ECS_OFFSET(ptr, ops->elem_size);
        }

        printf("]");
    }

    // Serialize a vector scope
    void serializeVector(flecs::meta::op_t *ops, 
        const void *ptr) 
    {
        auto vec = static_cast<const ecs_vec_t*>(ptr);
        serializeArray(ops, vec->count, vec->array);
    }

    // Serialize enum
    void serializeEnum(flecs::meta::op_t *op, const void *ptr) 
    {
        ecs_meta_op_kind_t kind = op->underlying_kind;
        ecs_map_key_t value;

        if (kind == EcsOpU8 || kind == EcsOpI8) {
            value = *static_cast<const uint8_t*>(ptr);
        } else if (kind == EcsOpU16 || kind == EcsOpI16) {
            value = *static_cast<const uint16_t*>(ptr);
        } else if (kind == EcsOpU32 || kind == EcsOpI32) {
            value = *static_cast<const uint32_t*>(ptr);
        } else if (kind == EcsOpUPtr || kind == EcsOpIPtr) {
            value = *static_cast<const uintptr_t*>(ptr);
        } else if (kind == EcsOpU64 || kind == EcsOpI64) {
            value = *static_cast<const uint64_t*>(ptr);
        } else {
            printf("<<invalid underlying enum type>>");
            return;
        }

        ecs_enum_constant_t *c = ecs_map_get_deref(op->is.constants, 
            ecs_enum_constant_t, value);
        printf("%s", c->name);
    }

    // Iterate over instruction array
    void serializeOps(flecs::meta::op_t *ops, int32_t op_count, 
        const void *base) 
    {
        for (int i = 0; i < op_count; i ++) {
            flecs::meta::op_t *op = &ops[i];

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
            // Instructions that forward to a type scope, like a (nested) struct 
            // or collection
            case EcsOpPushStruct:
                serializeStruct(op, ptr);
                break;
            case EcsOpPushArray:
                serializeArray(op, ecs_meta_op_get_elem_count(ops, ptr), ptr);
                break;
            case EcsOpPushVector:
                serializeVector(op, ptr);
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
                serialize(ecs, flecs::entity(ecs, op->type), ptr);
                break;

            // Serialize single values
            case EcsOpEnum:
                serializeEnum(op, ptr);
                break;
            case EcsOpBitmask:
                // Bitmask serialization requires iterating all the bits in a 
                // value and looking up the corresponding constant. For 
                // an example, see src/addons/script/serialize.c.
                break;
            case EcsOpBool:
                printf("%s", 
                    (*static_cast<const bool*>(ptr)) ? "true" : "false");
                break;
            case EcsOpChar:
                printf("'%c'", *static_cast<const char*>(ptr));
                break;
            case EcsOpByte:
            case EcsOpU8:
                printf("%" PRIu8, *static_cast<const uint8_t*>(ptr));
                break;
            case EcsOpU16:
                printf("%" PRIu16, *static_cast<const uint16_t*>(ptr));
                break;
            case EcsOpU32:
                printf("%" PRIu32, *static_cast<const uint32_t*>(ptr));
                break;
            case EcsOpU64:
                printf("%" PRIu64, *static_cast<const uint64_t*>(ptr));
                break;
            case EcsOpI8:
                printf("%" PRIi8, *static_cast<const int8_t*>(ptr));
                break;
            case EcsOpI16:
                printf("%" PRIi16, *static_cast<const int16_t*>(ptr));
                break;
            case EcsOpI32:
                printf("%" PRIi32, *static_cast<const int32_t*>(ptr));
                break;
            case EcsOpI64:
                printf("%" PRIi64, *static_cast<const int64_t*>(ptr));
                break;
            case EcsOpF32:
                printf("%.2f", 
                    static_cast<double>(*static_cast<const float*>(ptr)));
                break;
            case EcsOpF64:
                printf("%.2f", *static_cast<const double*>(ptr));
                break;
            case EcsOpUPtr:
                printf("%" PRIuPTR, *static_cast<const uintptr_t*>(ptr));
                break;
            case EcsOpIPtr:
                printf("%" PRIiPTR, *static_cast<const intptr_t*>(ptr));
                break;
            case EcsOpString:
                printf("\"%s\"", *static_cast<const char**>(ptr));
                break;
            case EcsOpEntity: {
                flecs::entity e(ecs, *static_cast<flecs::entity_t*>(ptr));
                printf("%s\n", e.path().c_str());
                break;
            }
            case EcsOpId: {
                flecs::id component(ecs, *static_cast<flecs::id_t*>(ptr));
                printf("%s\n", component.str().c_str());
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

    flecs::world& ecs;
};

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

int main(int, char *[]) {
    flecs::world ecs;

    // We need to register reflection data for our types before we can use them
    // with the serializer code.

    ecs.component<Rgb>()
        .member("red", &Rgb::red)
        .member("green", &Rgb::green)
        .member("blue", &Rgb::blue);

    ecs.component<StrokeKind>();

    ecs.component<Point>()
        .member("x", &Point::x)
        .member("y", &Point::y);

    ecs.component<Polygon>()
        .member("color", &Polygon::color)
        .member("stroke_color", &Polygon::stroke_color)
        .member("stroke_kind", &Polygon::stroke_kind)
        .member(ecs.vector<Point>(), "points");

    // Create the value
    Polygon p = {};
    p.color = {0, 0, 255};
    p.stroke_color = {255, 0, 0};
    p.stroke_kind = Dashed;
    ecs_vec_init_t(NULL, &p.points, Point, 0);
    *ecs_vec_append_t(NULL, &p.points, Point) = {0, 0};
    *ecs_vec_append_t(NULL, &p.points, Point) = {1, 1};
    *ecs_vec_append_t(NULL, &p.points, Point) = {-1, 1};


    // Serialize value
    Serializer::serialize(ecs, p);

    printf("\n");

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
