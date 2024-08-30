/**
 * @file addons/meta/runtime.c
 * @brief Runtime components.
 */

#include "flecs.h"
#include "meta.h"

#ifdef FLECS_META

// Stores all the information necessary to forward a hook call to a nested/underlying type
typedef struct ecs_meta_rtt_call_data_t {
  union {
    ecs_xtor_t xtor;
    ecs_move_t move;
    ecs_copy_t copy;
  } hook;
  const ecs_type_info_t *type_info;
  int32_t offset;
  int32_t count;
} ecs_meta_rtt_call_data_t;

// Lifecycle context for runtime types
typedef struct ecs_meta_rtt_ctx_t {
  ecs_vec_t ctors; /*< vector<ecs_meta_rtt_call_data_t> */
  ecs_vec_t dtors; /*< vector<ecs_meta_rtt_call_data_t> */
  ecs_vec_t copys; /*< vector<ecs_meta_rtt_call_data_t> */
  ecs_vec_t moves; /*< vector<ecs_meta_rtt_call_data_t> */
} ecs_meta_rtt_ctx_t;

// Invokes underlying type's constructor/destructor using saved information in the lifecycle context
static void flecs_meta_rtt_xtor(ecs_vec_t *xtor_data_vec, void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  int cb_count = ecs_vec_count(xtor_data_vec);
  for (int j = 0; j < count; j++) {
    void *elem_ptr = ECS_ELEM(ptr, type_info->size, j);
    for (int i = 0; i < cb_count; i++) {
      ecs_meta_rtt_call_data_t *xtor_data = ecs_vec_get_t(xtor_data_vec, ecs_meta_rtt_call_data_t, i);
      xtor_data->hook.xtor(ECS_OFFSET(elem_ptr, xtor_data->offset), xtor_data->count, xtor_data->type_info);
    }
  }
}

// Generic constructor. It will read hook information call data from the type's lifecycle context and call
// the constructors configured when the type was created.
static void flecs_meta_rtt_ctor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);
  flecs_meta_rtt_xtor(&rtt_ctx->ctors, ptr, count, type_info);
}

// Generic destructor. It will read hook information call data from the type's lifecycle context and call
// the destructors configured when the type was created.
static void flecs_meta_rtt_dtor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);
  flecs_meta_rtt_xtor(&rtt_ctx->dtors, ptr, count, type_info);
}

// Generic move hook. It will read hook information call data from the type's lifecycle context and call
// the move hooks configured when the type was created.
static void flecs_meta_rtt_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

  int cb_count = ecs_vec_count(&rtt_ctx->moves);
  for (int j = 0; j < count; j++) {
    ecs_size_t elem_offset = type_info->size * j;
    void *elem_dst_ptr = ECS_OFFSET(dst_ptr, elem_offset);
    void *elem_src_ptr = ECS_OFFSET(src_ptr, elem_offset);
    for (int i = 0; i < cb_count; i++) {
      ecs_meta_rtt_call_data_t *move_data = ecs_vec_get_t(&rtt_ctx->moves, ecs_meta_rtt_call_data_t, i);
      move_data->hook.move(ECS_OFFSET(elem_dst_ptr, move_data->offset), ECS_OFFSET(elem_src_ptr, move_data->offset),
                           move_data->count, move_data->type_info);
    }
  }
}

// Generic copy hook. It will read hook information call data from the type's lifecycle context and call
// the copy hooks configured when the type was created.
static void flecs_meta_rtt_copy(void *dst_ptr, const void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

  int cb_count = ecs_vec_count(&rtt_ctx->copys);
  for (int j = 0; j < count; j++) {
    ecs_size_t elem_offset = type_info->size * j;
    void *elem_dst_ptr = ECS_OFFSET(dst_ptr, elem_offset);
    const void *elem_src_ptr = ECS_OFFSET(src_ptr, elem_offset);
    for (int i = 0; i < cb_count; i++) {
      ecs_meta_rtt_call_data_t *copy_data = ecs_vec_get_t(&rtt_ctx->copys, ecs_meta_rtt_call_data_t, i);
      copy_data->hook.copy(ECS_OFFSET(elem_dst_ptr, copy_data->offset), ECS_OFFSET(elem_src_ptr, copy_data->offset),
                           copy_data->count, copy_data->type_info);
    }
  }
}

//
static void flecs_meta_rtt_default_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  (void) type_info;  // silence unused warning
  ecs_os_memcpy(dst_ptr, src_ptr, count);
}

static void flecs_meta_rtt_default_copy(void *dst_ptr, const void *src_ptr, int32_t count,
                                        const ecs_type_info_t *type_info) {
  (void) type_info;  // silence unused warning
  ecs_os_memcpy(dst_ptr, src_ptr, count);
}

static void flecs_meta_rtt_free_lifecycle_ctx(void *ctx) {
  if (!ctx)
    return;

  ecs_meta_rtt_ctx_t *lifecycle_ctx = (ecs_meta_rtt_ctx_t *) ctx;

  ecs_vec_fini_t(NULL, &lifecycle_ctx->ctors, ecs_meta_rtt_call_data_t);
  ecs_vec_fini_t(NULL, &lifecycle_ctx->dtors, ecs_meta_rtt_call_data_t);
  ecs_vec_fini_t(NULL, &lifecycle_ctx->moves, ecs_meta_rtt_call_data_t);
  ecs_vec_fini_t(NULL, &lifecycle_ctx->copys, ecs_meta_rtt_call_data_t);

  ecs_os_free(ctx);
}

