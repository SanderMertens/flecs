#include <meta.h>

static void meta_test_primitive(
    ecs_world_t *world, 
    ecs_entity_t t, 
    ecs_primitive_kind_t kind, 
    ecs_size_t size, 
    ecs_size_t alignment) 
{
    const EcsComponent *ct = ecs_get(world, t, EcsComponent);
    test_assert(ct != NULL);
    test_assert(ct->size == size);
    test_assert(ct->alignment == alignment);

    const EcsMetaType *mt = ecs_get(world, t, EcsMetaType);
    test_assert(mt != NULL);
    test_assert(mt->kind == EcsPrimitiveType);

    const EcsPrimitive *pt = ecs_get(world, t, EcsPrimitive);
    test_assert(pt != NULL);
    test_assert(pt->kind == kind);
}

#define meta_test_type(kind, type)\
    static void meta_test_##type(ecs_world_t *world, ecs_entity_t t) {\
        meta_test_primitive(world, t, kind, ECS_SIZEOF(ecs_##type##_t), ECS_ALIGNOF(ecs_##type##_t));\
    }

#define meta_test_builtin(world, type)\
    meta_test_##type(world, ecs_id(ecs_##type##_t))

meta_test_type(EcsBool, bool)
meta_test_type(EcsByte, byte)
meta_test_type(EcsChar, char)
meta_test_type(EcsI8, i8)
meta_test_type(EcsI16, i16)
meta_test_type(EcsI32, i32)
meta_test_type(EcsI64, i64)
meta_test_type(EcsIPtr, iptr)
meta_test_type(EcsU8, u8)
meta_test_type(EcsU16, u16)
meta_test_type(EcsU32, u32)
meta_test_type(EcsU64, u64)
meta_test_type(EcsUPtr, uptr)
meta_test_type(EcsF32, f32)
meta_test_type(EcsF64, f64)
meta_test_type(EcsString, string)
meta_test_type(EcsEntity, entity)

void PrimitiveTypes_bool() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsBool});
    test_assert(t != 0);

    meta_test_bool(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_byte() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsByte});
    test_assert(t != 0);

    meta_test_byte(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_char() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsChar});
    test_assert(t != 0);

    meta_test_char(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_i8() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsI8});
    test_assert(t != 0);

    meta_test_i8(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_i16() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsI16});
    test_assert(t != 0);

    meta_test_i16(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_i32() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsI32});
    test_assert(t != 0);

    meta_test_i32(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_i64() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsI64});
    test_assert(t != 0);

    meta_test_i64(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_iptr() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsIPtr});
    test_assert(t != 0);

    meta_test_iptr(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_u8() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsU8});
    test_assert(t != 0);

    meta_test_u8(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_u16() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsU16});
    test_assert(t != 0);

    meta_test_u16(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_u32() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsU32});
    test_assert(t != 0);

    meta_test_u32(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_u64() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsU64});
    test_assert(t != 0);

    meta_test_u64(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_uptr() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsUPtr});
    test_assert(t != 0);

    meta_test_uptr(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_float() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsF32});
    test_assert(t != 0);

    meta_test_f32(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_double() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsF64});
    test_assert(t != 0);

    meta_test_f64(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_string() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsString});
    test_assert(t != 0);

    meta_test_string(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_set(world, 0, EcsPrimitive, {.kind = EcsEntity});
    test_assert(t != 0);

    meta_test_entity(world, t);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_bool() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, bool);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_byte() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, byte);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_char() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, char);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_i8() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, i8);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_i16() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, i16);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_i32() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, i32);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_i64() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, i64);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_iptr() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, iptr);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_u8() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, u8);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_u16() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, i16);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_u32() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, u32);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_u64() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, u64);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_uptr() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, uptr);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_float() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, f32);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_double() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, f64);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_string() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, string);

    ecs_fini(world);
}

void PrimitiveTypes_builtin_entity() {
    ecs_world_t *world = ecs_init();

    meta_test_builtin(world, entity);

    ecs_fini(world);
}
