/**
 * @file addons/meta/runtime.c
 * @brief Runtime components.
 */

#include "meta.h"

#ifdef FLECS_META

static uintptr_t base;
#define PRINT_OFFSET(ptr) printf("offset=%lu\n", ((uintptr_t) ptr) - base);

typedef void (*ecs_meta_rtt_handle_opcode)(const ecs_world_t *world, void *dst_ptr, void *src_ptr,
                                           ecs_meta_type_op_t *op);

typedef struct ecs_meta_rtt_walk_t {
  ecs_meta_rtt_handle_opcode handle_opaque;
  ecs_meta_rtt_handle_opcode handle_vector;
  ecs_meta_rtt_handle_opcode handle_string;
  ecs_meta_rtt_handle_opcode handle_value;
} ecs_meta_rtt_walk_t;

static int flecs_meta_rtt_walk(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *ops,
                               int32_t ser_op_count, const ecs_meta_rtt_walk_t *walk) {
  for (int i = 0; i < ser_op_count; i++) {
    ecs_meta_type_op_t *op = &ops[i];
    switch (op->kind) {
      case EcsOpPush: {
        int pop_index = 0;
        for (int c = 0; c < op->count; c++) {
          size_t offset = op->size * c;
          pop_index = flecs_meta_rtt_walk(world, ECS_OFFSET(dst_ptr, offset), ECS_OFFSET(src_ptr, offset), &ops[i + 1],
                                          ser_op_count - i - 1, walk);
          ecs_assert(pop_index != -1, ECS_INTERNAL_ERROR, "unmatched push/pop");
        }
        i += pop_index + 1;
        break;
      }
      case EcsOpPop: {
        return i;
      }
      case EcsOpOpaque:
        if (walk->handle_opaque)
          walk->handle_opaque(world, ECS_OFFSET(dst_ptr, op->offset), ECS_OFFSET(src_ptr, op->offset), op);
        break;
      case EcsOpVector:
        if (walk->handle_vector)
          walk->handle_vector(world, ECS_OFFSET(dst_ptr, op->offset), ECS_OFFSET(src_ptr, op->offset), op);
        break;
      case EcsOpString:
        if (walk->handle_string)
          walk->handle_string(world, ECS_OFFSET(dst_ptr, op->offset), ECS_OFFSET(src_ptr, op->offset), op);
        break;
      case EcsOpArray:
      case EcsOpScope:
      case EcsOpEnum:
      case EcsOpBitmask:
      case EcsOpPrimitive:
      case EcsOpBool:
      case EcsOpChar:
      case EcsOpByte:
      case EcsOpU8:
      case EcsOpU16:
      case EcsOpU32:
      case EcsOpU64:
      case EcsOpI8:
      case EcsOpI16:
      case EcsOpI32:
      case EcsOpI64:
      case EcsOpF32:
      case EcsOpF64:
      case EcsOpUPtr:
      case EcsOpIPtr:
      case EcsOpEntity:
      case EcsOpId:
      default:
        walk->handle_value(world, ECS_OFFSET(dst_ptr, op->offset), ECS_OFFSET(src_ptr, op->offset), op);
        break;
    }
  }
  return -1;
}

// Destructors:
static void flecs_meta_rtt_opaque_dtor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  printf("dtor opaque at %p\n", dst_ptr);

  const ecs_type_info_t *ti = ecs_get_type_info(world, op->type);
  ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

  if (ti->hooks.dtor) {
    ti->hooks.dtor(dst_ptr, 1, ti);
  }
}

static void flecs_meta_rtt_vector_dtor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  const EcsVector *vt = ecs_get(world, op->type, EcsVector);
  ecs_assert(vt != NULL, ECS_INTERNAL_ERROR, NULL);

  ecs_vec_t *vec = (ecs_vec_t *) dst_ptr;
  const ecs_type_info_t *ti = ecs_get_type_info(world, vt->type);
  ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

  int32_t count = ecs_vec_count(vec);
  if (count && ti->hooks.dtor) {
    ti->hooks.dtor(ecs_vec_first(vec), count, ti);
  }
  ecs_vec_fini(NULL, vec, 0);
}

static void flecs_meta_rtt_value_dtor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
#ifndef NDEBUG
  ecs_os_memset(dst_ptr, 0xDE, op->size);
#endif
}

static void flecs_meta_rtt_string_dtor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  char **ppstr = (char **) dst_ptr;
  ecs_os_free(*ppstr);
  *ppstr = NULL;
}

// Constructors:
static void flecs_meta_rtt_opaque_ctor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  printf("ctor opaque at %p\n", dst_ptr);

  const ecs_type_info_t *ti = ecs_get_type_info(world, op->type);
  ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

  if (ti->hooks.ctor) {
    ti->hooks.ctor(dst_ptr, 1, ti);
  }
}

static void flecs_meta_rtt_vector_ctor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  const EcsVector *vt = ecs_get(world, op->type, EcsVector);
  ecs_assert(vt != NULL, ECS_INTERNAL_ERROR, NULL);

  ecs_vec_t *vec = (ecs_vec_t *) dst_ptr;
  const ecs_type_info_t *ti = ecs_get_type_info(world, vt->type);
  ecs_vec_init(NULL, vec, ti->size, 0);
}

static void flecs_meta_rtt_string_ctor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  char **ppstr = (char **) dst_ptr;
  *ppstr = NULL;
}

static void flecs_meta_rtt_value_ctor(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  ecs_os_memset(dst_ptr, 0, op->size);
}

// Move assign:
static void flecs_meta_rtt_opaque_move(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  const ecs_type_info_t *ti = ecs_get_type_info(world, op->type);
  ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
  ecs_assert(ti->hooks.move, ECS_INTERNAL_ERROR, "Cannot move component %s", ti->name);

  ti->hooks.move(dst_ptr, src_ptr, 1, ti);
}

