#include <meta.h>

void Serialized_primitive_constants() {
    test_int(EcsBool + EcsOpPrimitive, EcsOpBool);
    test_int(EcsChar + EcsOpPrimitive, EcsOpChar);
    test_int(EcsByte + EcsOpPrimitive, EcsOpByte);
    test_int(EcsU8 + EcsOpPrimitive, EcsOpU8);
    test_int(EcsU16 + EcsOpPrimitive, EcsOpU16);
    test_int(EcsU32 + EcsOpPrimitive, EcsOpU32);
    test_int(EcsU64 + EcsOpPrimitive, EcsOpU64);
    test_int(EcsI8 + EcsOpPrimitive, EcsOpI8);
    test_int(EcsI16 + EcsOpPrimitive, EcsOpI16);
    test_int(EcsI32 + EcsOpPrimitive, EcsOpI32);
    test_int(EcsI64 + EcsOpPrimitive, EcsOpI64);
    test_int(EcsF32 + EcsOpPrimitive, EcsOpF32);
    test_int(EcsF64 + EcsOpPrimitive, EcsOpF64);
    test_int(EcsUPtr + EcsOpPrimitive, EcsOpUPtr);
    test_int(EcsIPtr + EcsOpPrimitive, EcsOpIPtr);
    test_int(EcsString + EcsOpPrimitive, EcsOpString);
    test_int(EcsEntity + EcsOpPrimitive, EcsOpEntity);
}

static ecs_meta_type_op_t* ops_get(ecs_vector_t *ops, int32_t index) {
    ecs_meta_type_op_t* op = ecs_vector_get(ops, ecs_meta_type_op_t, index);
    test_assert(op != NULL);
    return op;
}

static void test_op(
    ecs_vector_t *ops, 
    int32_t index, 
    ecs_meta_type_op_kind_t kind, 
    int32_t count, 
    int32_t op_count, 
    ecs_entity_t type)
{
    ecs_meta_type_op_t *op = ops_get(ops, index);
    test_assert(op != NULL);

    test_int(op->kind, kind);
    test_int(op->count, count);
    test_int(op->op_count, op_count);
    test_str(op->name, NULL);
    test_int(op->type, type);
    test_int(op->offset, 0);
}

static void _test_member_op(
    ecs_vector_t *ops, 
    int32_t index, 
    ecs_meta_type_op_kind_t kind, 
    int32_t count, 
    int32_t op_count, 
    const char *name, 
    ecs_entity_t type,
    ecs_size_t offset)
{
    ecs_meta_type_op_t *op = ops_get(ops, index);
    test_assert(op != NULL);

    test_int(op->kind, kind);
    test_int(op->count, count);
    test_int(op->op_count, op_count);
    test_str(op->name, name);
    test_int(op->type, type);
    test_int(op->offset, offset);
}

#define test_mp(ops, index, kind, count, op_count, T, name, type)\
    _test_member_op(ops, index, kind, count, op_count, #name, type, \
        offsetof(T, name))

#define test_mn(ops, index, kind, count, op_count, offset, T, name, type)\
    _test_member_op(ops, index, kind, count, op_count, #name, type, \
        offset + offsetof(T, name))

void Serialized_ops_bool() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_bool_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsBool, 1, 1, ecs_id(ecs_bool_t));

    ecs_fini(world);
}

void Serialized_ops_byte() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_byte_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsByte, 1, 1, ecs_id(ecs_byte_t));

    ecs_fini(world);
}

void Serialized_ops_char() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_char_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsChar, 1, 1, ecs_id(ecs_char_t));

    ecs_fini(world);
}

void Serialized_ops_i8() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_i8_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsI8, 1, 1, ecs_id(ecs_i8_t));

    ecs_fini(world);
}

void Serialized_ops_i16() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_i16_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsI16, 1, 1, ecs_id(ecs_i16_t));

    ecs_fini(world);
}

void Serialized_ops_i32() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_i32_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsI32, 1, 1, ecs_id(ecs_i32_t));

    ecs_fini(world);
}

