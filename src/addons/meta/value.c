/**
 * @file addons/meta/value.c
 * @brief Functions for working with value values.
 */

#include "meta.h"
#include "type_support/type_support.h"

#ifdef FLECS_META

static const ecs_type_info_t* flecs_value_type_info(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_assert(ti != NULL, ECS_INVALID_PARAMETER,
        "entity '%s' is not a type",
        flecs_errstr(ecs_get_path(world, type)));
    return ti;
}

static void* flecs_value_alloc(
    const ecs_type_info_t *ti)
{
    void *result = ecs_os_calloc(ti->size);
    flecs_type_info_ctor(result, 1, ti);
    return result;
}

int flecs_value_ensure_type(
    const ecs_world_t *world,
    ecs_value_t *v,
    ecs_entity_t type)
{
    ecs_assert(v != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);

    if (v->type == type && v->ptr) {
        return 0;
    }

    const ecs_type_info_t *ti = flecs_value_type_info(world, type);
    if (!ti) {
        return -1;
    }

    ecs_value_fini(world, v);

    v->type = type;
    v->ptr = flecs_value_alloc(ti);

    return 0;
}

void ecs_value_set(
    const ecs_world_t *world,
    ecs_value_t *v,
    ecs_entity_t type,
    const void *ptr)
{
    ecs_assert(v != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETER,
        "value must be assigned a valid type");
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER,
        "value must be assigned a valid value");

    const ecs_type_info_t *ti = flecs_value_type_info(world, type);
    if (!ti) {
        return;
    }

    if (v->type != type || !v->ptr) {
        ecs_value_fini(world, v);
        v->type = type;
        v->ptr = flecs_value_alloc(ti);
    }

    flecs_type_info_copy(v->ptr, ptr, 1, ti);
}

ecs_value_t ecs_value_new(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_assert(type != 0, ECS_INVALID_PARAMETER,
        "value must be assigned a valid type");

    ecs_value_t result = {0};

    const ecs_type_info_t *ti = flecs_value_type_info(world, type);
    if (!ti) {
        return result;
    }

    result.type = type;
    result.ptr = flecs_value_alloc(ti);

    return result;
}

ecs_value_t ecs_value_init(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr)
{
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER,
        "value must be assigned a valid value");

    ecs_value_t result = ecs_value_new(world, type);
    if (!result.ptr) {
        return result;
    }

    const ecs_type_info_t *ti = flecs_value_type_info(world, type);
    flecs_type_info_copy(result.ptr, ptr, 1, ti);

    return result;
}

void ecs_value_set_type(
    const ecs_world_t *world,
    ecs_value_t *v,
    ecs_entity_t type)
{
    ecs_assert(v != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_value_fini(world, v);
    v->type = type;
}

void ecs_value_fini(
    const ecs_world_t *world,
    ecs_value_t *v)
{
    ecs_assert(v != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!v->ptr) {
        v->type = 0;
        return;
    }

    ecs_assert(v->type != 0, ECS_INVALID_PARAMETER,
        "value has a value but no type");

    const ecs_type_info_t *ti = flecs_value_type_info(world, v->type);
    if (ti) {
        flecs_type_info_dtor(v->ptr, 1, ti);
    }

    ecs_os_free(v->ptr);

    v->type = 0;
    v->ptr = NULL;
}

void ecs_value_copy(
    const ecs_world_t *world,
    ecs_value_t *dst,
    const ecs_value_t *src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!src->ptr) {
        ecs_value_set_type(world, dst, src->type);
        return;
    }

    ecs_value_set(world, dst, src->type, src->ptr);
}

void ecs_value_copy_ctor(
    const ecs_world_t *world,
    ecs_value_t *dst,
    const ecs_value_t *src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src != NULL, ECS_INVALID_PARAMETER, NULL);

    dst->type = 0;
    dst->ptr = NULL;

    ecs_value_copy(world, dst, src);
}