static void flecs_meta_rtt_vector_move(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  flecs_meta_rtt_vector_dtor(world, dst_ptr, NULL, op);

  ecs_vec_t *dst_vec = (ecs_vec_t *) dst_ptr;
  ecs_vec_t *src_vec = (ecs_vec_t *) src_ptr;

  *dst_vec = *src_vec;
  src_vec->array = NULL;
}

static void flecs_meta_rtt_string_move(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  flecs_meta_rtt_string_dtor(world, dst_ptr, NULL, op);

  char **dst_ppstr = (char **) dst_ptr;
  char **src_ppstr = (char **) src_ptr;

  *dst_ppstr = *src_ppstr;
  *src_ppstr = NULL;
}

// Copy assign:
static void flecs_meta_rtt_opaque_copy(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  const ecs_type_info_t *ti = ecs_get_type_info(world, op->type);
  ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

  if (ti->hooks.copy) {
    ti->hooks.copy(dst_ptr, src_ptr, 1, ti);
  } else {
    ecs_os_memcpy(dst_ptr, src_ptr, ti->size);
  }
}

static void flecs_meta_rtt_vector_copy(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  flecs_meta_rtt_vector_dtor(world, dst_ptr, NULL, op);

  const ecs_type_info_t *ti = ecs_get_type_info(world, op->type);
  ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

  ecs_vec_t *dst_vec = (ecs_vec_t *) dst_vec;
  ecs_vec_t *src_vec = (ecs_vec_t *) src_vec;
  int32_t count = ecs_vec_count(src_vec);
  ecs_vec_init(NULL, dst_vec, ti->size, count);
  if (ti->hooks.copy) {
    ti->hooks.copy(ecs_vec_first(dst_vec), ecs_vec_first(src_vec), count, ti);
  } else {
    ecs_os_memcpy(dst_ptr, src_ptr, count * ti->size);
  }
}

static void flecs_meta_rtt_string_copy(const ecs_world_t *world, void *dst_ptr, void *src_ptr, ecs_meta_type_op_t *op) {
  flecs_meta_rtt_string_dtor(world, dst_ptr, src_ptr, op);
  char **dst_ppstr = (char **) dst_ptr;
  char **src_ppstr = (char **) src_ptr;

  size_t len = ecs_os_strlen(*src_ppstr);
  *dst_ppstr = ecs_os_malloc(len);
  ecs_os_strcpy(*dst_ppstr, *src_ppstr);
}

static void flecs_meta_rtt_value_move_copy(const ecs_world_t *world, void *dst_ptr, void *src_ptr,
                                           ecs_meta_type_op_t *op) {
  ecs_os_memcpy(dst_ptr, src_ptr, op->size);
}

static const ecs_meta_rtt_walk_t rtt_dtor_walk = {.handle_opaque = flecs_meta_rtt_opaque_dtor,
                                                  .handle_vector = flecs_meta_rtt_vector_dtor,
                                                  .handle_string = flecs_meta_rtt_string_dtor,
                                                  .handle_value = flecs_meta_rtt_value_dtor};

static const ecs_meta_rtt_walk_t rtt_ctor_walk = {.handle_opaque = flecs_meta_rtt_opaque_ctor,
                                                  .handle_vector = flecs_meta_rtt_vector_ctor,
                                                  .handle_string = flecs_meta_rtt_string_ctor,
                                                  .handle_value = flecs_meta_rtt_value_ctor};

static const ecs_meta_rtt_walk_t rtt_move_walk = {.handle_opaque = flecs_meta_rtt_opaque_move,
                                                  .handle_vector = flecs_meta_rtt_vector_move,
                                                  .handle_string = flecs_meta_rtt_string_move,
                                                  .handle_value = flecs_meta_rtt_value_move_copy};

static const ecs_meta_rtt_walk_t rtt_copy_walk = {.handle_opaque = flecs_meta_rtt_opaque_copy,
                                                  .handle_vector = flecs_meta_rtt_vector_copy,
                                                  .handle_string = flecs_meta_rtt_string_copy,
                                                  .handle_value = flecs_meta_rtt_value_move_copy};

static void flecs_meta_rtt_walk_opcodes(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info,
                                        const ecs_meta_rtt_walk_t *walk) {
  const ecs_world_t *world = type_info->world;
  const EcsTypeSerializer *ser = ecs_get(world, type_info->component, EcsTypeSerializer);
  ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

  ecs_meta_type_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_type_op_t);
  int32_t op_count = ecs_vec_count(&ser->ops);
  base = (uintptr_t) dst_ptr;
  for (int i = 0; i < count; i++) {
    size_t offset = type_info->size * i;
    flecs_meta_rtt_walk(world, ECS_OFFSET(dst_ptr, offset), ECS_OFFSET(src_ptr, offset), ops, op_count, walk);
  }
}

void flecs_meta_rtt_dtor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  flecs_meta_rtt_walk_opcodes(ptr, NULL, count, type_info, &rtt_dtor_walk);
}

void flecs_meta_rtt_ctor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  flecs_meta_rtt_walk_opcodes(ptr, NULL, count, type_info, &rtt_ctor_walk);
}

void flecs_meta_rtt_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  flecs_meta_rtt_walk_opcodes(dst_ptr, src_ptr, count, type_info, &rtt_move_walk);
}

void flecs_meta_rtt_copy(void *dst_ptr, const void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  flecs_meta_rtt_walk_opcodes(dst_ptr, ECS_CONST_CAST(void *, src_ptr), count, type_info, &rtt_copy_walk);
}

#endif