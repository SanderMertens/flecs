/**
 * @file addons/meta/rtt_lifecycle.c
 * @brief Runtime component lifecycle management.
 */

#include "flecs.h"
#include "meta.h"

#ifdef FLECS_META

/* Stores all the information necessary to forward a hook call to a
 * struct member's type */
typedef struct ecs_rtt_call_data_t {
    const ecs_type_info_t *type_info;
    int32_t offset;
    int32_t count;
} ecs_rtt_call_data_t;

typedef struct ecs_rtt_struct_ctx_t {
    ecs_vec_t members;
    ecs_vec_t dtors;
} ecs_rtt_struct_ctx_t;

/* Generic copy assign hook */
static void flecs_rtt_default_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_os_memcpy(dst_ptr, src_ptr, count * type_info->size);
}

/* Generic move assign hook */
static void flecs_rtt_default_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    flecs_rtt_default_copy(dst_ptr, src_ptr, count, type_info);
}

typedef enum flecs_rtt_action_t {
    FlecsRttCtor,
    FlecsRttDtor,
    FlecsRttCopy,
    FlecsRttMove,
    FlecsRttCompare,
    FlecsRttEquals
} flecs_rtt_action_t;

static FLECS_ALWAYS_INLINE int flecs_rtt_struct_action(
    void *ptr,
    const void *src,
    int32_t count,
    const ecs_type_info_t *ti,
    flecs_rtt_action_t action)
{
    if (action >= FlecsRttCompare && ptr == src) {
        return 0;
    }

    ecs_rtt_struct_ctx_t *ctx = ti->hooks.lifecycle_ctx;
    ecs_vec_t *vec = action == FlecsRttDtor ? &ctx->dtors : &ctx->members;
    const ecs_rtt_call_data_t *members = ecs_vec_first(vec);
    int32_t i, j, member_count = ecs_vec_count(vec);
    for (j = 0; j < count; j ++) {
        for (i = 0; i < member_count; i ++) {
            const ecs_rtt_call_data_t *m = &members[i];
            const ecs_type_info_t *mt = m->type_info;
            void *dst = ECS_OFFSET(ptr, ti->size * j + m->offset);
            const void *from = src ? ECS_OFFSET(src, ti->size * j + m->offset) : NULL;
            switch (action) {
            case FlecsRttCtor:
                (mt->hooks.ctor ? mt->hooks.ctor : flecs_default_ctor)(
                    dst, m->count, mt);
                break;
            case FlecsRttDtor:
                mt->hooks.dtor(dst, m->count, mt);
                break;
            case FlecsRttCopy:
                (mt->hooks.copy ? mt->hooks.copy : flecs_rtt_default_copy)(
                    dst, from, m->count, mt);
                break;
            case FlecsRttMove:
                (mt->hooks.move ? mt->hooks.move : flecs_rtt_default_move)(
                    dst, ECS_CONST_CAST(void*, from), m->count, mt);
                break;
            case FlecsRttCompare:
            case FlecsRttEquals:
                for (int32_t e = 0; e < m->count; e ++) {
                    const void *a = ECS_ELEM(dst, mt->size, e);
                    const void *b = ECS_ELEM(from, mt->size, e);
                    int result = action == FlecsRttEquals ?
                        !mt->hooks.equals(a, b, mt) : mt->hooks.cmp(a, b, mt);
                    if (result) {
                        return result;
                    }
                }
                break;
            }
        }
    }

    return 0;
}

static void flecs_rtt_struct_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    flecs_rtt_struct_action(ptr, NULL, count, ti, FlecsRttCtor);
}

static void flecs_rtt_struct_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    flecs_rtt_struct_action(ptr, NULL, count, ti, FlecsRttDtor);
}

static void flecs_rtt_struct_copy(
    void *ptr,
    const void *src,
    int32_t count,
    const ecs_type_info_t *ti)
{
    flecs_rtt_struct_action(ptr, src, count, ti, FlecsRttCopy);
}

