/**
 * @file addons/meta/rtt_lifecycle.c
 * @brief Runtime component lifecycle management.
 */

#include "flecs.h"
#include "meta.h"

#ifdef FLECS_META

/* Per-member hook forwarding data. RTT (runtime type) hooks work by
 * iterating over each member and invoking the member type's hook at
 * the member's offset within the struct. */
typedef struct ecs_rtt_call_data_t {
    union {
        ecs_xtor_t xtor;
        ecs_move_t move;
        ecs_copy_t copy;
        ecs_cmp_t cmp;
        ecs_equals_t equals;
    } hook;
    const ecs_type_info_t *type_info;
    int32_t offset;
    int32_t count;
} ecs_rtt_call_data_t;

/* Lifecycle context for runtime struct types */
typedef struct ecs_rtt_struct_ctx_t {
    ecs_vec_t vctor;   /* vector<ecs_rtt_call_data_t> */
    ecs_vec_t vdtor;   /* vector<ecs_rtt_call_data_t> */
    ecs_vec_t vmove;   /* vector<ecs_rtt_call_data_t> */
    ecs_vec_t vcopy;   /* vector<ecs_rtt_call_data_t> */
    ecs_vec_t vcmp;   /* vector<ecs_rtt_call_data_t> */
    ecs_vec_t vequals; /* vector<ecs_rtt_call_data_t> */

} ecs_rtt_struct_ctx_t;

/* Lifecycle context for runtime arrays */
typedef struct ecs_rtt_array_ctx_t {
    const ecs_type_info_t *type_info;
    int32_t elem_count;
} ecs_rtt_array_ctx_t;

/* Lifecycle context for runtime vectors */
typedef struct ecs_rtt_vector_ctx_t {
    const ecs_type_info_t *type_info;
} ecs_rtt_vector_ctx_t;

/* Fallback copy hook: plain memcpy for members without a copy hook. */
static
void flecs_rtt_default_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_os_memcpy(dst_ptr, src_ptr, count * type_info->size);
}

/* Fallback move hook: plain memcpy for members without a move hook. */
static
void flecs_rtt_default_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    flecs_rtt_default_copy(dst_ptr, src_ptr, count, type_info);
}

/* -- RTT struct support -- */

/* Invoke per-member ctor/dtor hooks using the lifecycle context's call data. */
static
void flecs_rtt_struct_xtor(
    ecs_vec_t *xtor_data_vec,
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    int cb_count = ecs_vec_count(xtor_data_vec);
    int i, j;
    for (j = 0; j < count; j++) {
        void *elem_ptr = ECS_ELEM(ptr, type_info->size, j);
        for (i = 0; i < cb_count; i++) {
            ecs_rtt_call_data_t *xtor_data =
                ecs_vec_get_t(xtor_data_vec, ecs_rtt_call_data_t, i);
            xtor_data->hook.xtor(
                ECS_OFFSET(elem_ptr, xtor_data->offset),
                xtor_data->count,
                xtor_data->type_info);
        }
    }
}

/* RTT struct constructor: forwards to each member's ctor. */
static
void flecs_rtt_struct_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_struct_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_rtt_struct_xtor(&rtt_ctx->vctor, ptr, count, type_info);
}

/* RTT struct destructor: forwards to each member's dtor. */
static
void flecs_rtt_struct_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_struct_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_rtt_struct_xtor(&rtt_ctx->vdtor, ptr, count, type_info);
}

/* RTT struct move: forwards to each member's move hook. */
static
void flecs_rtt_struct_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_struct_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

    int cb_count = ecs_vec_count(&rtt_ctx->vmove);
    int i, j;
    for (j = 0; j < count; j++) {
        ecs_size_t elem_offset = type_info->size * j;
        void *elem_dst_ptr = ECS_OFFSET(dst_ptr, elem_offset);
        void *elem_src_ptr = ECS_OFFSET(src_ptr, elem_offset);
        for (i = 0; i < cb_count; i++) {
            ecs_rtt_call_data_t *move_data =
                ecs_vec_get_t(&rtt_ctx->vmove, ecs_rtt_call_data_t, i);
            move_data->hook.move(
                ECS_OFFSET(elem_dst_ptr, move_data->offset),
                ECS_OFFSET(elem_src_ptr, move_data->offset),
                move_data->count,
                move_data->type_info);
        }
    }
}

