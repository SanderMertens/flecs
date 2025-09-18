/*
 * @file addons/meta/rtt_lifecycle.c
 * @brief Runtime components lifecycle management
 */

#include "flecs.h"
#include "meta.h"

#ifdef FLECS_META

/* Stores all the information necessary to forward a hook call to a
 * struct's member type */
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

/* Lifecycle context for runtime structs */
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

/* Generic copy assign hook */
static
void flecs_rtt_default_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_os_memcpy(dst_ptr, src_ptr, count * type_info->size);
}

/* Generic move assign hook */
static
void flecs_rtt_default_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    flecs_rtt_default_copy(dst_ptr, src_ptr, count, type_info);
}

/*
 *
 * RTT struct support
 *
 */

/* Invokes struct member type's constructor/destructor using saved information
 * in the lifecycle context */
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

/* Generic struct constructor. It will read hook information call data from
 * the structs's lifecycle context and call the constructors configured when
 * the type was created. */
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

/* Generic struct destructor. It will read hook information call data from
 * the structs's lifecycle context and call the constructors configured when
 * the type was created. */
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

/* Generic move hook. It will read hook information call data from the
 * structs's lifecycle context and call the move hooks configured when
 * the type was created. */
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

/* Generic copy hook. It will read hook information call data from the
 * structs's lifecycle context and call the copy hooks configured when
 * the type was created. */
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

/* Generic compare hook. It will read hook information call data from the
 * structs's lifecycle context and call the compare hooks configured when
 * the type was created. */
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

/* Generic equals hook. It will read hook information call data from the
 * structs's lifecycle context and call the equals hooks configured when
 * the type was created. */
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

static
void flecs_rtt_free_lifecycle_nop(
    void *ctx)
{
    (void)ctx;
}

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

/* Checks if a struct member's types have hooks installed. If so, it generates
 * and installs required hooks for the struct type itself. These hooks will
 * invoke the member hooks when necessary  */