static void flecs_rtt_struct_move(
    void *ptr,
    void *src,
    int32_t count,
    const ecs_type_info_t *ti)
{
    flecs_rtt_struct_action(ptr, src, count, ti, FlecsRttMove);
}

static int flecs_rtt_struct_cmp(
    const void *ptr,
    const void *src,
    const ecs_type_info_t *ti)
{
    return flecs_rtt_struct_action(
        ECS_CONST_CAST(void*, ptr), src, 1, ti, FlecsRttCompare);
}

static bool flecs_rtt_struct_equals(
    const void *ptr,
    const void *src,
    const ecs_type_info_t *ti)
{
    return !flecs_rtt_struct_action(
        ECS_CONST_CAST(void*, ptr), src, 1, ti, FlecsRttEquals);
}

static void flecs_rtt_free_lifecycle_nop(
    void *ctx)
{
    (void)ctx;
}

static void flecs_rtt_free_lifecycle_struct_ctx(
    void *ctx)
{
    if (!ctx) {
        return;
    }

    ecs_rtt_struct_ctx_t *data = ctx;
    ecs_vec_fini_t(NULL, &data->members, ecs_rtt_call_data_t);
    ecs_vec_fini_t(NULL, &data->dtors, ecs_rtt_call_data_t);
    ecs_os_free(ctx);
}

static ecs_rtt_struct_ctx_t* flecs_rtt_struct_members(
    ecs_world_t *world,
    const ecs_type_info_t *ti,
    const ecs_member_t *members,
    int32_t member_count,
    ecs_type_hooks_t *hooks)
{
    ecs_rtt_struct_ctx_t *result = ecs_os_malloc_t(ecs_rtt_struct_ctx_t);
    ecs_vec_init_t(NULL, &result->members, ecs_rtt_call_data_t, member_count);
    ecs_vec_init_t(NULL, &result->dtors, ecs_rtt_call_data_t, 0);
    ecs_type_hooks_t available = {
        .cmp = flecs_rtt_struct_cmp,
        .equals = flecs_rtt_struct_equals
    };
    for (int32_t i = 0; i < member_count; i ++) {
        const ecs_member_t *member = &members[i];
        const ecs_type_info_t *mt = ecs_get_type_info(world, member->type);
        if (!mt || mt == ti) {
            continue;
        }

        ecs_rtt_call_data_t *data =
            ecs_vec_append_t(NULL, &result->members, ecs_rtt_call_data_t);
        *data = (ecs_rtt_call_data_t){
            mt, member->offset, member->count ? member->count : 1
        };
        if (mt->hooks.dtor) {
            *ecs_vec_append_t(NULL, &result->dtors, ecs_rtt_call_data_t) = *data;
            available.dtor = flecs_rtt_struct_dtor;
        }

        if (mt->hooks.ctor && mt->hooks.ctor != flecs_default_ctor) {
            available.ctor = flecs_rtt_struct_ctor;
        }

        if (mt->hooks.copy) available.copy = flecs_rtt_struct_copy;
        if (mt->hooks.move) available.move = flecs_rtt_struct_move;
        if (!mt->hooks.cmp) available.cmp = NULL;
        if (!mt->hooks.equals) available.equals = NULL;
        available.flags |= mt->hooks.flags & ECS_TYPE_HOOKS_ILLEGAL;
    }

    if (hooks) {
        ecs_flags32_t flags = available.flags;
        hooks->ctor = flags & ECS_TYPE_HOOK_CTOR_ILLEGAL ? NULL : available.ctor;
        hooks->dtor = flags & ECS_TYPE_HOOK_DTOR_ILLEGAL ? NULL : available.dtor;
        hooks->copy = flags & ECS_TYPE_HOOK_COPY_ILLEGAL ? NULL : available.copy;
        hooks->move = flags & ECS_TYPE_HOOK_MOVE_ILLEGAL ? NULL : available.move;
        hooks->cmp = flags & ECS_TYPE_HOOK_CMP_ILLEGAL ? NULL : available.cmp;
        hooks->equals = flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL ? NULL : available.equals;
        hooks->flags = flags;
    }

    return result;
}