/* RTT struct copy: forwards to each member's copy hook. */
static
void flecs_rtt_struct_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_struct_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

    int cb_count = ecs_vec_count(&rtt_ctx->vcopy);
    int i, j;
    for (j = 0; j < count; j++) {
        ecs_size_t elem_offset = type_info->size * j;
        void *elem_dst_ptr = ECS_OFFSET(dst_ptr, elem_offset);
        const void *elem_src_ptr = ECS_OFFSET(src_ptr, elem_offset);
        for (i = 0; i < cb_count; i++) {
            ecs_rtt_call_data_t *copy_data =
                ecs_vec_get_t(&rtt_ctx->vcopy, ecs_rtt_call_data_t, i);
            copy_data->hook.copy(
                ECS_OFFSET(elem_dst_ptr, copy_data->offset),
                ECS_OFFSET(elem_src_ptr, copy_data->offset),
                copy_data->count,
                copy_data->type_info);
        }
    }
}

/* RTT struct compare: forwards to each member's cmp hook. */
static
int flecs_rtt_struct_cmp(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if(a_ptr == b_ptr) {
        return 0;
    }

    ecs_rtt_struct_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

    int cb_count = ecs_vec_count(&rtt_ctx->vcmp);
    int i;
    for (i = 0; i < cb_count; i++) {
        ecs_rtt_call_data_t *comp_data =
        ecs_vec_get_t(&rtt_ctx->vcmp, ecs_rtt_call_data_t, i);
        int c = comp_data->hook.cmp(
            ECS_OFFSET(a_ptr, comp_data->offset),
            ECS_OFFSET(b_ptr, comp_data->offset),
            comp_data->type_info);
        if (c != 0) {
            return c;
        }
    }
    return 0;
}

/* RTT struct equals: forwards to each member's equals hook. */
static
bool flecs_rtt_struct_equals(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if(a_ptr == b_ptr) {
        return true;
    }

    ecs_rtt_struct_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

    int cb_count = ecs_vec_count(&rtt_ctx->vequals);
    int i;
    for (i = 0; i < cb_count; i++) {
        ecs_rtt_call_data_t *comp_data =
        ecs_vec_get_t(&rtt_ctx->vequals, ecs_rtt_call_data_t, i);
        bool eq = comp_data->hook.equals(
            ECS_OFFSET(a_ptr, comp_data->offset),
            ECS_OFFSET(b_ptr, comp_data->offset),
            comp_data->type_info);
        if (!eq) {
            return false;
        }
    }
    return true;
}

/* No-op lifecycle context free; used when hooks exist but no ctx was allocated. */
static
void flecs_rtt_free_lifecycle_nop(
    void *ctx)
{
    (void)ctx;
}

/* Free the lifecycle context for a runtime struct type. */
static
void flecs_rtt_free_lifecycle_struct_ctx(
    void *ctx)
{
    if (!ctx) {
        return;
    }

    ecs_rtt_struct_ctx_t *lifecycle_ctx = ctx;

    ecs_vec_fini_t(NULL, &lifecycle_ctx->vctor, ecs_rtt_call_data_t);
    ecs_vec_fini_t(NULL, &lifecycle_ctx->vdtor, ecs_rtt_call_data_t);
    ecs_vec_fini_t(NULL, &lifecycle_ctx->vmove, ecs_rtt_call_data_t);
    ecs_vec_fini_t(NULL, &lifecycle_ctx->vcopy, ecs_rtt_call_data_t);
    ecs_vec_fini_t(NULL, &lifecycle_ctx->vcmp, ecs_rtt_call_data_t);
    ecs_vec_fini_t(NULL, &lifecycle_ctx->vequals, ecs_rtt_call_data_t);

    ecs_os_free(ctx);
}

/* Configure generic forwarding hooks for a runtime struct type. Allocates
 * and returns a lifecycle context that will hold per-member call data. */