static ecs_meta_rtt_ctx_t *flecs_meta_rtt_configure_hooks(ecs_world_t *world, const ecs_type_info_t *ti, bool ctor,
                                                          bool dtor, bool move, bool copy) {
  ecs_type_hooks_t hooks = ti->hooks;
  flecs_meta_rtt_free_lifecycle_ctx(hooks.lifecycle_ctx);
  ecs_meta_rtt_ctx_t *rtt_ctx = NULL;
  if (ctor || dtor || move || copy) {
    rtt_ctx = ecs_os_malloc_t(ecs_meta_rtt_ctx_t);
    ecs_vec_init_t(NULL, &rtt_ctx->ctors, ecs_meta_rtt_call_data_t, 0);
    ecs_vec_init_t(NULL, &rtt_ctx->dtors, ecs_meta_rtt_call_data_t, 0);
    ecs_vec_init_t(NULL, &rtt_ctx->moves, ecs_meta_rtt_call_data_t, 0);
    ecs_vec_init_t(NULL, &rtt_ctx->copys, ecs_meta_rtt_call_data_t, 0);
    hooks.lifecycle_ctx = rtt_ctx;
    hooks.lifecycle_ctx_free = flecs_meta_rtt_free_lifecycle_ctx;

    if (ctor) {
      hooks.ctor = flecs_meta_rtt_ctor;
    }
    if (dtor) {
      hooks.dtor = flecs_meta_rtt_dtor;
    }
    if (move) {
      hooks.move = flecs_meta_rtt_move;
    }
    if (copy) {
      hooks.copy = flecs_meta_rtt_copy;
    }
  } else {
    hooks.lifecycle_ctx = NULL;
  }
  ecs_set_hooks_id(world, ti->component, &hooks);
  return rtt_ctx;
}

// Checks if a struct member's types have hooks installed. If so, it generates and installs required hooks for the
// struct type itself. These hooks will invoke the member hooks when necessary
// returns true if hooks were actually configured, false otherwise (struct did not need hooks)
static void flecs_meta_rtt_init_default_hooks_struct(ecs_world_t *world, ecs_entity_t component,
                                                     const ecs_type_info_t *ti) {
  // Obtain struct information to figure out what members it contains:
  const EcsStruct *struct_info = ecs_get(world, component, EcsStruct);
  ecs_assert(struct_info != NULL, ECS_INTERNAL_ERROR, NULL);

  // These flags will be set to true if we determine we need to generate a hook of a particular type:
  bool ctor_hook_required = false;
  bool dtor_hook_required = false;
  bool move_hook_required = false;
  bool copy_hook_required = false;

  // Iterate all struct members and see if hooks are required:
  for (int i = 0; i < ecs_vec_count(&struct_info->members); i++) {
    ecs_member_t *m = ecs_vec_get_t(&struct_info->members, ecs_member_t, i);
    const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
    ctor_hook_required |= member_ti->hooks.ctor && member_ti->hooks.ctor != flecs_default_ctor;
    dtor_hook_required |= member_ti->hooks.dtor != NULL;
    move_hook_required |= member_ti->hooks.move != NULL;
    copy_hook_required |= member_ti->hooks.copy != NULL;
  }

  // If any hook is required, then create a lifecycle context and configure a generic hook that will interpret that
  // context:
  ecs_meta_rtt_ctx_t *rtt_ctx = flecs_meta_rtt_configure_hooks(world, ti, ctor_hook_required, dtor_hook_required,
                                                               move_hook_required, copy_hook_required);

  if (!rtt_ctx)
    return;  // no hooks required

  // At least a hook was configured, therefore examine each struct member to build the vector of calls that will then
  // be executed by the generic hook handler:
  for (int i = 0; i < ecs_vec_count(&struct_info->members); i++) {
    ecs_member_t *m = ecs_vec_get_t(&struct_info->members, ecs_member_t, i);
    const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
    if (ctor_hook_required) {
      ecs_meta_rtt_call_data_t *ctor_data = ecs_vec_append_t(NULL, &rtt_ctx->ctors, ecs_meta_rtt_call_data_t);
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
      ecs_meta_rtt_call_data_t *dtor_data = ecs_vec_append_t(NULL, &rtt_ctx->dtors, ecs_meta_rtt_call_data_t);
      dtor_data->count = m->count;
      dtor_data->offset = m->offset;
      dtor_data->type_info = member_ti;
      dtor_data->hook.xtor = member_ti->hooks.dtor;
    }
    if (move_hook_required) {
      ecs_meta_rtt_call_data_t *move_data = ecs_vec_append_t(NULL, &rtt_ctx->moves, ecs_meta_rtt_call_data_t);
      move_data->offset = m->offset;
      move_data->type_info = member_ti;
      if (member_ti->hooks.move) {
        move_data->count = m->count;
        move_data->hook.move = member_ti->hooks.move;
      } else {
        move_data->count = m->size * m->count;
        move_data->hook.move = flecs_meta_rtt_default_move;
      }
    }
    if (copy_hook_required) {
      ecs_meta_rtt_call_data_t *copy_data = ecs_vec_append_t(NULL, &rtt_ctx->copys, ecs_meta_rtt_call_data_t);
      copy_data->offset = m->offset;
      copy_data->type_info = member_ti;
      if (member_ti->hooks.copy) {
        copy_data->count = m->count;
        copy_data->hook.copy = member_ti->hooks.copy;
      } else {
        copy_data->count = m->size * m->count;
        copy_data->hook.copy = flecs_meta_rtt_default_copy;
      }
    }
  }
}