static void flecs_rtt_struct_hooks(
    ecs_world_t *world,
    const ecs_type_info_t *ti,
    const ecs_member_t *members,
    int32_t member_count,
    ecs_type_hooks_t *hooks)
{
    if (hooks->lifecycle_ctx_free) {
        hooks->lifecycle_ctx_free(hooks->lifecycle_ctx);
    }

    hooks->lifecycle_ctx = flecs_rtt_struct_members(
        world, ti, members, member_count, hooks);
    hooks->lifecycle_ctx_free = flecs_rtt_free_lifecycle_struct_ctx;
}

/*
 *
 * RTT vector support
 *
 */

/* Generic vector constructor. Makes sure the vector structure is initialized to
 * 0 elements */
static void flecs_rtt_vector_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    const ecs_type_info_t *element_ti = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        ecs_vec_t *vec = ECS_ELEM(ptr, type_info->size, i);
        ecs_vec_init(NULL, vec, element_ti->size, 0);
    }
}

static FLECS_ALWAYS_INLINE void flecs_rtt_vector_clear(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info,
    bool release)
{
    const ecs_type_info_t *element_ti = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        ecs_vec_t *vec = ECS_ELEM(ptr, type_info->size, i);
        int32_t num_elements = ecs_vec_count(vec);
        if (element_ti->hooks.dtor && num_elements) {
            flecs_type_info_dtor(
                ecs_vec_first(vec), num_elements, element_ti);
        }

        if (release) {
            ecs_vec_fini(NULL, vec, element_ti->size);
        } else {
            ecs_vec_clear(vec);
        }
    }
}

static void flecs_rtt_vector_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    flecs_rtt_vector_clear(ptr, count, ti, true);
}

/* Generic vector move hook. */
static void flecs_rtt_vector_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    flecs_rtt_vector_dtor(dst_ptr, count, type_info);
    int i;
    for (i = 0; i < count; i++) {
        ecs_vec_t *src_vec = ECS_ELEM(src_ptr, type_info->size, i);
        ecs_vec_t *dst_vec = ECS_ELEM(dst_ptr, type_info->size, i);
        *dst_vec = *src_vec;
        src_vec->array = NULL;
        src_vec->count = 0;
    }
}

/* Generic vector copy hook. It makes a deep copy of vector contents */
static void flecs_rtt_vector_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    const ecs_type_info_t *ti = type_info->hooks.lifecycle_ctx;
    flecs_rtt_vector_clear(dst_ptr, count, type_info, false);
    for (int32_t i = 0; i < count; i ++) {
        const ecs_vec_t *src = ECS_ELEM(src_ptr, type_info->size, i);
        ecs_vec_t *dst = ECS_ELEM(dst_ptr, type_info->size, i);
        ecs_vec_init_if(dst, ti->size);
        int32_t src_count = ecs_vec_count(src);
        ecs_vec_set_count(NULL, dst, ti->size, src_count);
        if (!src_count) {
            continue;
        }

        if (ti->hooks.ctor) {
            flecs_type_info_ctor(ecs_vec_first(dst), src_count, ti);
        } else {
            flecs_default_ctor(ecs_vec_first(dst), src_count, ti);
        }

        flecs_type_info_copy(ecs_vec_first(dst), ecs_vec_first(src), src_count, ti);
    }
}

