/**
 * @file addons/meta/type_support/primitive_ts.c
 * @brief Primitive type support.
 */

#include "type_support.h"

#ifdef FLECS_META

/* ecs_string_t lifecycle */

static ECS_COPY(ecs_string_t, dst, src, {
    ecs_os_free(*(ecs_string_t*)dst);
    *(ecs_string_t*)dst = ecs_os_strdup(*(const ecs_string_t*)src);
})

static ECS_MOVE(ecs_string_t, dst, src, {
    ecs_os_free(*(ecs_string_t*)dst);
    *(ecs_string_t*)dst = *(ecs_string_t*)src;
    *(ecs_string_t*)src = NULL;
})

static ECS_DTOR(ecs_string_t, ptr, {
    ecs_os_free(*(ecs_string_t*)ptr);
    *(ecs_string_t*)ptr = NULL;
})

/* Primitive comparers */

#define FLECS_PRIMITIVE_COMPARE(T, NAME, EQUALS) \
static int flecs_compare_##NAME( \
    const void *a_ptr, \
    const void *b_ptr, \
    const ecs_type_info_t *ti) \
{ \
    (void)ti; \
    T a = *(const T*)a_ptr; \
    T b = *(const T*)b_ptr; \
    return (a > b) - (a < b); \
} \
static bool flecs_equals_##NAME( \
    const void *a_ptr, \
    const void *b_ptr, \
    const ecs_type_info_t *ti) \
{ \
    return EQUALS(a_ptr, b_ptr, ti) == 0; \
}

FLECS_PRIMITIVE_COMPARE(ecs_bool_t, bool, flecs_compare_bool)
FLECS_PRIMITIVE_COMPARE(ecs_char_t, char, flecs_compare_char)
FLECS_PRIMITIVE_COMPARE(ecs_byte_t, byte, flecs_compare_byte)
FLECS_PRIMITIVE_COMPARE(ecs_u8_t, u8, flecs_compare_u8)
FLECS_PRIMITIVE_COMPARE(ecs_u16_t, u16, flecs_compare_u16)
FLECS_PRIMITIVE_COMPARE(ecs_u32_t, u32, flecs_compare_u32)

FLECS_PRIMITIVE_COMPARE(ecs_u64_t, u64, flecs_compare_u64)
FLECS_PRIMITIVE_COMPARE(ecs_uptr_t, uptr, flecs_compare_uptr)
FLECS_PRIMITIVE_COMPARE(ecs_i8_t, i8, flecs_compare_i8)
FLECS_PRIMITIVE_COMPARE(ecs_i16_t, i16, flecs_compare_i16)
FLECS_PRIMITIVE_COMPARE(ecs_i32_t, i32, flecs_compare_i32)
FLECS_PRIMITIVE_COMPARE(ecs_i64_t, i64, flecs_compare_i64)
FLECS_PRIMITIVE_COMPARE(ecs_iptr_t, iptr, flecs_compare_iptr)

FLECS_PRIMITIVE_COMPARE(ecs_f32_t, f32, flecs_compare_u32)
FLECS_PRIMITIVE_COMPARE(ecs_f64_t, f64, flecs_compare_u64)
FLECS_PRIMITIVE_COMPARE(ecs_entity_t, entity, flecs_compare_entity)
FLECS_PRIMITIVE_COMPARE(ecs_id_t, id, flecs_compare_id)

#undef FLECS_PRIMITIVE_COMPARE

int flecs_compare_string(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *ti) {
    (void)ti;
    const ecs_string_t str_a = *((const ecs_string_t *) a_ptr);
    const ecs_string_t str_b = *((const ecs_string_t *) b_ptr);
    if(str_a == str_b) {
        return 0;
    }
    if(str_a == NULL) {
        return -1;
    }
    if(str_b == NULL) {
        return 1;
    }
    return ecs_os_strcmp(str_a, str_b);
}

bool flecs_equals_string(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *ti)
{
    return flecs_compare_string(a_ptr, b_ptr, ti) == 0;
}