static
void flecs_rtt_init_default_hooks_struct(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_type_info_t *ti)
{
    /* Obtain struct information to figure out what members it contains: */
    const EcsStruct *struct_info = ecs_get(world, component, EcsStruct);
    ecs_assert(struct_info != NULL, ECS_INTERNAL_ERROR, NULL);

    /* These flags will be set to true if we determine we need to generate a
     * hook of a particular type: */
    bool ctor_hook_required = false;
    bool dtor_hook_required = false;
    bool move_hook_required = false;
    bool copy_hook_required = false;
    bool valid_cmp = true;
    bool valid_equals = true;

    /* Iterate all struct members and see if any member type has hooks. If so,
     * the struct itself will need to have that hook: */
    int i, member_count = ecs_vec_count(&struct_info->members);
    ecs_member_t *members = ecs_vec_first(&struct_info->members);
    ecs_flags32_t flags = 0;
    for (i = 0; i < member_count; i++) {
        ecs_member_t *m = &members[i];
        const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
        ctor_hook_required |= member_ti->hooks.ctor &&
                              member_ti->hooks.ctor != flecs_default_ctor;
        dtor_hook_required |= member_ti->hooks.dtor != NULL;
        move_hook_required |= member_ti->hooks.move != NULL;
        copy_hook_required |= member_ti->hooks.copy != NULL;
        /* A struct has a valid cmp/equals hook if all its members have it: */
        valid_cmp &= member_ti->hooks.cmp != NULL;
        valid_equals  &= member_ti->hooks.equals != NULL;
        flags |= member_ti->hooks.flags;
    }

    /* If any hook is required, then create a lifecycle context and configure a
     * generic hook that will interpret that context: */
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

    /* At least a hook was configured, therefore examine each struct member to
     * build the vector of calls that will then be executed by the generic hook
     * handler: */
    for (i = 0; i < member_count; i++) {
        ecs_member_t *m = &members[i];
        const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
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

/*
 *
 * RTT array support
 *
 */

static
void flecs_rtt_free_lifecycle_array_ctx(
    void *ctx)
{
    if (!ctx) {
        return;
    }

    ecs_os_free(ctx);
}

/* Generic array constructor. It will invoke the constructor of the underlying
 * type for all the elements */
static
void flecs_rtt_array_ctor(
    void *ptr,
    int32_t count, /* note: "count" is how many arrays to initialize, not how
                      many elements are in the array */
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_xtor_t ctor = rtt_ctx->type_info->hooks.ctor;
    int i;
    for (i = 0; i < count; i++) {
        void *arr = ECS_ELEM(ptr, type_info->size, i);
        ctor(arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* Generic array constructor. It will invoke the destructor of the underlying
 * type for all the elements */
static
void flecs_rtt_array_dtor(
    void *ptr,
    int32_t count, /* note: "count" is how many arrays to destroy, not how
                      many elements are in the array */
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_xtor_t dtor = rtt_ctx->type_info->hooks.dtor;
    int i;
    for (i = 0; i < count; i++) {
        void *arr = ECS_ELEM(ptr, type_info->size, i);
        dtor(arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* Generic array move hook. It will invoke the move hook of the underlying
 * type for all the elements */
static
void flecs_rtt_array_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count, /* note: "count" is how many arrays to move, not how
                      many elements are in the array */
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_move_t move = rtt_ctx->type_info->hooks.move;
    int i;
    for (i = 0; i < count; i++) {
        void *src_arr = ECS_ELEM(src_ptr, type_info->size, i);
        void *dst_arr = ECS_ELEM(dst_ptr, type_info->size, i);
        move(dst_arr, src_arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* Generic array copy hook. It will invoke the copy hook of the underlying
 * type for all the elements */
static
void flecs_rtt_array_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count, /* note: "count" is how many arrays to copy, not how
                      many elements are in the array */
    const ecs_type_info_t *type_info)
{
    ecs_rtt_array_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_copy_t copy = rtt_ctx->type_info->hooks.copy;
    int i;
    for (i = 0; i < count; i++) {
        const void *src_arr = ECS_ELEM(src_ptr, type_info->size, i);
        void *dst_arr = ECS_ELEM(dst_ptr, type_info->size, i);
        copy(dst_arr, src_arr, rtt_ctx->elem_count, rtt_ctx->type_info);
    }
}

/* Generic array compare hook. It will invoke the compare hook of the underlying
 * type for each element */
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
    ecs_cmp_t cmp = rtt_ctx->type_info->hooks.cmp;
    ecs_assert(cmp,  ECS_INVALID_PARAMETER, NULL);
    ecs_size_t element_size = rtt_ctx->type_info->size;
    int i;
    for (i = 0; i < rtt_ctx->elem_count; i++) {
        const void *a_element = ECS_ELEM(a_ptr, element_size, i);
        const void *b_element = ECS_ELEM(b_ptr, element_size, i);
        int c = cmp(a_element, b_element, rtt_ctx->type_info);
        if(c != 0) {
            return c;
        }
    }
    return 0;
}

/* Generic array equals hook. It will invoke the equals hook of the underlying
 * type for each element */
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
    ecs_equals_t equals = rtt_ctx->type_info->hooks.equals;
    ecs_assert(equals,  ECS_INVALID_PARAMETER, NULL);
    ecs_size_t element_size = rtt_ctx->type_info->size;
    int i;
    for (i = 0; i < rtt_ctx->elem_count; i++) {
        const void *a_element = ECS_ELEM(a_ptr, element_size, i);
        const void *b_element = ECS_ELEM(b_ptr, element_size, i);
        bool eq = equals(a_element, b_element, rtt_ctx->type_info);
        if(!eq) {
            return false;
        }
    }
    return true;
}

/* Checks if an array's underlying type has hooks installed. If so, it generates
 * and installs required hooks for the array type itself. These hooks will
 * invoke the underlying type's hook for each element in the array. */
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

/*
 *
 * RTT vector support
 *
 */

static
void flecs_rtt_free_lifecycle_vector_ctx(
    void *ctx)
{
    if (!ctx) {
        return;
    }

    ecs_os_free(ctx);
}

/* Generic vector constructor. Makes sure the vector structure is initialized to
 * 0 elements */
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

/* Generic vector destructor. It will invoke the destructor for each element of
 * the vector and finalize resources associated to the vector itself. */
static
void flecs_rtt_vector_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_vector_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    ecs_xtor_t dtor = rtt_ctx->type_info->hooks.dtor;
    int i;
    for (i = 0; i < count; i++) {
        ecs_vec_t *vec = ECS_ELEM(ptr, type_info->size, i);
        int32_t num_elements = ecs_vec_count(vec);
        if (dtor && num_elements) {
            dtor(ecs_vec_first(vec), num_elements, rtt_ctx->type_info);
        }
        ecs_vec_fini(NULL, vec, rtt_ctx->type_info->size);
    }
}

/* Generic vector move hook. */
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

/* Generic vector copy hook. It makes a deep copy of vector contents */
static
void flecs_rtt_vector_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ecs_rtt_vector_ctx_t *rtt_ctx = type_info->hooks.lifecycle_ctx;
    flecs_rtt_vector_dtor(dst_ptr, count, type_info);
    ecs_copy_t copy = rtt_ctx->type_info->hooks.copy
                          ? rtt_ctx->type_info->hooks.copy
                          : flecs_rtt_default_copy;
    ecs_xtor_t ctor = rtt_ctx->type_info->hooks.ctor
                          ? rtt_ctx->type_info->hooks.ctor
                          : flecs_default_ctor;
    ecs_xtor_t dtor = rtt_ctx->type_info->hooks.dtor;
    int i;
    for (i = 0; i < count; i++) {
        const ecs_vec_t *src_vec = ECS_ELEM(src_ptr, type_info->size, i);
        ecs_vec_t *dst_vec = ECS_ELEM(dst_ptr, type_info->size, i);
        ecs_vec_init_if(dst_vec, rtt_ctx->type_info->size);
        int32_t src_count = ecs_vec_count(src_vec);
        int32_t dst_count = ecs_vec_count(dst_vec);
        if (dtor && dst_count) {
            dtor(ecs_vec_first(dst_vec), dst_count, rtt_ctx->type_info);
        }
        ecs_vec_set_count(NULL, dst_vec, rtt_ctx->type_info->size, src_count);
        ctor(ecs_vec_first(dst_vec), src_count, rtt_ctx->type_info);
        copy(
            ecs_vec_first(dst_vec),
            ecs_vec_first(src_vec),
            src_count,
            rtt_ctx->type_info);
    }
}

/* Generic vector compare hook. */
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
    ecs_cmp_t cmp = rtt_ctx->type_info->hooks.cmp;
    ecs_assert(cmp,  ECS_INVALID_PARAMETER, NULL);

    ecs_size_t element_size = rtt_ctx->type_info->size;
    const void *a = ecs_vec_first(vec_a);
    const void *b = ecs_vec_first(vec_b);

    int i;
    for (i = 0; i < count_a; i++) {
        const void *a_element = ECS_ELEM(a, element_size, i);
        const void *b_element = ECS_ELEM(b, element_size, i);
        int c = cmp(a_element, b_element, rtt_ctx->type_info);
        if(c != 0) {
            return c;
        }
    }
    return 0;
}

/* Generic vector equals hook. */
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
    ecs_equals_t equals = rtt_ctx->type_info->hooks.equals;
    ecs_assert(equals,  ECS_INVALID_PARAMETER, NULL);

    ecs_size_t element_size = rtt_ctx->type_info->size;
    const void *a = ecs_vec_first(vec_a);
    const void *b = ecs_vec_first(vec_b);

    int i;
    for (i = 0; i < count_a; i++) {
        const void *a_element = ECS_ELEM(a, element_size, i);
        const void *b_element = ECS_ELEM(b, element_size, i);
        int eq = equals(a_element, b_element, rtt_ctx->type_info);
        if(!eq) {
            return false;
        }
    }
    return true;
}

/* Generates and installs required hooks for managing the vector and underlying
 * type lifecycle. Vectors always have hooks because at the very least the
 * vector structure itself must be initialized/destroyed/copied/moved, even if
 * empty. */
static
void flecs_rtt_init_default_hooks_vector(
    ecs_world_t *world,
    ecs_entity_t component)
{
    const EcsVector *vector_info = ecs_get(world, component, EcsVector);
    ecs_assert(vector_info != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *element_ti =
        ecs_get_type_info(world, vector_info->type);
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


    /* propagate only the compare/equals hook illegal flag, if set */
    hooks.flags |= flags & (ECS_TYPE_HOOK_CMP_ILLEGAL|ECS_TYPE_HOOK_EQUALS_ILLEGAL);
    
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, component, &hooks);
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
         * zero-initialized
         * - For struct types, configure a hook that in turn calls hooks of
         * member types, if those member types have hooks defined themselves.
         * - For array types, configure a hook that in turn calls hooks for the
         * underlying type, for each element in the array.
         *  - For vector types, configure hooks to manage the vector structure
         * itself, move the vector and deep-copy vector elements
         * */

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
        /* Make sure there is at least a default constructor. This ensures that
         * a new component value does not contain uninitialized memory, which
         * could cause serializers to crash when for example inspecting string
         * fields. */
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