/* Generic vector compare hook. */
static int flecs_rtt_vector_cmp(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if(a_ptr == b_ptr) {
        return 0;
    }

    const ecs_vec_t *vec_a = a_ptr;
    const ecs_vec_t *vec_b = b_ptr;

    ecs_size_t count_a = ecs_vec_count(vec_a);
    ecs_size_t count_b = ecs_vec_count(vec_b);
    {
        int c = count_a - count_b;
        if(c != 0) {
            return c;
        }
    }

    const ecs_type_info_t *element_ti = type_info->hooks.lifecycle_ctx;

    ecs_size_t element_size = element_ti->size;
    const void *a = ecs_vec_first(vec_a);
    const void *b = ecs_vec_first(vec_b);

    int i;
    for (i = 0; i < count_a; i++) {
        const void *a_element = ECS_ELEM(a, element_size, i);
        const void *b_element = ECS_ELEM(b, element_size, i);
        int c = flecs_type_info_cmp(a_element, b_element, element_ti);
        if(c != 0) {
            return c;
        }
    }

    return 0;
}

/* Generic vector equals hook. */
static bool flecs_rtt_vector_equals(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if(a_ptr == b_ptr) {
        return true;
    }

    const ecs_vec_t *vec_a = a_ptr;
    const ecs_vec_t *vec_b = b_ptr;

    ecs_size_t count_a = ecs_vec_count(vec_a);
    ecs_size_t count_b = ecs_vec_count(vec_b);
    if (count_a != count_b) {
        return false;
    }

    const ecs_type_info_t *element_ti = type_info->hooks.lifecycle_ctx;

    ecs_size_t element_size = element_ti->size;
    const void *a = ecs_vec_first(vec_a);
    const void *b = ecs_vec_first(vec_b);

    int i;
    for (i = 0; i < count_a; i++) {
        const void *a_element = ECS_ELEM(a, element_size, i);
        const void *b_element = ECS_ELEM(b, element_size, i);
        bool eq = flecs_type_info_equals(
            a_element, b_element, element_ti);
        if(!eq) {
            return false;
        }
    }

    return true;
}

static void flecs_rtt_free_lifecycle_map_ctx(
    void *ctx)
{
    ecs_os_free(ctx);
}

static void flecs_rtt_map_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    const ecs_meta_map_info_t *info = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        ecs_map_t *map = ECS_ELEM(ptr, type_info->size, i);
        flecs_meta_map_fini(info, map);
    }
}

static void flecs_rtt_map_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    flecs_rtt_map_dtor(dst_ptr, count, type_info);
    int i;
    for (i = 0; i < count; i++) {
        ecs_map_t *src_map = ECS_ELEM(src_ptr, type_info->size, i);
        ecs_map_t *dst_map = ECS_ELEM(dst_ptr, type_info->size, i);
        *dst_map = *src_map;
        ecs_os_zeromem(src_map);
    }
}

static void flecs_rtt_map_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    const ecs_meta_map_info_t *info = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        const ecs_map_t *src_map = ECS_ELEM(src_ptr, type_info->size, i);
        ecs_map_t *dst_map = ECS_ELEM(dst_ptr, type_info->size, i);
        flecs_meta_map_copy(info, dst_map, src_map);
    }
}

static int flecs_rtt_map_key_cmp(
    const ecs_meta_map_info_t *info,
    const void *a,
    const void *b)
{
    if (info->owns_key) {
        return flecs_type_info_cmp(a, b, info->key_ti);
    }

    /* Keys returned by the map iterator point to the map key */
    ecs_map_key_t key_a = *(const ecs_map_key_t*)a;
    ecs_map_key_t key_b = *(const ecs_map_key_t*)b;
    return (key_a > key_b) - (key_a < key_b);
}

