/**
 * @file addons/meta/runtime.c
 * @brief Runtime components.
 */

#include "flecs.h"
#include "meta.h"

#ifdef FLECS_META

typedef struct ecs_meta_rtt_xtor_data_t {
  ecs_xtor_t xtor;
  const ecs_type_info_t *type_info;
  size_t offset;
  int32_t count;
} ecs_meta_rtt_xtor_data_t;

typedef struct ecs_meta_rtt_move_data_t {
  ecs_move_t move;
  const ecs_type_info_t *type_info;
  size_t offset;
  int32_t count;
} ecs_meta_rtt_move_data_t;

typedef struct ecs_meta_rtt_copy_data_t {
  ecs_copy_t copy;
  const ecs_type_info_t *type_info;
  size_t offset;
  int32_t count;
} ecs_meta_rtt_copy_data_t;

typedef struct ecs_meta_rtt_ctx_t {
  ecs_vec_t ctors; /*< vector<ecs_meta_rtt_xtor_data_t> */
  ecs_vec_t dtors; /*< vector<ecs_meta_rtt_xtor_data_t> */
  ecs_vec_t copys; /*< vector<ecs_meta_rtt_move_data_t> */
  ecs_vec_t moves; /*< vector<ecs_meta_rtt_copy_data_t> */
} ecs_meta_rtt_ctx_t;

static void flecs_meta_rtt_xtor(ecs_vec_t *xtor_data_vec, void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  int cb_count = ecs_vec_count(xtor_data_vec);
  for (int j = 0; j < count; j++) {
    void *elem_ptr = ECS_ELEM(ptr, type_info->size, j);
    for (int i = 0; i < cb_count; i++) {
      ecs_meta_rtt_xtor_data_t *xtor_data = ecs_vec_get_t(xtor_data_vec, ecs_meta_rtt_xtor_data_t, i);
      xtor_data->xtor(ECS_OFFSET(elem_ptr, xtor_data->offset), xtor_data->count, xtor_data->type_info);
    }
  }
}

void flecs_meta_rtt_ctor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);
  flecs_meta_rtt_xtor(&rtt_ctx->ctors, ptr, count, type_info);
}

void flecs_meta_rtt_dtor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);
  flecs_meta_rtt_xtor(&rtt_ctx->dtors, ptr, count, type_info);
}

void flecs_meta_rtt_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

  int cb_count = ecs_vec_count(&rtt_ctx->moves);
  for (int j = 0; j < count; j++) {
    void *elem_dst_ptr = ECS_ELEM(dst_ptr, type_info->size, j);
    void *elem_src_ptr = ECS_ELEM(src_ptr, type_info->size, j);
    for (int i = 0; i < cb_count; i++) {
      ecs_meta_rtt_move_data_t *move_data = ecs_vec_get_t(&rtt_ctx->moves, ecs_meta_rtt_move_data_t, i);
      move_data->move(ECS_OFFSET(elem_dst_ptr, move_data->offset), ECS_OFFSET(elem_src_ptr, move_data->offset),
                      move_data->count, move_data->type_info);
    }
  }
}

void flecs_meta_rtt_copy(void *dst_ptr, const void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_meta_rtt_ctx_t *rtt_ctx = (ecs_meta_rtt_ctx_t *) type_info->hooks.lifecycle_ctx;
  ecs_assert(rtt_ctx != NULL, ECS_INTERNAL_ERROR, NULL);

  int cb_count = ecs_vec_count(&rtt_ctx->copys);
  for (int j = 0; j < count; j++) {
    void *elem_dst_ptr = ECS_ELEM(dst_ptr, type_info->size, j);
    const void *elem_src_ptr = ECS_ELEM(src_ptr, type_info->size, j);
    for (int i = 0; i < cb_count; i++) {
      ecs_meta_rtt_copy_data_t *copy_data = ecs_vec_get_t(&rtt_ctx->copys, ecs_meta_rtt_copy_data_t, i);
      copy_data->copy(ECS_OFFSET(elem_dst_ptr, copy_data->offset), ECS_OFFSET(elem_src_ptr, copy_data->offset),
                      copy_data->count, copy_data->type_info);
    }
  }
}

static void flecs_meta_rtt_default_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  ecs_os_memcpy(dst_ptr, src_ptr, count);
}

static void flecs_meta_rtt_default_copy(void *dst_ptr, const void *src_ptr, int32_t count,
                                        const ecs_type_info_t *type_info) {
  ecs_os_memcpy(dst_ptr, src_ptr, count);
}