void ecs_value_move(
    const ecs_world_t *world,
    ecs_value_t *dst,
    ecs_value_t *src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_value_fini(world, dst);

    *dst = *src;
    src->type = 0;
    src->ptr = NULL;
}

void ecs_value_move_ctor(
    const ecs_world_t *world,
    ecs_value_t *dst,
    ecs_value_t *src)
{
    (void)world;
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src != NULL, ECS_INVALID_PARAMETER, NULL);

    *dst = *src;
    src->type = 0;
    src->ptr = NULL;
}

int ecs_value_compare(
    const ecs_world_t *world,
    const ecs_value_t *a,
    const ecs_value_t *b)
{
    ecs_assert(a != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);

    if (a->type != b->type) {
        return (a->type > b->type) ? 1 : -1;
    }

    if (a->ptr == b->ptr) {
        return 0;
    }

    if (!a->ptr || !b->ptr) {
        return a->ptr ? 1 : -1;
    }

    const ecs_type_info_t *ti = flecs_value_type_info(world, a->type);
    ecs_assert(ti != NULL, ECS_INVALID_PARAMETER, NULL);

    return flecs_type_info_cmp(a->ptr, b->ptr, ti);
}

bool ecs_value_equals(
    const ecs_world_t *world,
    const ecs_value_t *a,
    const ecs_value_t *b)
{
    ecs_assert(a != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(b != NULL, ECS_INVALID_PARAMETER, NULL);

    if (a->type != b->type) {
        return false;
    }

    if (a->ptr == b->ptr) {
        return true;
    }

    if (!a->ptr || !b->ptr) {
        return false;
    }

    const ecs_type_info_t *ti = flecs_value_type_info(world, a->type);
    ecs_assert(ti != NULL, ECS_INVALID_PARAMETER, NULL);

    return flecs_type_info_equals(a->ptr, b->ptr, ti);
}

static ecs_world_t* flecs_value_world(
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.lifecycle_ctx;
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    return world;
}

static void flecs_value_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = flecs_value_world(ti);
    ecs_value_t *v = ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_value_fini(world, &v[i]);
    }
}

static void flecs_value_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = flecs_value_world(ti);
    ecs_value_t *dst = dst_ptr;
    ecs_value_t *src = src_ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_value_move(world, &dst[i], &src[i]);
    }
}

static void flecs_value_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = flecs_value_world(ti);
    ecs_value_t *dst = dst_ptr;
    const ecs_value_t *src = src_ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_value_copy(world, &dst[i], &src[i]);
    }
}

static int flecs_value_cmp(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = flecs_value_world(ti);
    const ecs_value_t *a = a_ptr;
    const ecs_value_t *b = b_ptr;

    if (a->type != b->type) {
        return (a->type > b->type) ? 1 : -1;
    }

    if (a->ptr == b->ptr) {
        return 0;
    }

    if (!a->ptr || !b->ptr) {
        return a->ptr ? 1 : -1;
    }

    const ecs_type_info_t *value_ti = ecs_get_type_info(world, a->type);
    ecs_assert(value_ti != NULL, ECS_INTERNAL_ERROR, NULL);

    if (value_ti->hooks.cmp &&
        !(value_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL))
    {
        return value_ti->hooks.cmp(a->ptr, b->ptr, value_ti);
    }

    return ecs_os_memcmp(a->ptr, b->ptr, value_ti->size);
}

static bool flecs_value_equals(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *ti)
{
    return flecs_value_cmp(a_ptr, b_ptr, ti) == 0;
}

static void flecs_value_fini_lifecycle_ctx(
    void *ctx)
{
    (void)ctx;
}

void flecs_value_hooks(
    ecs_world_t *world,
    ecs_type_hooks_t *hooks)
{
    hooks->flags = 0;
    hooks->ctor = flecs_default_ctor;
    hooks->dtor = flecs_value_dtor;
    hooks->move = flecs_value_move;
    hooks->copy = flecs_value_copy;
    hooks->copy_ctor = NULL;
    hooks->move_ctor = NULL;
    hooks->ctor_move_dtor = NULL;
    hooks->move_dtor = NULL;
    hooks->cmp = flecs_value_cmp;
    hooks->equals = flecs_value_equals;
    hooks->lifecycle_ctx = world;
    hooks->lifecycle_ctx_free = flecs_value_fini_lifecycle_ctx;
}