static int flecs_rtt_map_cmp(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if (a_ptr == b_ptr) {
        return 0;
    }

    const ecs_map_t *a = a_ptr, *b = b_ptr;
    int32_t count_diff = ecs_map_count(a) - ecs_map_count(b);
    if (count_diff) {
        return count_diff;
    }

    const ecs_meta_map_info_t *info = type_info->hooks.lifecycle_ctx;

    /* Find the smallest key that is only in one of the maps */
    const void *first_key = NULL;
    ecs_meta_map_iter_t it = flecs_meta_map_iter(info, a);
    while (flecs_meta_map_next(&it)) {
        const void *key = flecs_meta_map_iter_key(&it);
        if ((!first_key || flecs_rtt_map_key_cmp(info, key, first_key) < 0) &&
            !flecs_meta_map_get(info, b, key))
        {
            first_key = key;
        }
    }

    if (first_key) {
        it = flecs_meta_map_iter(info, b);
        while (flecs_meta_map_next(&it)) {
            const void *key = flecs_meta_map_iter_key(&it);
            if (flecs_rtt_map_key_cmp(info, key, first_key) < 0 &&
                !flecs_meta_map_get(info, a, key))
            {
                return 1;
            }
        }
        return -1;
    }

    /* Maps have the same keys, compare values of the smallest key with
     * different values. */
    const ecs_type_info_t *ti = info->value_ti;
    int result = 0;
    it = flecs_meta_map_iter(info, a);
    while (flecs_meta_map_next(&it)) {
        const void *key = flecs_meta_map_iter_key(&it);
        if (!first_key || flecs_rtt_map_key_cmp(info, key, first_key) < 0) {
            int cmp = flecs_type_info_cmp(flecs_meta_map_iter_value(&it),
                flecs_meta_map_get(info, b, key), ti);
            if (cmp) {
                first_key = key;
                result = cmp;
            }
        }
    }
    return result;
}

static bool flecs_rtt_map_equals(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if (a_ptr == b_ptr) {
        return true;
    }

    const ecs_map_t *map_a = a_ptr;
    const ecs_map_t *map_b = b_ptr;

    int32_t count_a = ecs_map_count(map_a);
    int32_t count_b = ecs_map_count(map_b);
    if (count_a != count_b) {
        return false;
    }

    if (!count_a) {
        return true;
    }

    const ecs_meta_map_info_t *info = type_info->hooks.lifecycle_ctx;

    ecs_meta_map_iter_t it = flecs_meta_map_iter(info, map_a);
    while (flecs_meta_map_next(&it)) {
        const void *val_b = flecs_meta_map_get(
            info, map_b, flecs_meta_map_iter_key(&it));
        if (!val_b) {
            return false;
        }

        bool eq = flecs_type_info_equals(
            flecs_meta_map_iter_value(&it), val_b, info->value_ti);
        if (!eq) {
            return false;
        }
    }

    return true;
}

static void flecs_rtt_vector_hooks(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_entity_t element,
    ecs_type_hooks_t *hooks)
{
    const ecs_type_info_t *ti = ecs_is_alive(world, element)
        ? ecs_get_type_info(world, element) : NULL;
    if (!ti) {
        ecs_err("collection '%s' has invalid element type",
            ecs_get_name(world, component));
        return;
    }

    if (hooks->lifecycle_ctx_free) {
        hooks->lifecycle_ctx_free(hooks->lifecycle_ctx);
    }

    hooks->lifecycle_ctx = ECS_CONST_CAST(ecs_type_info_t*, ti);
    hooks->lifecycle_ctx_free = flecs_rtt_free_lifecycle_nop;
    hooks->ctor = flecs_rtt_vector_ctor;
    hooks->dtor = flecs_rtt_vector_dtor;
    hooks->move = flecs_rtt_vector_move;
    hooks->copy = flecs_rtt_vector_copy;
    hooks->cmp = ti->hooks.cmp && !(ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)
        ? flecs_rtt_vector_cmp : NULL;
    hooks->equals = ti->hooks.equals && !(ti->hooks.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL)
        ? flecs_rtt_vector_equals : NULL;
    hooks->flags |= ti->hooks.flags &
        (ECS_TYPE_HOOK_CMP_ILLEGAL | ECS_TYPE_HOOK_EQUALS_ILLEGAL);
    hooks->flags &= ECS_TYPE_HOOKS_ILLEGAL;
}