static void flecs_meta_rtt_free_lifecycle_ctx(void *ctx) {
  if (!ctx)
    return;

  ecs_meta_rtt_ctx_t *lifecycle_ctx = (ecs_meta_rtt_ctx_t *) ctx;

  ecs_vec_fini_t(NULL, &lifecycle_ctx->ctors, ecs_meta_rtt_xtor_data_t);
  ecs_vec_fini_t(NULL, &lifecycle_ctx->dtors, ecs_meta_rtt_xtor_data_t);
  ecs_vec_fini_t(NULL, &lifecycle_ctx->moves, ecs_meta_rtt_move_data_t);
  ecs_vec_fini_t(NULL, &lifecycle_ctx->copys, ecs_meta_rtt_copy_data_t);

  ecs_os_free(ctx);
}

static ecs_meta_rtt_ctx_t *flecs_meta_rtt_configure_hooks(ecs_world_t *world, const ecs_type_info_t *ti, bool ctor,
                                                          bool dtor, bool move, bool copy) {
  ecs_type_hooks_t hooks = ti->hooks;
  flecs_meta_rtt_free_lifecycle_ctx(hooks.lifecycle_ctx);
  ecs_meta_rtt_ctx_t *rtt_ctx = NULL;
  if (ctor || dtor || move || copy) {
    rtt_ctx = ecs_os_malloc_t(ecs_meta_rtt_ctx_t);
    ecs_vec_init_t(NULL, &rtt_ctx->ctors, ecs_meta_rtt_xtor_data_t, 0);
    ecs_vec_init_t(NULL, &rtt_ctx->dtors, ecs_meta_rtt_xtor_data_t, 0);
    ecs_vec_init_t(NULL, &rtt_ctx->moves, ecs_meta_rtt_move_data_t, 0);
    ecs_vec_init_t(NULL, &rtt_ctx->copys, ecs_meta_rtt_copy_data_t, 0);
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

void flecs_meta_rtt_init_default_hooks(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  EcsType *type_field = ecs_field(it, EcsType, 0);

  int i;
  for (i = 0; i < it->count; i++) {
    EcsType *type = &type_field[i];
    if (type->existing)
      continue;  // non-rtt type.

    /* If a component is defined from reflection data, configure it with the
     * default constructor. This ensures that a new component value does not
     * contain uninitialized memory, which could cause serializers to crash
     * when for example inspecting string fields. */

    ecs_entity_t e = it->entities[i];
    const ecs_type_info_t *ti = ecs_get_type_info(world, e);

    if (type->kind == EcsStructType) {
      const EcsStruct *struct_info = ecs_get(world, e, EcsStruct);
      ecs_assert(struct_info != NULL, ECS_INTERNAL_ERROR, NULL);
      bool ctor_hook_required = false;
      bool dtor_hook_required = false;
      bool move_hook_required = false;
      bool copy_hook_required = false;

      for (int i = 0; i < ecs_vec_count(&struct_info->members); i++) {
        ecs_member_t *m = ecs_vec_get_t(&struct_info->members, ecs_member_t, i);
        const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
        ctor_hook_required |= member_ti->hooks.ctor && member_ti->hooks.ctor != flecs_default_ctor;
        dtor_hook_required |= member_ti->hooks.dtor != NULL;
        move_hook_required |= member_ti->hooks.move != NULL;
        copy_hook_required |= member_ti->hooks.copy != NULL;
      }

      ecs_meta_rtt_ctx_t *rtt_ctx = flecs_meta_rtt_configure_hooks(world, ti, ctor_hook_required, dtor_hook_required,
                                                                   move_hook_required, copy_hook_required);

      if (rtt_ctx) {
        for (int i = 0; i < ecs_vec_count(&struct_info->members); i++) {
          ecs_member_t *m = ecs_vec_get_t(&struct_info->members, ecs_member_t, i);
          const ecs_type_info_t *member_ti = ecs_get_type_info(world, m->type);
          if (ctor_hook_required) {
            ecs_meta_rtt_xtor_data_t *ctor_data = ecs_vec_append_t(NULL, &rtt_ctx->ctors, ecs_meta_rtt_xtor_data_t);
            ctor_data->count = m->count;
            ctor_data->offset = m->offset;
            ctor_data->type_info = member_ti;
            if (member_ti->hooks.ctor) {
              ctor_data->xtor = member_ti->hooks.ctor;
            } else {
              ctor_data->xtor = flecs_default_ctor;
            }
          }
          if (dtor_hook_required) {
            if (member_ti->hooks.dtor) {
              ecs_meta_rtt_xtor_data_t *dtor_data = ecs_vec_append_t(NULL, &rtt_ctx->dtors, ecs_meta_rtt_xtor_data_t);
              dtor_data->count = m->count;
              dtor_data->offset = m->offset;
              dtor_data->type_info = member_ti;
              dtor_data->xtor = member_ti->hooks.dtor;
            }
          }
          if (move_hook_required) {
            ecs_meta_rtt_move_data_t *move_data = ecs_vec_append_t(NULL, &rtt_ctx->moves, ecs_meta_rtt_move_data_t);
            move_data->offset = m->offset;
            move_data->type_info = member_ti;
            if (member_ti->hooks.move) {
              move_data->count = m->count;
              move_data->move = member_ti->hooks.move;
            } else {
              move_data->count = m->size * m->count;
              move_data->move = flecs_meta_rtt_default_move;
            }
          }
          if (copy_hook_required) {
            ecs_meta_rtt_copy_data_t *copy_data = ecs_vec_append_t(NULL, &rtt_ctx->copys, ecs_meta_rtt_copy_data_t);
            copy_data->offset = m->offset;
            copy_data->type_info = member_ti;
            if (member_ti->hooks.copy) {
              copy_data->count = m->count;
              copy_data->copy = member_ti->hooks.copy;
            } else {
              copy_data->count = m->size * m->count;
              copy_data->copy = flecs_meta_rtt_default_copy;
            }
          }
        }
        continue;
      }
      // rtt hook not required, so potentially set a default ctor below.
    } else if (type->kind == EcsArrayType) {
      const EcsArray *array_info = ecs_get(world, e, EcsArray);
      ecs_assert(array_info != NULL, ECS_INTERNAL_ERROR, NULL);
      const ecs_type_info_t *array_ti = ecs_get_type_info(world, array_info->type);
      bool ctor_hook_required = array_ti->hooks.ctor && array_ti->hooks.ctor != flecs_default_ctor;
      bool dtor_hook_required = array_ti->hooks.dtor != NULL;
      bool move_hook_required = array_ti->hooks.move != NULL;
      bool copy_hook_required = array_ti->hooks.copy != NULL;

      ecs_meta_rtt_ctx_t *rtt_ctx = flecs_meta_rtt_configure_hooks(world, ti, ctor_hook_required, dtor_hook_required,
                                                                   move_hook_required, copy_hook_required);

      if (rtt_ctx) {
        if (ctor_hook_required) {
          ecs_meta_rtt_xtor_data_t *ctor_data = ecs_vec_append_t(NULL, &rtt_ctx->ctors, ecs_meta_rtt_xtor_data_t);
          ctor_data->count = array_info->count;
          ctor_data->offset = 0;
          ctor_data->type_info = array_ti;
          ctor_data->xtor = array_ti->hooks.ctor;
        }
        if (dtor_hook_required) {
          ecs_meta_rtt_xtor_data_t *dtor_data = ecs_vec_append_t(NULL, &rtt_ctx->dtors, ecs_meta_rtt_xtor_data_t);
          dtor_data->count = array_info->count;
          dtor_data->offset = 0;
          dtor_data->type_info = array_ti;
          dtor_data->xtor = array_ti->hooks.dtor;
        }
        if (move_hook_required) {
          ecs_meta_rtt_move_data_t *move_data = ecs_vec_append_t(NULL, &rtt_ctx->moves, ecs_meta_rtt_move_data_t);
          move_data->count = array_info->count;
          move_data->offset = 0;
          move_data->type_info = array_ti;
          move_data->move = array_ti->hooks.move;
        }
        if (copy_hook_required) {
          ecs_meta_rtt_copy_data_t *copy_data = ecs_vec_append_t(NULL, &rtt_ctx->copys, ecs_meta_rtt_copy_data_t);
          copy_data->count = array_info->count;
          copy_data->offset = 0;
          copy_data->type_info = array_ti;
          copy_data->copy = array_ti->hooks.copy;
        }
        continue;
      }
      // rtt hook not required, so potentially set a default ctor below.
    } else if (type->kind == EcsVectorType) {
      // TODO -- pending cursor primitives to manipulate vectors
    }
    if (!ti || !ti->hooks.ctor) {
      ecs_set_hooks_id(world, e, &(ecs_type_hooks_t){.ctor = flecs_default_ctor});
    }
  }
}

#endif