void Serialized_ops_i64() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_i64_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsI64, 1, 1, ecs_id(ecs_i64_t));

    ecs_fini(world);
}

void Serialized_ops_iptr() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_iptr_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsIPtr, 1, 1, ecs_id(ecs_iptr_t));

    ecs_fini(world);
}

void Serialized_ops_u8() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_u8_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsU8, 1, 1, ecs_id(ecs_u8_t));

    ecs_fini(world);
}

void Serialized_ops_u16() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_u16_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsU16, 1, 1, ecs_id(ecs_u16_t));

    ecs_fini(world);
}

void Serialized_ops_u32() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_u32_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsU32, 1, 1, ecs_id(ecs_u32_t));

    ecs_fini(world);
}

void Serialized_ops_u64() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_u64_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsU64, 1, 1, ecs_id(ecs_u64_t));

    ecs_fini(world);
}

void Serialized_ops_uptr() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_uptr_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsUPtr, 1, 1, ecs_id(ecs_uptr_t));

    ecs_fini(world);
}

void Serialized_ops_float() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_f32_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsF32, 1, 1, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Serialized_ops_double() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_f64_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsF64, 1, 1, ecs_id(ecs_f64_t));

    ecs_fini(world);
}

void Serialized_ops_string() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_string_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsString, 1, 1, ecs_id(ecs_string_t));

    ecs_fini(world);
}

void Serialized_ops_entity() {
    ecs_world_t *world = ecs_init();

    const EcsMetaTypeSerialized *s = ecs_get(
        world, ecs_id(ecs_entity_t), EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpPrimitive + EcsEntity, 1, 1, ecs_id(ecs_entity_t));

    ecs_fini(world);
}