static int flecs_rtt_ensure_hook(
    ecs_world_t *world,
    ecs_entity_t type,
    bool equals);

static bool flecs_rtt_has_hook(
    const ecs_type_info_t *ti,
    bool equals)
{
    if (equals) {
        return ti->hooks.equals != NULL &&
            !(ti->hooks.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL);
    } else {
        return ti->hooks.cmp != NULL &&
            !(ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL);
    }
}

static bool flecs_rtt_map_has_hook(
    const ecs_meta_map_info_t *info,
    bool equals)
{
    if (!flecs_rtt_has_hook(info->value_ti, equals)) {
        return false;
    }

    /* Comparing maps with keys owned by the map requires comparing keys */
    return equals || !info->owns_key || flecs_rtt_has_hook(info->key_ti, false);
}

static void flecs_rtt_map_hooks(
    ecs_world_t *world,
    ecs_entity_t component,
    const EcsMap *map,
    ecs_type_hooks_t *hooks)
{
    ecs_meta_map_info_t info;
    if (flecs_meta_map_info_init(world, map->key_type, map->type, &info)) {
        ecs_err("map '%s' has invalid key or value type",
            ecs_get_name(world, component));
        return;
    }

    if (hooks->lifecycle_ctx_free) {
        hooks->lifecycle_ctx_free(hooks->lifecycle_ctx);
    }

    ecs_meta_map_info_t *ctx = ecs_os_malloc_t(ecs_meta_map_info_t);
    *ctx = info;

    const ecs_type_info_t *ti = info.value_ti;
    hooks->lifecycle_ctx = ctx;
    hooks->lifecycle_ctx_free = flecs_rtt_free_lifecycle_map_ctx;
    hooks->ctor = flecs_default_ctor;
    hooks->dtor = flecs_rtt_map_dtor;
    hooks->move = flecs_rtt_map_move;
    hooks->copy = flecs_rtt_map_copy;
    hooks->cmp = flecs_rtt_map_has_hook(&info, false) 
        ? flecs_rtt_map_cmp : NULL;
    hooks->equals = flecs_rtt_map_has_hook(&info, true) 
        ? flecs_rtt_map_equals : NULL;
    hooks->flags |= ti->hooks.flags &
        (ECS_TYPE_HOOK_CMP_ILLEGAL | ECS_TYPE_HOOK_EQUALS_ILLEGAL);
    hooks->flags &= ECS_TYPE_HOOKS_ILLEGAL;
}

static void flecs_rtt_set_hook(
    ecs_type_hooks_t *hooks,
    bool equals,
    ecs_cmp_t cmp,
    ecs_equals_t eq)
{
    if (equals) {
        hooks->equals = eq;
        hooks->flags &= ECS_TYPE_HOOKS_ILLEGAL;
        hooks->flags &= ~ECS_TYPE_HOOK_EQUALS_ILLEGAL;
    } else {
        hooks->cmp = cmp;
        hooks->flags &= ECS_TYPE_HOOKS_ILLEGAL;
        hooks->flags &= ~ECS_TYPE_HOOK_CMP_ILLEGAL;
    }
}

