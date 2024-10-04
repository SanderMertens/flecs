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
    } hook;
    const ecs_type_info_t *type_info;
    int32_t offset;
    int32_t count;
} ecs_rtt_call_data_t;

/* Lifecycle context for runtime structs */
typedef struct ecs_rtt_struct_ctx_t {
    ecs_vec_t vctor; /* vector<ecs_string_initializertt_call_data_t> */
    ecs_vec_t vdtor; /* vector<ecs_rtt_call_data_t> */
    ecs_vec_t vmove; /* vector<ecs_rtt_call_data_t> */
    ecs_vec_t vcopy; /* vector<ecs_rtt_call_data_t> */
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

    ecs_os_free(ctx);
}

static
ecs_rtt_struct_ctx_t * flecs_rtt_configure_struct_hooks(
    ecs_world_t *world,
    const ecs_type_info_t *ti,
    bool ctor,
    bool dtor,
    bool move,
    bool copy)
{
    ecs_type_hooks_t hooks = ti->hooks;
    if (hooks.lifecycle_ctx_free) {
        hooks.lifecycle_ctx_free(hooks.lifecycle_ctx);
    }

    ecs_rtt_struct_ctx_t *rtt_ctx = NULL;
    if (ctor || dtor || move || copy) {
        rtt_ctx = ecs_os_malloc_t(ecs_rtt_struct_ctx_t);
        ecs_vec_init_t(NULL, &rtt_ctx->vctor, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vdtor, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vmove, ecs_rtt_call_data_t, 0);
        ecs_vec_init_t(NULL, &rtt_ctx->vcopy, ecs_rtt_call_data_t, 0);
        hooks.lifecycle_ctx = rtt_ctx;
        hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_struct_ctx;

        if (ctor) {
            hooks.ctor = flecs_rtt_struct_ctor;
        }
        if (dtor) {
            hooks.dtor = flecs_rtt_struct_dtor;
        }
        if (move) {
            hooks.move = flecs_rtt_struct_move;
        }
        if (copy) {
            hooks.copy = flecs_rtt_struct_copy;
        }
    } else {
        hooks.lifecycle_ctx = NULL;
        hooks.lifecycle_ctx_free = NULL;
    }
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

    /* Iterate all struct members and see if any member type has hooks. If so,
     * the struct itself will need to have that hook: */
    int i, member_count = ecs_vec_count(&struct_info->members);
    ecs_member_t *members = ecs_vec_first(&struct_info->members);
    for (i = 0; i < member_count; i++) {
        ecs_member_t *m = &members[i];
        const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
        ctor_hook_required |= member_ti->hooks.ctor &&
                              member_ti->hooks.ctor != flecs_default_ctor;
        dtor_hook_required |= member_ti->hooks.dtor != NULL;
        move_hook_required |= member_ti->hooks.move != NULL;
        copy_hook_required |= member_ti->hooks.copy != NULL;
    }

    /* If any hook is required, then create a lifecycle context and configure a
     * generic hook that will interpret that context: */
    ecs_rtt_struct_ctx_t *rtt_ctx = flecs_rtt_configure_struct_hooks(
        world,
        ti,
        ctor_hook_required,
        dtor_hook_required,
        move_hook_required,
        copy_hook_required);

    if (!rtt_ctx) {
        return; /* no hooks required */
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
            ctor_data->count = m->count;
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
            dtor_data->count = m->count;
            dtor_data->offset = m->offset;
            dtor_data->type_info = member_ti;
            dtor_data->hook.xtor = member_ti->hooks.dtor;
        }
        if (move_hook_required) {
            ecs_rtt_call_data_t *move_data =
                ecs_vec_append_t(NULL, &rtt_ctx->vmove, ecs_rtt_call_data_t);
            move_data->offset = m->offset;
            move_data->type_info = member_ti;
            move_data->count = m->count;
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
            copy_data->count = m->count;
            if (member_ti->hooks.copy) {
                copy_data->hook.copy = member_ti->hooks.copy;
            } else {
                copy_data->hook.copy = flecs_rtt_default_copy;
            }
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
    const ecs_type_info_t *array_ti =
        ecs_get_type_info(world, array_info->type);
    bool ctor_hook_required =
        array_ti->hooks.ctor && array_ti->hooks.ctor != flecs_default_ctor;
    bool dtor_hook_required = array_ti->hooks.dtor != NULL;
    bool move_hook_required = array_ti->hooks.move != NULL;
    bool copy_hook_required = array_ti->hooks.copy != NULL;

    if (!ctor_hook_required && !dtor_hook_required && !move_hook_required &&
        !copy_hook_required) {
        return; /* no hooks required */
    }

    ecs_rtt_array_ctx_t *rtt_ctx = ecs_os_malloc_t(ecs_rtt_array_ctx_t);
    rtt_ctx->type_info = array_ti;
    rtt_ctx->elem_count = array_info->count;
    ecs_type_hooks_t hooks = *ecs_get_hooks_id(world, component);
    if (hooks.lifecycle_ctx_free) {
        hooks.lifecycle_ctx_free(hooks.lifecycle_ctx);
    }

    hooks.lifecycle_ctx = rtt_ctx;
    hooks.lifecycle_ctx_free = flecs_rtt_free_lifecycle_array_ctx;

    if (ctor_hook_required) {
        hooks.ctor = flecs_rtt_array_ctor;
    }

    if (dtor_hook_required) {
        hooks.dtor = flecs_rtt_array_dtor;
    }

    if (move_hook_required) {
        hooks.move = flecs_rtt_array_move;
    }

    if (copy_hook_required) {
        hooks.copy = flecs_rtt_array_copy;
    }

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
    const ecs_type_info_t *vector_ti =
        ecs_get_type_info(world, vector_info->type);
    ecs_rtt_vector_ctx_t *rtt_ctx = ecs_os_malloc_t(ecs_rtt_vector_ctx_t);
    rtt_ctx->type_info = vector_ti;
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
        const ecs_type_info_t *ti = ecs_get_type_info(world, component);

        if (ti) {
            if (type->kind == EcsStructType) {
                flecs_rtt_init_default_hooks_struct(world, component, ti);
            } else if (type->kind == EcsArrayType) {
                flecs_rtt_init_default_hooks_array(world, component);
            } else if (type->kind == EcsVectorType) {
                flecs_rtt_init_default_hooks_vector(world, component);
            }
        }

        /* Make sure there is at least a default constructor. This ensures that
         * a new component value does not contain uninitialized memory, which
         * could cause serializers to crash when for example inspecting string
         * fields. */
        if (!ti || !ti->hooks.ctor) {
            ecs_set_hooks_id(
                world,
                component,
                &(ecs_type_hooks_t){.ctor = flecs_default_ctor});
        }
    }
}

#endif