void Serialized_ops_struct_bool() {
    typedef struct {
        ecs_bool_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_bool_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpBool, 1, 1, T, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_i32() {
    typedef struct {
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpI32, 1, 1, T, x, ecs_id(ecs_i32_t));
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpI32, 1, 1, T, x, ecs_id(ecs_i32_t));
    test_mp(s->ops, 2, EcsOpI32, 1, 1, T, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_i32_bool() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpI32, 1, 1, T, x, ecs_id(ecs_i32_t));
    test_mp(s->ops, 2, EcsOpBool, 1, 1, T, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_bool_i32() {
    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpI32, 1, 1, T, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_bool() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 5);

    test_op(s->ops, 0, EcsOpPush, 1, 5, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 3, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_i32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 5);

    test_op(s->ops, 0, EcsOpPush, 1, 5, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 3, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpI32, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_i32_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_bool_w_bool_member() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
        bool x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 6);

    test_op(s->ops, 0, EcsOpPush, 1, 6, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 3, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 4, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 5, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_bool_w_i32_member() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 6);

    test_op(s->ops, 0, EcsOpPush, 1, 6, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 3, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 4, EcsOpI32, 1, 1, T, x, ecs_id(ecs_i32_t));
    test_op(s->ops, 5, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_bool_2_bool() {
    typedef struct {
        ecs_bool_t x;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 8);

    test_op(s->ops, 0, EcsOpPush, 1, 8, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 3, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 4, EcsOpPush, 1, 3, T, n_2, n2);
    test_mn(s->ops, 5, EcsOpBool, 1, 1, offsetof(T, n_2), N1, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 6, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 7, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_i32_2_bool() {
    typedef struct {
        ecs_i32_t x;
    } N1;
    typedef struct {
        ecs_bool_t x;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 8);

    test_op(s->ops, 0, EcsOpPush, 1, 8, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 3, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpI32, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_i32_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 4, EcsOpPush, 1, 3, T, n_2, n2);
    test_mn(s->ops, 5, EcsOpBool, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 6, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 7, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_i32_i32_2_bool_bool() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 10);

    test_op(s->ops, 0, EcsOpPush, 1, 10, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 4, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpI32, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 3, EcsOpI32, 1, 1, offsetof(T, n_1), N1, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 5, EcsOpPush, 1, 4, T, n_2, n2);
    test_mn(s->ops, 6, EcsOpBool, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 7, EcsOpBool, 1, 1, offsetof(T, n_2), N2, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 8, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 9, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_bool_bool_2_i32_i32() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } N1;
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 10);

    test_op(s->ops, 0, EcsOpPush, 1, 10, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 4, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 3, EcsOpBool, 1, 1, offsetof(T, n_1), N1, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 5, EcsOpPush, 1, 4, T, n_2, n2);
    test_mn(s->ops, 6, EcsOpI32, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 7, EcsOpI32, 1, 1, offsetof(T, n_2), N2, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 8, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 9, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_i32_bool_2_bool_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
    } N1;
    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 10);

    test_op(s->ops, 0, EcsOpPush, 1, 10, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 4, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpI32, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 3, EcsOpBool, 1, 1, offsetof(T, n_1), N1, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 5, EcsOpPush, 1, 4, T, n_2, n2);
    test_mn(s->ops, 6, EcsOpBool, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 7, EcsOpI32, 1, 1, offsetof(T, n_2), N2, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 8, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 9, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_struct_1_bool_i32_2_i32_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
    } N1;
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 10);

    test_op(s->ops, 0, EcsOpPush, 1, 10, t);
    test_mp(s->ops, 1, EcsOpPush, 1, 4, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 3, EcsOpI32, 1, 1, offsetof(T, n_1), N1, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 5, EcsOpPush, 1, 4, T, n_2, n2);
    test_mn(s->ops, 6, EcsOpI32, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 7, EcsOpBool, 1, 1, offsetof(T, n_2), N2, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 8, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 9, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_2_lvls_bool() {
    typedef struct {
        ecs_bool_t x;
    } NN1;

    typedef struct {
        ecs_bool_t x;
    } NN2;

    typedef struct {
        NN1 nn_1;
    } N1;

    typedef struct {
        NN2 nn_2;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t nn1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t nn2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"nn_1", nn1},
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"nn_2", nn2},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 12);

    test_op(s->ops, 0, EcsOpPush, 1, 12, t);

    test_mp(s->ops, 1, EcsOpPush, 1, 5, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpPush, 1, 3, offsetof(T, n_1), N1, nn_1, nn1);
    test_mn(s->ops, 3, EcsOpBool, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 5, EcsOpPop, 1, 1, 0);

    test_mp(s->ops, 6, EcsOpPush, 1, 5, T, n_2, n2);
    test_mn(s->ops, 7, EcsOpPush, 1, 3, offsetof(T, n_2), N2, nn_2, nn2);
    test_mn(s->ops, 8, EcsOpBool, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 9, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 10, EcsOpPop, 1, 1, 0);

    test_op(s->ops, 11, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_2_lvls_bool_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } NN1;

    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } NN2;

    typedef struct {
        NN1 nn_1;
    } N1;

    typedef struct {
        NN2 nn_2;
    } N2;

    typedef struct {
        N1 n_1;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t nn1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t nn2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"nn_1", nn1},
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"nn_2", nn2},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 14);

    test_op(s->ops, 0, EcsOpPush, 1, 14, t);

    test_mp(s->ops, 1, EcsOpPush, 1, 6, T, n_1, n1);
    test_mn(s->ops, 2, EcsOpPush, 1, 4, offsetof(T, n_1), N1, nn_1, nn1);
    test_mn(s->ops, 3, EcsOpBool, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 4, EcsOpBool, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 5, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 6, EcsOpPop, 1, 1, 0);

    test_mp(s->ops, 7, EcsOpPush, 1, 6, T, n_2, n2);
    test_mn(s->ops, 8, EcsOpPush, 1, 4, offsetof(T, n_2), N2, nn_2, nn2);
    test_mn(s->ops, 9, EcsOpBool, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 10, EcsOpBool, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 11, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 12, EcsOpPop, 1, 1, 0);

    test_op(s->ops, 13, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_2_lvls_i32_i32_w_member_before() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } NN1;

    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } NN2;

    typedef struct {
        ecs_bool_t x;
        NN1 nn_1;
    } N1;

    typedef struct {
        ecs_bool_t x;
        NN2 nn_2;
    } N2;

    typedef struct {
        ecs_bool_t x;
        N1 n_1;
        ecs_bool_t y;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t nn1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t nn2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN2",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"nn_1", nn1},
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"nn_2", nn2},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"n_1", n1},
            {"y", ecs_id(ecs_bool_t)},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 18);

    test_op(s->ops, 0, EcsOpPush, 1, 18, t);

    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpPush, 1, 7, T, n_1, n1);
    test_mn(s->ops, 3, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 4, EcsOpPush, 1, 4, offsetof(T, n_1), N1, nn_1, nn1);
    test_mn(s->ops, 5, EcsOpI32, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 6, EcsOpI32, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 7, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 8, EcsOpPop, 1, 1, 0);

    test_mp(s->ops, 9, EcsOpBool, 1, 1, T, y, ecs_id(ecs_bool_t));
    test_mp(s->ops, 10, EcsOpPush, 1, 7, T, n_2, n2);
    test_mn(s->ops, 11, EcsOpBool, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 12, EcsOpPush, 1, 4, offsetof(T, n_2), N2, nn_2, nn2);
    test_mn(s->ops, 13, EcsOpI32, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 14, EcsOpI32, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 15, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 16, EcsOpPop, 1, 1, 0);

    test_op(s->ops, 17, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_2_lvls_1_bool_i32_2_i32_bool_w_member_before() {
    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
    } NN1;

    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
    } NN2;

    typedef struct {
        ecs_bool_t x;
        NN1 nn_1;
    } N1;

    typedef struct {
        ecs_bool_t x;
        NN2 nn_2;
    } N2;

    typedef struct {
        ecs_bool_t x;
        N1 n_1;
        ecs_bool_t y;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t nn1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t nn2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN2",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"nn_1", nn1},
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"nn_2", nn2},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"n_1", n1},
            {"y", ecs_id(ecs_bool_t)},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 18);

    test_op(s->ops, 0, EcsOpPush, 1, 18, t);

    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpPush, 1, 7, T, n_1, n1);
    test_mn(s->ops, 3, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 4, EcsOpPush, 1, 4, offsetof(T, n_1), N1, nn_1, nn1);
    test_mn(s->ops, 5, EcsOpBool, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 6, EcsOpI32, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 7, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 8, EcsOpPop, 1, 1, 0);

    test_mp(s->ops, 9, EcsOpBool, 1, 1, T, y, ecs_id(ecs_bool_t));
    test_mp(s->ops, 10, EcsOpPush, 1, 7, T, n_2, n2);
    test_mn(s->ops, 11, EcsOpBool, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 12, EcsOpPush, 1, 4, offsetof(T, n_2), N2, nn_2, nn2);
    test_mn(s->ops, 13, EcsOpI32, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 14, EcsOpBool, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 15, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 16, EcsOpPop, 1, 1, 0);

    test_op(s->ops, 17, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_nested_2_lvls_1_i32_bool_2_bool_i32_w_member_before() {
    typedef struct {
        ecs_i32_t x;
        ecs_bool_t y;
    } NN1;

    typedef struct {
        ecs_bool_t x;
        ecs_i32_t y;
    } NN2;

    typedef struct {
        ecs_bool_t x;
        NN1 nn_1;
    } N1;

    typedef struct {
        ecs_i64_t x;
        NN2 nn_2;
    } N2;

    typedef struct {
        ecs_i64_t x;
        N1 n_1;
        ecs_bool_t y;
        N2 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t nn1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_bool_t)},
        }
    });

    ecs_entity_t nn2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "NN2",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"nn_1", nn1},
        }
    });

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N2",
        .members = {
            {"x", ecs_id(ecs_i64_t)},
            {"nn_2", nn2},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i64_t)},
            {"n_1", n1},
            {"y", ecs_id(ecs_bool_t)},
            {"n_2", n2},
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 18);

    test_op(s->ops, 0, EcsOpPush, 1, 18, t);

    test_mp(s->ops, 1, EcsOpI64, 1, 1, T, x, ecs_id(ecs_i64_t));
    test_mp(s->ops, 2, EcsOpPush, 1, 7, T, n_1, n1);
    test_mn(s->ops, 3, EcsOpBool, 1, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 4, EcsOpPush, 1, 4, offsetof(T, n_1), N1, nn_1, nn1);
    test_mn(s->ops, 5, EcsOpI32, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, x, ecs_id(ecs_i32_t));
    test_mn(s->ops, 6, EcsOpBool, 1, 1, offsetof(T, n_1) + offsetof(N1, nn_1), NN1, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 7, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 8, EcsOpPop, 1, 1, 0);

    test_mp(s->ops, 9, EcsOpBool, 1, 1, T, y, ecs_id(ecs_bool_t));
    test_mp(s->ops, 10, EcsOpPush, 1, 7, T, n_2, n2);
    test_mn(s->ops, 11, EcsOpI64, 1, 1, offsetof(T, n_2), N2, x, ecs_id(ecs_i64_t));
    test_mn(s->ops, 12, EcsOpPush, 1, 4, offsetof(T, n_2), N2, nn_2, nn2);
    test_mn(s->ops, 13, EcsOpBool, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, x, ecs_id(ecs_bool_t));
    test_mn(s->ops, 14, EcsOpI32, 1, 1, offsetof(T, n_2) + offsetof(N2, nn_2), NN2, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 15, EcsOpPop, 1, 1, 0);
    test_op(s->ops, 16, EcsOpPop, 1, 1, 0);

    test_op(s->ops, 17, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_array_bool_1() {
    typedef struct {
        ecs_bool_t x[1];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 1}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_array_bool_2() {
    typedef struct {
        ecs_bool_t x[2];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 2}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpBool, 2, 1, T, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_array_bool_3() {
    typedef struct {
        ecs_bool_t x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 3}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpBool, 3, 1, T, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_array_bool_1_w_i32_after() {
    typedef struct {
        ecs_bool_t x[1];
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 1},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpI32, 1, 1, T, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_array_bool_2_w_i32_after() {
    typedef struct {
        ecs_bool_t x[2];
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 2},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpBool, 2, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpI32, 1, 1, T, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_array_bool_3_w_i32_after() {
    typedef struct {
        ecs_bool_t x[3];
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t), 3},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpBool, 3, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpI32, 1, 1, T, y, ecs_id(ecs_i32_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_array_struct_bool_3_w_i32_after() {
    typedef struct {
        ecs_bool_t x[2];
    } N1;

    typedef struct {
        N1 n_1[3];
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();
    
    ecs_entity_t n = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_bool_t), 2},
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"n_1", n, 3},
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 6);

    test_op(s->ops, 0, EcsOpPush, 1, 6, t);
    test_mp(s->ops, 1, EcsOpPush, 3, 3, T, n_1, n);
    test_mn(s->ops, 2, EcsOpBool, 2, 1, offsetof(T, n_1), N1, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);
    test_mp(s->ops, 4, EcsOpI32, 1, 1, T, x, ecs_id(ecs_i32_t));
    test_op(s->ops, 5, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_standalone_array_bool_1() {
    typedef struct {
        ecs_bool_t x[1];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 1
    });

    test_assert(a != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpArray, 1, 1, T, x, a);
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_standalone_array_bool_2() {
    typedef struct {
        ecs_bool_t x[2];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 2
    });

    test_assert(a != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpArray, 1, 1, T, x, a);
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_standalone_array_bool_3() {
    typedef struct {
        ecs_bool_t x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 3
    });

    test_assert(a != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpArray, 1, 1, T, x, a);
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_standalone_array_bool_1_w_bool_after() {
    typedef struct {
        ecs_bool_t x[1];
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 1
    });

    test_assert(a != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a},
            {"y", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpArray, 1, 1, T, x, a);
    test_mp(s->ops, 2, EcsOpBool, 1, 1, T, y, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_standalone_array_bool_2_w_bool_after() {
    typedef struct {
        ecs_bool_t x[2];
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 2
    });

    test_assert(a != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a},
            {"y", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpArray, 1, 1, T, x, a);
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_standalone_array_bool_3_w_bool_after() {
    typedef struct {
        ecs_bool_t x[3];
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "A",
        .type = ecs_id(ecs_bool_t),
        .count = 3
    });

    test_assert(a != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", a},
            {"y", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpArray, 1, 1, T, x, a);
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_vector() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity.name = "V",
        .type = ecs_id(ecs_bool_t)
    });

    test_assert(v != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, v, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpVector, 1, 1, v);

    ecs_fini(world);
}

void Serialized_ops_struct_w_vector() {
    typedef struct {
        ecs_vector_t *v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity.name = "V",
        .type = ecs_id(ecs_bool_t)
    });

    test_assert(v != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"v", v}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 3);

    test_op(s->ops, 0, EcsOpPush, 1, 3, t);
    test_mp(s->ops, 1, EcsOpVector, 1, 1, T, v, v);
    test_op(s->ops, 2, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_w_vector_w_bool_before() {
    typedef struct {
        bool x;
        ecs_vector_t *v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity.name = "V",
        .type = ecs_id(ecs_bool_t)
    });

    test_assert(v != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"v", v}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpVector, 1, 1, T, v, v);
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_struct_w_vector_w_bool_after() {
    typedef struct {
        ecs_vector_t *v;
        bool x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_vector_init(world, &(ecs_vector_desc_t){
        .entity.name = "V",
        .type = ecs_id(ecs_bool_t)
    });

    test_assert(v != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"v", v},
            {"x", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 4);

    test_op(s->ops, 0, EcsOpPush, 1, 4, t);
    test_mp(s->ops, 1, EcsOpVector, 1, 1, T, v, v);
    test_mp(s->ops, 2, EcsOpBool, 1, 1, T, x, ecs_id(ecs_bool_t));
    test_op(s->ops, 3, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_bitmask() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    test_assert(b != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, b, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpBitmask, 1, 1, b);

    ecs_fini(world);
}

void Serialized_ops_struct_w_bitmask() {
    typedef struct {
        bool before;
        ecs_u32_t v;
        bool after;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    test_assert(b != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"before", ecs_id(ecs_bool_t)},
            {"v", b},
            {"after", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 5);

    test_op(s->ops, 0, EcsOpPush, 1, 5, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, before, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpBitmask, 1, 1, T, v, b);
    test_mp(s->ops, 3, EcsOpBool, 1, 1, T, after, ecs_id(ecs_bool_t));
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}

void Serialized_ops_enum() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, e, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 1);

    test_op(s->ops, 0, EcsOpEnum, 1, 1, e);

    ecs_fini(world);
}

void Serialized_ops_struct_w_enum() {
    typedef struct {
        bool before;
        ecs_i32_t v;
        bool after;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"before", ecs_id(ecs_bool_t)},
            {"v", e},
            {"after", ecs_id(ecs_bool_t)}
        }
    });
    test_assert(t != 0);

    const EcsMetaTypeSerialized *s = ecs_get(world, t, EcsMetaTypeSerialized);
    test_assert(s != NULL);
    test_int(ecs_vector_count(s->ops), 5);

    test_op(s->ops, 0, EcsOpPush, 1, 5, t);
    test_mp(s->ops, 1, EcsOpBool, 1, 1, T, before, ecs_id(ecs_bool_t));
    test_mp(s->ops, 2, EcsOpEnum, 1, 1, T, v, e);
    test_mp(s->ops, 3, EcsOpBool, 1, 1, T, after, ecs_id(ecs_bool_t));
    test_op(s->ops, 4, EcsOpPop, 1, 1, 0);

    ecs_fini(world);
}