static int flecs_rtt_gen_struct_hook(
    ecs_world_t *world,
    ecs_entity_t component,
    bool equals,
    const ecs_member_t *members,
    int32_t member_count)
{
    int i;

    for (i = 0; i < member_count; i++) {
        const ecs_member_t *m = &members[i];
        if (m->type == component) {
            continue;
        }

        if (flecs_rtt_ensure_hook(world, m->type, equals)) {
            return -1;
        }
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_hooks_t hooks = ti->hooks;
    if (hooks.lifecycle_ctx_free != flecs_rtt_free_lifecycle_struct_ctx) {
        if (hooks.lifecycle_ctx) {
            return -1;
        }

        hooks.lifecycle_ctx = flecs_rtt_struct_members(
            world, ti, members, member_count, NULL);
        hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_struct_ctx;
    }

    flecs_rtt_set_hook(&hooks, equals,
        flecs_rtt_struct_cmp, flecs_rtt_struct_equals);
    ecs_set_hooks_id(world, component, &hooks);
    return 0;
}

static int flecs_rtt_gen_collection_hook(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_entity_t element,
    bool equals,
    ecs_cmp_t cmp,
    ecs_equals_t eq)
{
    if (!element || element == component ||
        flecs_rtt_ensure_hook(world, element, equals))
    {
        return -1;
    }

    const ecs_type_info_t *element_ti = ecs_get_type_info(world, element);
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_type_hooks_t hooks = ti->hooks;
    if (hooks.lifecycle_ctx && hooks.lifecycle_ctx != element_ti) {
        return -1;
    }

    hooks.lifecycle_ctx = ECS_CONST_CAST(ecs_type_info_t*, element_ti);
    hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_nop;
    flecs_rtt_set_hook(&hooks, equals, cmp, eq);
    ecs_set_hooks_id(world, component, &hooks);
    return 0;
}

static int flecs_rtt_gen_map_hook(
    ecs_world_t *world,
    ecs_entity_t component,
    const EcsMap *map,
    bool equals)
{
    if (!map || map->type == component ||
        flecs_rtt_ensure_hook(world, map->type, equals))
    {
        return -1;
    }

    ecs_meta_map_info_t info;
    if (flecs_meta_map_info_init(world, map->key_type, map->type, &info)) {
        return -1;
    }

    if (!flecs_rtt_map_has_hook(&info, equals)) {
        return -1;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_type_hooks_t hooks = ti->hooks;
    if (hooks.lifecycle_ctx_free != flecs_rtt_free_lifecycle_map_ctx) {
        if (hooks.lifecycle_ctx) {
            return -1;
        }

        ecs_meta_map_info_t *ctx = ecs_os_malloc_t(ecs_meta_map_info_t);
        *ctx = info;
        hooks.lifecycle_ctx = ctx;
        hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_map_ctx;
    }

    flecs_rtt_set_hook(&hooks, equals, flecs_rtt_map_cmp, flecs_rtt_map_equals);
    ecs_set_hooks_id(world, component, &hooks);
    return 0;
}

static int flecs_rtt_gen_underlying_hook(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_entity_t underlying,
    bool equals)
{
    if (!underlying || underlying == component ||
        flecs_rtt_ensure_hook(world, underlying, equals))
    {
        return -1;
    }

    const ecs_type_info_t *underlying_ti = ecs_get_type_info(
        world, underlying);
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_type_hooks_t hooks = ti->hooks;
    flecs_rtt_set_hook(&hooks, equals,
        underlying_ti->hooks.cmp, underlying_ti->hooks.equals);
    ecs_set_hooks_id(world, component, &hooks);
    return 0;
}

static int flecs_rtt_ensure_hook(
    ecs_world_t *world,
    ecs_entity_t type,
    bool equals)
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    if (!ti) {
        return -1;
    }

    if (flecs_rtt_has_hook(ti, equals)) {
        return 0;
    }

    const EcsType *type_ptr = ecs_get(world, type, EcsType);
    if (!type_ptr) {
        return -1;
    }

    switch (type_ptr->kind) {
    case EcsStructType: {
        const EcsStruct *st = ecs_get(world, type, EcsStruct);
        return st ? flecs_rtt_gen_struct_hook(world, type, equals,
            st->members.array, st->members.count) : -1;
    }
    case EcsArrayType: {
        const EcsArray *arr = ecs_get(world, type, EcsArray);
        if (!arr || arr->type == type) {
            return -1;
        }

        ecs_member_t member = { .type = arr->type, .count = arr->count };
        return flecs_rtt_gen_struct_hook(world, type, equals, &member, 1);
    }
    case EcsVectorType: {
        const EcsVector *v = ecs_get(world, type, EcsVector);
        return flecs_rtt_gen_collection_hook(world, type,
            v ? v->type : 0, equals, flecs_rtt_vector_cmp, flecs_rtt_vector_equals);
    }
    case EcsMapType: {
        const EcsMap *m = ecs_get(world, type, EcsMap);
        return flecs_rtt_gen_map_hook(world, type, m, equals);
    }
    case EcsEnumType: {
        const EcsEnum *e = ecs_get(world, type, EcsEnum);
        return flecs_rtt_gen_underlying_hook(world, type,
            e ? e->underlying_type : 0, equals);
    }
    case EcsBitmaskType:
        return flecs_rtt_gen_underlying_hook(world, type,
            ecs_id(ecs_u32_t), equals);
    case EcsPrimitiveType:
    case EcsOpaqueType:
    case EcsValueType:
        break;
    }

    return -1;
}

int ecs_set_rtt_compare(
    ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);
    return flecs_rtt_ensure_hook(world, type, false);
}