static
ecs_rtt_struct_ctx_t * flecs_rtt_configure_struct_hooks(
    ecs_world_t *world,
    const ecs_type_info_t *ti,
    ecs_flags32_t flags,
    bool ctor,
    bool dtor,
    bool move,
    bool copy,
    bool cmp,
    bool equals)
{
    ecs_type_hooks_t hooks = ti->hooks;
    if (hooks.lifecycle_ctx_free) {
        hooks.lifecycle_ctx_free(hooks.lifecycle_ctx);
    }

    hooks.ctor = ctor && !(flags & ECS_TYPE_HOOK_CTOR_ILLEGAL) ? 
        flecs_rtt_struct_ctor : NULL;

    hooks.dtor = dtor && !(flags & ECS_TYPE_HOOK_DTOR_ILLEGAL) ? 
        flecs_rtt_struct_dtor : NULL;
    
    hooks.move = move && !(flags & ECS_TYPE_HOOK_MOVE_ILLEGAL) ? 
        flecs_rtt_struct_move : NULL;

    hooks.copy = copy && !(flags & ECS_TYPE_HOOK_COPY_ILLEGAL) ? 
        flecs_rtt_struct_copy : NULL;
    
    hooks.cmp = cmp && !(flags & ECS_TYPE_HOOK_CMP_ILLEGAL) ? 
        flecs_rtt_struct_cmp : NULL;
    
    hooks.equals = equals && !(flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL) ? 
        flecs_rtt_struct_equals : NULL;

    ecs_rtt_struct_ctx_t *rtt_ctx = NULL;
    if (hooks.ctor || hooks.dtor || hooks.move || hooks.copy 
        || hooks.cmp || hooks.equals) {
        rtt_ctx = ecs_os_malloc_t(ecs_rtt_struct_ctx_t);
        ecs_vec_init_t(NULL, &rtt_ctx->vctor, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vdtor, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vmove, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vcopy, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vcmp, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vequals, ecs_rtt_call_data_t, 0);

        hooks.lifecycle_ctx = rtt_ctx;
        hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_struct_ctx;
    } else {
        hooks.lifecycle_ctx = NULL;
        hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_nop;
    }

    hooks.flags = flags;
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, ti->component, &hooks);
    return rtt_ctx;
}

/* Scan struct members to determine which hooks are needed, then configure
 * generic forwarding hooks and build per-member call data vectors. */