bool flecs_type_is_value(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    if (!type) {
        return false;
    }

    if (type == ecs_id(ecs_value_t)) {
        return true;
    }

    const EcsType *t = ecs_get(world, type, EcsType);
    return t && t->kind == EcsValueType;
}

ecs_entity_t flecs_value_type_constraint(
    const ecs_world_t *world,
    ecs_entity_t value_type)
{
    if (!value_type || value_type == ecs_id(ecs_value_t)) {
        return 0;
    }

    const EcsValue *v = ecs_get(world, value_type, EcsValue);
    return v ? v->type : 0;
}

bool ecs_value_type_accepts(
    const ecs_world_t *world,
    ecs_entity_t value_type,
    ecs_entity_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!type || !ecs_is_alive(world, type)) {
        return false;
    }

    if (flecs_type_is_value(world, type)) {
        return false;
    }

    ecs_entity_t constraint = flecs_value_type_constraint(world, value_type);
    if (!constraint) {
        return ecs_get_type_info(world, type) != NULL;
    }

    if (type == constraint) {
        return true;
    }

    return flecs_struct_is_derived_from(world, type, constraint);
}

static void flecs_set_value_type(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsValue *value = ecs_field(it, EcsValue, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t type = value[i].type;

        if (type) {
            if (!ecs_is_alive(world, type)) {
                ecs_err("value type '%s' has invalid type constraint",
                    ecs_get_name(world, e));
                continue;
            }

            if (!ecs_get_type_info(world, type)) {
                ecs_err("type constraint '%s' of value type '%s' is not a type",
                    flecs_errstr(ecs_get_path(world, type)),
                    ecs_get_name(world, e));
                continue;
            }

            if (flecs_type_is_value(world, type)) {
                ecs_err("type constraint of value type '%s' cannot be a "
                    "value type", ecs_get_name(world, e));
                continue;
            }
        }

        if (flecs_init_type_t(it->stage, e, EcsValueType, ecs_value_t)) {
            continue;
        }
    }
}

ecs_entity_t ecs_value_type_init(
    ecs_world_t *world,
    const ecs_value_type_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsValue, { .type = desc->type });

    flecs_resume_readonly(world, &rs);

    if (!ecs_has(world, t, EcsType)) {
        return 0;
    }

    return t;
}

void flecs_meta_value_init(
    ecs_world_t *world)
{
    ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {
            .id = ecs_id(ecs_value_t),
            .name = "value",
            .symbol = "ecs_value_t"
        }),
        .type.size = ECS_SIZEOF(ecs_value_t),
        .type.alignment = ECS_ALIGNOF(ecs_value_t)
    });

    ecs_type_hooks_t hooks = *ecs_get_hooks_id(world, ecs_id(ecs_value_t));
    flecs_value_hooks(world, &hooks);
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, ecs_id(ecs_value_t), &hooks);

    flecs_init_type_t(world, ecs_id(ecs_value_t),
        EcsValueType, ecs_value_t);

    ecs_entity_t value_component = ecs_entity(world, {
        .id = ecs_id(EcsValue), .name = "value_type", .symbol = "EcsValue" });
    ecs_add_pair(world, value_component, EcsOnInstantiate, EcsDontInherit);
    ecs_component(world, {
        .entity = value_component,
        .type.size = sizeof(EcsValue),
        .type.alignment = ECS_ALIGNOF(EcsValue)
    });

    ecs_observer(world, {
        .query.terms[0] = { .id = ecs_id(EcsValue) },
        .events = {EcsOnSet},
        .callback = flecs_set_value_type,
        .global_observer = true
    });
}

#endif