// Checks if an array's underlying type has hooks installed. If so, it generates and installs required hooks for the
// array type itself. These hooks will invoke the underlying type's hook for each element in the array.
static void flecs_meta_rtt_init_default_hooks_array(ecs_world_t *world, ecs_entity_t component,
                                                    const ecs_type_info_t *ti) {
  const EcsArray *array_info = ecs_get(world, component, EcsArray);
  ecs_assert(array_info != NULL, ECS_INTERNAL_ERROR, NULL);
  const ecs_type_info_t *array_ti = ecs_get_type_info(world, array_info->type);
  bool ctor_hook_required = array_ti->hooks.ctor && array_ti->hooks.ctor != flecs_default_ctor;
  bool dtor_hook_required = array_ti->hooks.dtor != NULL;
  bool move_hook_required = array_ti->hooks.move != NULL;
  bool copy_hook_required = array_ti->hooks.copy != NULL;

  ecs_meta_rtt_ctx_t *rtt_ctx = flecs_meta_rtt_configure_hooks(world, ti, ctor_hook_required, dtor_hook_required,
                                                               move_hook_required, copy_hook_required);

  if (!rtt_ctx)
    return;  // no hooks required

  if (ctor_hook_required) {
    ecs_meta_rtt_call_data_t *ctor_data = ecs_vec_append_t(NULL, &rtt_ctx->ctors, ecs_meta_rtt_call_data_t);
    ctor_data->count = array_info->count;
    ctor_data->offset = 0;
    ctor_data->type_info = array_ti;
    ctor_data->hook.xtor = array_ti->hooks.ctor;
  }
  if (dtor_hook_required) {
    ecs_meta_rtt_call_data_t *dtor_data = ecs_vec_append_t(NULL, &rtt_ctx->dtors, ecs_meta_rtt_call_data_t);
    dtor_data->count = array_info->count;
    dtor_data->offset = 0;
    dtor_data->type_info = array_ti;
    dtor_data->hook.xtor = array_ti->hooks.dtor;
  }
  if (move_hook_required) {
    ecs_meta_rtt_call_data_t *move_data = ecs_vec_append_t(NULL, &rtt_ctx->moves, ecs_meta_rtt_call_data_t);
    move_data->count = array_info->count;
    move_data->offset = 0;
    move_data->type_info = array_ti;
    move_data->hook.move = array_ti->hooks.move;
  }
  if (copy_hook_required) {
    ecs_meta_rtt_call_data_t *copy_data = ecs_vec_append_t(NULL, &rtt_ctx->copys, ecs_meta_rtt_call_data_t);
    copy_data->count = array_info->count;
    copy_data->offset = 0;
    copy_data->type_info = array_ti;
    copy_data->hook.copy = array_ti->hooks.copy;
  }
}

void flecs_meta_rtt_init_default_hooks(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  EcsType *type_field = ecs_field(it, EcsType, 0);

  int i;
  for (i = 0; i < it->count; i++) {
    EcsType *type = &type_field[i];
    if (type->existing)
      continue;  // non-rtt type. Ignore.

    /* If a component is defined from reflection data, configure appropriate default hooks.
     * - For trivial types, at least set a default constructor so memory is zero-initialized
     * - For struct types, configure a hook that in turn calls hooks of member types, if those member types have hooks
     * defined themselves.
     * - For array types, configure a hook that in turn calls hooks for the underlying type, for each
     * element in the array.
     * */

    ecs_entity_t component = it->entities[i];
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);

    if (ti) {
      if (type->kind == EcsStructType) {
        flecs_meta_rtt_init_default_hooks_struct(world, component, ti);
      } else if (type->kind == EcsArrayType) {
        flecs_meta_rtt_init_default_hooks_array(world, component, ti);
      } else if (type->kind == EcsVectorType) {
        // TODO -- pending cursor primitives to manipulate vectors. These cursor primitives would have to at least
        // invoke ctor/dtor when growing/shrinking the vector.
      }
    }

    /* Make sure there is at least a default constructor. This ensures that a new
     * component value does not contain uninitialized memory, which could cause serializers to crash when for example
     * inspecting string fields. */
    if (!ti || !ti->hooks.ctor) {
      ecs_set_hooks_id(world, component, &(ecs_type_hooks_t){.ctor = flecs_default_ctor});
    }
  }
}

#endif