static
void flecs_rtt_init_default_hooks_struct(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_type_info_t *ti)
{
    /* Inspect struct members to determine which hooks are needed */
    const EcsStruct *struct_info = ecs_get(world, component, EcsStruct);
    ecs_assert(struct_info != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Determine per-hook whether any member requires forwarding */
    bool ctor_hook_required = false;
    bool dtor_hook_required = false;
    bool move_hook_required = false;
    bool copy_hook_required = false;
    bool valid_cmp = true;
    bool valid_equals = true;

    /* Check each member type for hooks; the struct needs a hook if any member does */
    int i, member_count = ecs_vec_count(&struct_info->members);
    ecs_member_t *members = ecs_vec_first(&struct_info->members);
    ecs_flags32_t flags = 0;
    for (i = 0; i < member_count; i++) {
        ecs_member_t *m = &members[i];
        const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
        if (!member_ti || member_ti == ti) {
            continue;
        }
        ctor_hook_required |= member_ti->hooks.ctor &&
                              member_ti->hooks.ctor != flecs_default_ctor;
        dtor_hook_required |= member_ti->hooks.dtor != NULL;
        move_hook_required |= member_ti->hooks.move != NULL;
        copy_hook_required |= member_ti->hooks.copy != NULL;
        /* Struct cmp/equals is only valid if all members support it */
        valid_cmp &= member_ti->hooks.cmp != NULL;
        valid_equals  &= member_ti->hooks.equals != NULL;
        flags |= member_ti->hooks.flags;
    }

    /* Create lifecycle context and configure generic forwarding hooks */
    ecs_rtt_struct_ctx_t *rtt_ctx = flecs_rtt_configure_struct_hooks(
        world,
        ti,
        flags,
        ctor_hook_required,
        dtor_hook_required,
        move_hook_required,
        copy_hook_required,
        valid_cmp,
        valid_equals
        );

    if (!rtt_ctx) {
        return; /* no hook forwarding required */
    }

    /* Build per-member call data vectors for each configured hook */
    for (i = 0; i < member_count; i++) {
        ecs_member_t *m = &members[i];
        const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
        if (!member_ti || member_ti == ti) {
            continue;
        }
        if (ctor_hook_required) {
            ecs_rtt_call_data_t *ctor_data =
                ecs_vec_append_t(NULL, &rtt_ctx->vctor, ecs_rtt_call_data_t);
            ctor_data->count = m->count ? m->count : 1;
            ctor_data->offset = m->offset;
            ctor_data->type_info = member_ti;
            if (member_ti->hooks.ctor) {
                ctor_data->hook.xtor = member_ti->hooks.ctor;
            } else {
                ctor_data->hook.xtor = flecs_default_ctor;
            }
        }
        if (dtor_hook_required && member_ti->hooks.dtor) {
            ecs_rtt_call_data_t *dtor_data =
                ecs_vec_append_t(NULL, &rtt_ctx->vdtor, ecs_rtt_call_data_t);
            dtor_data->count = m->count ? m->count : 1;
            dtor_data->offset = m->offset;
            dtor_data->type_info = member_ti;
            dtor_data->hook.xtor = member_ti->hooks.dtor;
        }
        if (move_hook_required) {
            ecs_rtt_call_data_t *move_data =
                ecs_vec_append_t(NULL, &rtt_ctx->vmove, ecs_rtt_call_data_t);
            move_data->offset = m->offset;
            move_data->type_info = member_ti;
            move_data->count = m->count ? m->count : 1;
            if (member_ti->hooks.move) {
                move_data->hook.move = member_ti->hooks.move;
            } else {
                move_data->hook.move = flecs_rtt_default_move;
            }
        }
        if (copy_hook_required) {
            ecs_rtt_call_data_t *copy_data =
                ecs_vec_append_t(NULL, &rtt_ctx->vcopy, ecs_rtt_call_data_t);
            copy_data->offset = m->offset;
            copy_data->type_info = member_ti;
            copy_data->count = m->count ? m->count : 1;
            if (member_ti->hooks.copy) {
                copy_data->hook.copy = member_ti->hooks.copy;
            } else {
                copy_data->hook.copy = flecs_rtt_default_copy;
            }
        }
        if (valid_cmp) {
            ecs_rtt_call_data_t *comp_data =
            ecs_vec_append_t(NULL, &rtt_ctx->vcmp, ecs_rtt_call_data_t);
            comp_data->offset = m->offset;
            comp_data->type_info = member_ti;
            comp_data->count = 1;
            ecs_assert(member_ti->hooks.cmp, ECS_INTERNAL_ERROR, NULL);
            comp_data->hook.cmp = member_ti->hooks.cmp; 
        }
        if (valid_equals) {
            ecs_rtt_call_data_t *comp_data =
            ecs_vec_append_t(NULL, &rtt_ctx->vequals, ecs_rtt_call_data_t);
            comp_data->offset = m->offset;
            comp_data->type_info = member_ti;
            comp_data->count = 1;
            ecs_assert(member_ti->hooks.equals, ECS_INTERNAL_ERROR, NULL);
            comp_data->hook.equals = member_ti->hooks.equals; 
        }
    }
}

/* -- RTT array support -- */

/* Free the lifecycle context for a runtime array type. */
static
void flecs_rtt_free_lifecycle_array_ctx(
    void *ctx)
{
    if (!ctx) {
        return;
    }

    ecs_os_free(ctx);
}

/* RTT array constructor. count = number of array instances, not elements. */
static
void flecs_rtt_array_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        void *arr = ECS_ELEM(ptr, type_info->size, i);
        flecs_type_info_ctor(arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* RTT array destructor. count = number of array instances, not elements. */
static
void flecs_rtt_array_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        void *arr = ECS_ELEM(ptr, type_info->size, i);
        flecs_type_info_dtor(arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* RTT array move. count = number of array instances, not elements. */
static
void flecs_rtt_array_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        void *src_arr = ECS_ELEM(src_ptr, type_info->size, i);
        void *dst_arr = ECS_ELEM(dst_ptr, type_info->size, i);
        flecs_type_info_move(
            dst_arr, src_arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* RTT array copy. count = number of array instances, not elements. */
static
void flecs_rtt_array_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        const void *src_arr = ECS_ELEM(src_ptr, type_info->size, i);
        void *dst_arr = ECS_ELEM(dst_ptr, type_info->size, i);
        flecs_type_info_copy(
            dst_arr, src_arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* RTT array compare: lexicographic comparison of all elements. */
static
int flecs_rtt_array_cmp(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if(a_ptr == b_ptr) {
        return 0;
    }

    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_size_t element_size = rtt_ctx->type_info->size;
    int i;
    for (i = 0; i < rtt_ctx->elem_count; i++) {
        const void *a_element = ECS_ELEM(a_ptr, element_size, i);
        const void *b_element = ECS_ELEM(b_ptr, element_size, i);
        int c = flecs_type_info_cmp(a_element, b_element, rtt_ctx->type_info);
        if(c != 0) {
            return c;
        }
    }
    return 0;
}

/* RTT array equals: element-wise equality check. */
static
bool flecs_rtt_array_equals(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *type_info)
{
    if(a_ptr == b_ptr) {
        return true;
    }

    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_size_t element_size = rtt_ctx->type_info->size;
    int i;
    for (i = 0; i < rtt_ctx->elem_count; i++) {
        const void *a_element = ECS_ELEM(a_ptr, element_size, i);
        const void *b_element = ECS_ELEM(b_ptr, element_size, i);
        bool eq = flecs_type_info_equals(
            a_element, b_element, rtt_ctx->type_info);
        if(!eq) {
            return false;
        }
    }
    return true;
}

/* Configure RTT array hooks based on element type capabilities. */
static
void flecs_rtt_init_default_hooks_array(
    ecs_world_t *world, 
    ecs_entity_t component)
{
    const EcsArray *array_info = ecs_get(world, component, EcsArray);
    ecs_assert(array_info != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *element_ti =
        ecs_get_type_info(world, array_info->type);
    ecs_flags32_t flags = element_ti->hooks.flags;
    bool ctor_hook_required =
        element_ti->hooks.ctor && element_ti->hooks.ctor != flecs_default_ctor;
    bool dtor_hook_required = element_ti->hooks.dtor != NULL;
    bool move_hook_required = element_ti->hooks.move != NULL;
    bool copy_hook_required = element_ti->hooks.copy != NULL;
    bool valid_cmp = element_ti->hooks.cmp != NULL && !(flags & ECS_TYPE_HOOK_CMP_ILLEGAL);
    bool valid_equals = element_ti->hooks.equals != NULL && !(flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL);
    

    ecs_type_hooks_t hooks = *ecs_get_hooks_id(world, component);
    
    hooks.ctor = ctor_hook_required && !(flags & ECS_TYPE_HOOK_CTOR_ILLEGAL) ? 
        flecs_rtt_array_ctor : NULL;
    hooks.dtor = dtor_hook_required && !(flags & ECS_TYPE_HOOK_DTOR_ILLEGAL) ? 
        flecs_rtt_array_dtor : NULL;
    hooks.move = move_hook_required && !(flags & ECS_TYPE_HOOK_MOVE_ILLEGAL) ? 
        flecs_rtt_array_move : NULL;
    hooks.copy = copy_hook_required && !(flags & ECS_TYPE_HOOK_COPY_ILLEGAL) ? 
        flecs_rtt_array_copy : NULL;
    hooks.cmp = valid_cmp && !(flags & ECS_TYPE_HOOK_CMP_ILLEGAL) ? 
        flecs_rtt_array_cmp : NULL;
    hooks.equals = valid_equals && !(flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL) ? 
        flecs_rtt_array_equals : NULL;


    if (hooks.lifecycle_ctx_free) {
        hooks.lifecycle_ctx_free(hooks.lifecycle_ctx);
        hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_nop;
    }

    if (hooks.ctor || hooks.dtor || hooks.move ||
        hooks.copy || hooks.cmp || hooks.equals) 
    {
        ecs_rtt_array_ctx_t *rtt_ctx = ecs_os_malloc_t(ecs_rtt_array_ctx_t);
        rtt_ctx->type_info = element_ti;
        rtt_ctx->elem_count = array_info->count;
        if (hooks.lifecycle_ctx_free) {
            hooks.lifecycle_ctx_free(hooks.lifecycle_ctx);
        }

        hooks.lifecycle_ctx = rtt_ctx;
        hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_array_ctx;
    }

    hooks.flags = flags;
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, component, &hooks);
}

/* -- RTT vector support -- */

/* Free the lifecycle context for a runtime vector type. */
static
void flecs_rtt_free_lifecycle_vector_ctx(
    void *ctx)
{
    if (!ctx) {
        return;
    }

    ecs_os_free(ctx);
}

/* RTT vector constructor: initializes each ecs_vec_t to empty. */
static
void flecs_rtt_vector_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_vector_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        ecs_vec_t *vec = ECS_ELEM(ptr, type_info->size, i);
        ecs_vec_init(NULL, vec, rtt_ctx->type_info->size, 0);
    }
}

/* RTT vector destructor: destroys elements then frees the ecs_vec_t. */
static
void flecs_rtt_vector_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_vector_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    int i;
    for (i = 0; i < count; i++) {
        ecs_vec_t *vec = ECS_ELEM(ptr, type_info->size, i);
        int32_t num_elements = ecs_vec_count(vec);
        if (rtt_ctx->type_info->hooks.dtor && num_elements) {
            flecs_type_info_dtor(
                ecs_vec_first(vec), num_elements, rtt_ctx->type_info);
        }
        ecs_vec_fini(NULL, vec, rtt_ctx->type_info->size);
    }
}

/* RTT vector move: destroys dst, then steals src's array pointer. */
static
void flecs_rtt_vector_move(
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

/* RTT vector copy: deep-copies vector contents via element hooks. */
static
void flecs_rtt_vector_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_vector_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    flecs_rtt_vector_dtor(dst_ptr, count, type_info);
    bool has_copy = rtt_ctx->type_info->hooks.copy != NULL;
    bool has_ctor = rtt_ctx->type_info->hooks.ctor != NULL;
    bool has_dtor = rtt_ctx->type_info->hooks.dtor != NULL;
    int i;
    for (i = 0; i < count; i++) {
        const ecs_vec_t *src_vec = ECS_ELEM(src_ptr, type_info->size, i);
        ecs_vec_t *dst_vec = ECS_ELEM(dst_ptr, type_info->size, i);
        ecs_vec_init_if(dst_vec, rtt_ctx->type_info->size);
        int32_t src_count = ecs_vec_count(src_vec);
        int32_t dst_count = ecs_vec_count(dst_vec);
        if (has_dtor && dst_count) {
            flecs_type_info_dtor(
                ecs_vec_first(dst_vec), dst_count, rtt_ctx->type_info);
        }
        ecs_vec_set_count(NULL, dst_vec, rtt_ctx->type_info->size, src_count);
        if (has_ctor) {
            flecs_type_info_ctor(
                ecs_vec_first(dst_vec), src_count, rtt_ctx->type_info);
        } else {
            flecs_default_ctor(
                ecs_vec_first(dst_vec), src_count, rtt_ctx->type_info);
        }
        if (has_copy) {
            flecs_type_info_copy(
                ecs_vec_first(dst_vec),
                ecs_vec_first(src_vec),
                src_count,
                rtt_ctx->type_info);
        } else {
            flecs_rtt_default_copy(
                ecs_vec_first(dst_vec),
                ecs_vec_first(src_vec),
                src_count,
                rtt_ctx->type_info);
        }
    }
}

/* RTT vector compare: length-first, then element-wise comparison. */
static
int flecs_rtt_vector_cmp(
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

    ecs_rtt_vector_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;

    ecs_size_t element_size = rtt_ctx->type_info->size;
    const void *a = ecs_vec_first(vec_a);
    const void *b = ecs_vec_first(vec_b);

    int i;
    for (i = 0; i < count_a; i++) {
        const void *a_element = ECS_ELEM(a, element_size, i);
        const void *b_element = ECS_ELEM(b, element_size, i);
        int c = flecs_type_info_cmp(a_element, b_element, rtt_ctx->type_info);
        if(c != 0) {
            return c;
        }
    }
    return 0;
}

/* RTT vector equals: length check then element-wise equality. */
static
bool flecs_rtt_vector_equals(
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
    {
        int c = count_a - count_b;
        if(c != 0) {
            return c;
        }
    }

    ecs_rtt_vector_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;

    ecs_size_t element_size = rtt_ctx->type_info->size;
    const void *a = ecs_vec_first(vec_a);
    const void *b = ecs_vec_first(vec_b);

    int i;
    for (i = 0; i < count_a; i++) {
        const void *a_element = ECS_ELEM(a, element_size, i);
        const void *b_element = ECS_ELEM(b, element_size, i);
        bool eq = flecs_type_info_equals(
            a_element, b_element, rtt_ctx->type_info);
        if(!eq) {
            return false;
        }
    }
    return true;
}

/* Configure RTT vector hooks. Vectors always need ctor/dtor/move/copy
 * for ecs_vec_t lifecycle; cmp/equals depend on element type. */
static
void flecs_rtt_init_default_hooks_vector(
    ecs_world_t *world,
    ecs_entity_t component)
{
    const EcsVector *vector_info = ecs_get(world, component, EcsVector);
    ecs_assert(vector_info != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!ecs_is_alive(world, vector_info->type)) {
        ecs_err("vector '%s' has invalid element type", ecs_get_name(world, component));
        return;
    }

    const ecs_type_info_t *element_ti =
        ecs_get_type_info(world, vector_info->type);
    if (!element_ti) {
        ecs_err("vector '%s' has invalid element type", ecs_get_name(world, component));
        return;
    }

    ecs_rtt_vector_ctx_t *rtt_ctx = ecs_os_malloc_t(ecs_rtt_vector_ctx_t);
    rtt_ctx->type_info = element_ti;
    ecs_flags32_t flags = element_ti->hooks.flags;

    ecs_type_hooks_t hooks = *ecs_get_hooks_id(world, component);
    
    if (hooks.lifecycle_ctx_free) {
        hooks.lifecycle_ctx_free(hooks.lifecycle_ctx);
    }
    hooks.lifecycle_ctx = rtt_ctx;
    hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_vector_ctx;

    hooks.ctor = flecs_rtt_vector_ctor;
    hooks.dtor = flecs_rtt_vector_dtor;
    hooks.move = flecs_rtt_vector_move;
    hooks.copy = flecs_rtt_vector_copy;
    
    if (element_ti->hooks.cmp != NULL && !(flags & ECS_TYPE_HOOK_CMP_ILLEGAL)) {
        hooks.cmp = flecs_rtt_vector_cmp;
    } else {
        hooks.cmp = NULL;
    }

    if (element_ti->hooks.equals != NULL && !(flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL)) {
        hooks.equals = flecs_rtt_vector_equals;
    } else {
        hooks.equals = NULL;
    }

    /* Propagate compare/equals illegal flags from element type */
    hooks.flags |= flags & (ECS_TYPE_HOOK_CMP_ILLEGAL|ECS_TYPE_HOOK_EQUALS_ILLEGAL);
    
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, component, &hooks);
}

/* Observer callback: initialize RTT hooks when EcsType is set on a
 * non-existing (runtime-defined) component. */
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

        /* RTT types need auto-generated hooks: structs forward to member
         * hooks, arrays forward to element hooks, vectors manage the
         * ecs_vec_t lifecycle. Trivial types get a zero-init ctor. */

        ecs_entity_t component = it->entities[i];

        /* Skip configuring hooks for ids already in use */
        const ecs_world_t* w = ecs_get_world(world);
        if(ecs_id_in_use(w, component) || 
            ecs_id_in_use(w, ecs_pair(component, EcsWildcard))) {
            continue;
        } 

        const ecs_type_info_t *ti = ecs_get_type_info(world, component);
        ecs_assert(ti,ECS_INTERNAL_ERROR,NULL);

        if (type->kind == EcsStructType) {
            flecs_rtt_init_default_hooks_struct(world, component, ti);
        } else if (type->kind == EcsArrayType) {
            flecs_rtt_init_default_hooks_array(world, component);
        } else if (type->kind == EcsVectorType) {
            flecs_rtt_init_default_hooks_vector(world, component);
        }

        ecs_type_hooks_t hooks = ti->hooks;
        /* Ensure at least a zero-init ctor to prevent reading
         * uninitialized memory (e.g. dangling string pointers). */
        if(!ti->hooks.ctor && !(ti->hooks.flags & ECS_TYPE_HOOK_CTOR_ILLEGAL)) {
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
