/**
 * @file addons/meta/serialized.c
 * @brief Serialize type into flat operations array to speed up deserialization.
 */

#include "meta.h"

#ifdef FLECS_META

static
int flecs_meta_serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops);

ecs_meta_type_op_kind_t flecs_meta_primitive_to_op_kind(ecs_primitive_kind_t kind) {
    return EcsOpPrimitive + kind;
}

static
ecs_size_t flecs_meta_type_size(ecs_world_t *world, ecs_entity_t type) {
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);
    return comp->size;
}

static
ecs_meta_type_op_t* flecs_meta_ops_add(ecs_vec_t *ops, ecs_meta_type_op_kind_t kind) {
    ecs_meta_type_op_t *op = ecs_vec_append_t(NULL, ops, ecs_meta_type_op_t);
    op->kind = kind;
    op->offset = 0;
    op->count = 1;
    op->op_count = 1;
    op->size = 0;
    op->name = NULL;
    op->members = NULL;
    op->type = 0;
    op->member_index = 0;
    return op;
}

static
ecs_meta_type_op_t* flecs_meta_ops_get(ecs_vec_t *ops, int32_t index) {
    ecs_meta_type_op_t* op = ecs_vec_get_t(ops, ecs_meta_type_op_t, index);
    ecs_assert(op != NULL, ECS_INTERNAL_ERROR, NULL);
    return op;
}

static
int flecs_meta_serialize_primitive(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    const EcsPrimitive *ptr = ecs_get(world, type, EcsPrimitive);
    if (!ptr) {
        char *name = ecs_get_path(world, type);
        ecs_err("entity '%s' is not a primitive type", name);
        ecs_os_free(name);
        return -1;
    }

    ecs_meta_type_op_t *op = flecs_meta_ops_add(ops, flecs_meta_primitive_to_op_kind(ptr->kind));
    op->offset = offset;
    op->type = type;
    op->size = flecs_meta_type_size(world, type);
    return 0;
}

static
int flecs_meta_serialize_enum(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    
    ecs_meta_type_op_t *op = flecs_meta_ops_add(ops, EcsOpEnum);
    op->offset = offset;
    op->type = type;
    op->size = ECS_SIZEOF(ecs_i32_t);
    return 0;
}

static
int flecs_meta_serialize_bitmask(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    
    ecs_meta_type_op_t *op = flecs_meta_ops_add(ops, EcsOpBitmask);
    op->offset = offset;
    op->type = type;
    op->size = ECS_SIZEOF(ecs_u32_t);
    return 0;
}

static
int flecs_meta_serialize_array(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;

    ecs_meta_type_op_t *op = flecs_meta_ops_add(ops, EcsOpArray);
    op->offset = offset;
    op->type = type;
    op->size = flecs_meta_type_size(world, type);
    return 0;
}

static
int flecs_meta_serialize_array_component(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_vec_t *ops)
{
    const EcsArray *ptr = ecs_get(world, type, EcsArray);
    if (!ptr) {
        return -1; /* Should never happen, will trigger internal error */
    }

    if (flecs_meta_serialize_type(world, ptr->type, 0, ops) != 0) {
        return -1;
    }

    ecs_meta_type_op_t *first = ecs_vec_first(ops);
    first->count = ptr->count;
    return 0;
}

static
int flecs_meta_serialize_vector(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    ecs_meta_type_op_t *op = flecs_meta_ops_add(ops, EcsOpVector);
    op->offset = offset;
    op->type = type;
    op->size = flecs_meta_type_size(world, type);
    return 0;
}

static
int flecs_meta_serialize_custom_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    (void)world;
    ecs_meta_type_op_t *op = flecs_meta_ops_add(ops, EcsOpOpaque);
    op->offset = offset;
    op->type = type;
    op->size = flecs_meta_type_size(world, type);
    return 0;
}

static
int flecs_meta_serialize_struct(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    const EcsStruct *ptr = ecs_get(world, type, EcsStruct);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t cur, first = ecs_vec_count(ops);
    ecs_meta_type_op_t *op = flecs_meta_ops_add(ops, EcsOpPush);
    op->offset = offset;
    op->type = type;
    op->size = flecs_meta_type_size(world, type);

    ecs_member_t *members = ecs_vec_first(&ptr->members);
    int32_t i, count = ecs_vec_count(&ptr->members);

    ecs_hashmap_t *member_index = NULL;
    if (count) {        
        op->members = member_index = flecs_name_index_new(
            world, &world->allocator);
    }

    for (i = 0; i < count; i ++) {
        ecs_member_t *member = &members[i];

        cur = ecs_vec_count(ops);
        if (flecs_meta_serialize_type(world, member->type, offset + member->offset, ops) != 0) {
            continue;
        }

        op = flecs_meta_ops_get(ops, cur);
        if (!op->type) {
            op->type = member->type;
        }

        if (op->count <= 1) {
            op->count = member->count;
        }

        const char *member_name = member->name;
        op->name = member_name;
        op->op_count = ecs_vec_count(ops) - cur;
        op->member_index = i;

        flecs_name_index_ensure(
            member_index, flecs_ito(uint64_t, cur - first - 1), 
                member_name, 0, 0);
    }

    ecs_meta_type_op_t *pop = flecs_meta_ops_add(ops, EcsOpPop);
    pop->type = type;
    flecs_meta_ops_get(ops, first)->op_count = ecs_vec_count(ops) - first;
    return 0;
}

static
int flecs_meta_serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vec_t *ops)
{
    const EcsType *ptr = ecs_get(world, type, EcsType);
    if (!ptr) {
        char *path = ecs_get_path(world, type);
        ecs_err("missing EcsType for type %s'", path);
        ecs_os_free(path);
        return -1;
    }

    switch(ptr->kind) {
    case EcsPrimitiveType: return flecs_meta_serialize_primitive(world, type, offset, ops);
    case EcsEnumType: return flecs_meta_serialize_enum(world, type, offset, ops);
    case EcsBitmaskType: return flecs_meta_serialize_bitmask(world, type, offset, ops);
    case EcsStructType: return flecs_meta_serialize_struct(world, type, offset, ops);
    case EcsArrayType: return flecs_meta_serialize_array(world, type, offset, ops);
    case EcsVectorType: return flecs_meta_serialize_vector(world, type, offset, ops);
    case EcsOpaqueType: return flecs_meta_serialize_custom_type(world, type, offset, ops);
    }

    return 0;
}

static
int flecs_meta_serialize_component(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_vec_t *ops)
{
    const EcsType *ptr = ecs_get(world, type, EcsType);
    if (!ptr) {
        char *path = ecs_get_path(world, type);
        ecs_err("missing EcsType for type %s'", path);
        ecs_os_free(path);
        return -1;
    }

    if (ptr->kind == EcsArrayType) {
        return flecs_meta_serialize_array_component(world, type, ops);
    } else {
        return flecs_meta_serialize_type(world, type, 0, ops);
    }
}

void ecs_meta_type_serialized_init(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_vec_t ops;
        ecs_vec_init_t(NULL, &ops, ecs_meta_type_op_t, 0);
        flecs_meta_serialize_component(world, e, &ops);

        EcsTypeSerializer *ptr = ecs_ensure(
            world, e, EcsTypeSerializer);
        if (ptr->ops.array) {
            ecs_meta_dtor_serialized(ptr);
        }

        ptr->ops = ops;
    }
}

#endif