int ecs_set_rtt_equals(
    ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);
    return flecs_rtt_ensure_hook(world, type, true);
}

void flecs_rtt_init_default_hooks(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    EcsType *type_field = ecs_field(it, EcsType, 0);

    int i;
    for (i = 0; i < it->count; i++) {
        EcsType *type = &type_field[i];
        if (type->existing) {
            continue; /* non-rtt type. Ignore. */
        }

        /* If a component is defined from reflection data, configure appropriate
         * default hooks.
         * - For trivial types, at least set a default constructor so memory is
         *   zero-initialized.
         * - For struct types, configure a hook that in turn calls hooks of
         *   member types, if those member types have hooks defined themselves.
         * - For array types, configure a hook that in turn calls hooks for the
         *   underlying type, for each element in the array.
         * - For vector types, configure hooks to manage the vector structure
         *   itself, move the vector and deep-copy vector elements.
         */

        ecs_entity_t component = it->entities[i];

        /* Skip configuring hooks for ids already in use */
        const ecs_world_t* w = ecs_get_world(world);
        if(ecs_id_in_use(w, component) || 
            ecs_id_in_use(w, ecs_pair(component, EcsWildcard))) {
            continue;
        } 

        const ecs_type_info_t *ti = ecs_get_type_info(world, component);
        ecs_assert(ti,ECS_INTERNAL_ERROR,NULL);
        ecs_type_hooks_t hooks = ti->hooks;

        if (type->kind == EcsStructType) {
            const EcsStruct *st = ecs_get(world, component, EcsStruct);
            flecs_rtt_struct_hooks(
                world, ti, st->members.array, st->members.count, &hooks);
        } else if (type->kind == EcsArrayType) {
            const EcsArray *arr = ecs_get(world, component, EcsArray);
            ecs_member_t member = { .type = arr->type, .count = arr->count };
            flecs_rtt_struct_hooks(world, ti, &member, 1, &hooks);
        } else if (type->kind == EcsVectorType) {
            const EcsVector *vector = ecs_get(world, component, EcsVector);
            flecs_rtt_vector_hooks(world, component, vector->type, &hooks);
        } else if (type->kind == EcsMapType) {
            const EcsMap *map = ecs_get(world, component, EcsMap);
            flecs_rtt_map_hooks(world, component, map, &hooks);
        } else if (type->kind == EcsValueType) {
            flecs_value_hooks(world, &hooks);
        }

        /* Make sure there is at least a default constructor. This ensures that
         * a new component value does not contain uninitialized memory, which
         * could cause serializers to crash when for example inspecting string
         * fields. */
        if(!hooks.ctor && !(hooks.flags & ECS_TYPE_HOOK_CTOR_ILLEGAL)) {
            hooks.ctor = flecs_default_ctor;
        }

        hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
        ecs_set_hooks_id(
            world,
            component,
            &hooks);
    }
}

#endif