static void flecs_set_primitive(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsPrimitive *type = ecs_field(it, EcsPrimitive, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        switch(type->kind) {
        case EcsBool:
            flecs_init_type_t(world, e, EcsPrimitiveType, bool);
            break;
        case EcsChar:
            flecs_init_type_t(world, e, EcsPrimitiveType, char);
            break;
        case EcsByte:
            flecs_init_type_t(world, e, EcsPrimitiveType, bool);
            break;
        case EcsU8:
            flecs_init_type_t(world, e, EcsPrimitiveType, uint8_t);
            break;
        case EcsU16:
            flecs_init_type_t(world, e, EcsPrimitiveType, uint16_t);
            break;
        case EcsU32:
            flecs_init_type_t(world, e, EcsPrimitiveType, uint32_t);
            break;
        case EcsU64:
            flecs_init_type_t(world, e, EcsPrimitiveType, uint64_t);
            break;
        case EcsI8:
            flecs_init_type_t(world, e, EcsPrimitiveType, int8_t);
            break;
        case EcsI16:
            flecs_init_type_t(world, e, EcsPrimitiveType, int16_t);
            break;
        case EcsI32:
            flecs_init_type_t(world, e, EcsPrimitiveType, int32_t);
            break;
        case EcsI64:
            flecs_init_type_t(world, e, EcsPrimitiveType, int64_t);
            break;
        case EcsF32:
            flecs_init_type_t(world, e, EcsPrimitiveType, float);
            break;
        case EcsF64:
            flecs_init_type_t(world, e, EcsPrimitiveType, double);
            break;
        case EcsUPtr:
            flecs_init_type_t(world, e, EcsPrimitiveType, uintptr_t);
            break;
        case EcsIPtr:
            flecs_init_type_t(world, e, EcsPrimitiveType, intptr_t);
            break;
        case EcsString:
            flecs_init_type_t(world, e, EcsPrimitiveType, char*);
            break;
        case EcsEntity:
            flecs_init_type_t(world, e, EcsPrimitiveType, ecs_entity_t);
            break;
        case EcsId:
            flecs_init_type_t(world, e, EcsPrimitiveType, ecs_id_t);
            break;
        }
    }
}

ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsPrimitive, { desc->kind });

    flecs_resume_readonly(world, &rs);
    return t;
}

void flecs_meta_primitives_init(
    ecs_world_t *world)
{
    ecs_entity_t primitive = ecs_entity(world, { .id = ecs_id(EcsPrimitive),
        .name = "primitive", .symbol = "EcsPrimitive" });
    ecs_add_pair(world, primitive, EcsOnInstantiate, EcsDontInherit);
    ecs_component(world, {
        .entity = primitive,
        .type.size = sizeof(EcsPrimitive),
        .type.alignment = ECS_ALIGNOF(EcsPrimitive)
    });

    ecs_observer(world, {
        .query.terms[0] = { .id = ecs_id(EcsPrimitive) },
        .events = {EcsOnSet},
        .callback = flecs_set_primitive,
        .global_observer = true
    });

    /* Initialize primitive types */
    #define ECS_PRIMITIVE(world, T, primitive_kind)\
        ecs_component_init(world, &(ecs_component_desc_t){\
            .entity = ecs_entity(world, {\
                .id = ecs_id(ecs_##T##_t),\
                .name = #T,\
                .symbol = #T \
            }),\
            .type.size = ECS_SIZEOF(ecs_##T##_t),\
            .type.alignment = ECS_ALIGNOF(ecs_##T##_t),\
        });\
        ecs_set(world, ecs_id(ecs_##T##_t), EcsPrimitive, {\
            .kind = primitive_kind\
        });\
        ecs_set_hooks(world, ecs_##T##_t, { \
            .cmp = flecs_compare_##T, \
            .equals = flecs_equals_##T \
        })

    ECS_PRIMITIVE(world, bool, EcsBool);
    ECS_PRIMITIVE(world, char, EcsChar);
    ECS_PRIMITIVE(world, byte, EcsByte);
    ECS_PRIMITIVE(world, u8, EcsU8);
    ECS_PRIMITIVE(world, u16, EcsU16);
    ECS_PRIMITIVE(world, u32, EcsU32);
    ECS_PRIMITIVE(world, u64, EcsU64);
    ECS_PRIMITIVE(world, uptr, EcsUPtr);
    ECS_PRIMITIVE(world, i8, EcsI8);
    ECS_PRIMITIVE(world, i16, EcsI16);
    ECS_PRIMITIVE(world, i32, EcsI32);
    ECS_PRIMITIVE(world, i64, EcsI64);
    ECS_PRIMITIVE(world, iptr, EcsIPtr);
    ECS_PRIMITIVE(world, f32, EcsF32);
    ECS_PRIMITIVE(world, f64, EcsF64);
    ECS_PRIMITIVE(world, string, EcsString);
    ECS_PRIMITIVE(world, entity, EcsEntity);
    ECS_PRIMITIVE(world, id, EcsId);

    #undef ECS_PRIMITIVE

    ecs_type_hooks_t string_hooks = *ecs_get_hooks(world, ecs_string_t);
    string_hooks.ctor = flecs_default_ctor;
    string_hooks.copy = ecs_copy(ecs_string_t);
    string_hooks.move = ecs_move(ecs_string_t);
    string_hooks.dtor = ecs_dtor(ecs_string_t);
    string_hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, ecs_id(ecs_string_t), &string_hooks);
}

#endif
