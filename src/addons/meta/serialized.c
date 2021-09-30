#include "meta.h"

#ifdef FLECS_META

static
ecs_vector_t* serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vector_t *ops);

static
ecs_meta_type_op_kind_t primitive_to_op_kind(ecs_primitive_kind_t kind) {
    return EcsOpPrimitive + kind;
}

static
ecs_meta_type_op_t* ops_add(ecs_vector_t **ops, ecs_meta_type_op_kind_t kind) {
    ecs_meta_type_op_t *op = ecs_vector_add(ops, ecs_meta_type_op_t);
    op->kind = kind;
    op->offset = 0;
    op->count = 1;
    op->op_count = 1;
    op->name = NULL;
    op->type = 0;
    return op;
}

static
ecs_meta_type_op_t* ops_get(ecs_vector_t *ops, int32_t index) {
    ecs_meta_type_op_t* op = ecs_vector_get(ops, ecs_meta_type_op_t, index);
    ecs_assert(op != NULL, ECS_INTERNAL_ERROR, NULL);
    return op;
}

static
ecs_vector_t* serialize_primitive(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vector_t *ops)
{
    const EcsPrimitive *ptr = ecs_get(world, type, EcsPrimitive);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_meta_type_op_t *op = ops_add(&ops, primitive_to_op_kind(ptr->kind));
    op->offset = offset,
    op->type = type;

    return ops;
}

static
ecs_vector_t* serialize_array(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vector_t *ops)
{
    const EcsArray *ptr = ecs_get(world, type, EcsArray);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t first = ecs_vector_count(ops);
    ops = serialize_type(world, ptr->type, offset, ops);
    
    ecs_meta_type_op_t *op = ops_get(ops, first);
    op->offset = offset;
    op->count = ptr->count;
    op->op_count = ecs_vector_count(ops) - first;

    return ops;
}

static
ecs_vector_t* serialize_vector(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vector_t *ops)
{
    (void)world;
    
    ecs_meta_type_op_t *op = ops_add(&ops, EcsOpVector);
    op->offset = offset;
    op->count = 1;
    op->op_count = 1;
    op->type = type;

    return ops;
}

static
ecs_vector_t* serialize_struct(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vector_t *ops)
{
    const EcsStruct *ptr = ecs_get(world, type, EcsStruct);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t cur, first = ecs_vector_count(ops);
    ecs_meta_type_op_t *op = ops_add(&ops, EcsOpPush);
    op->offset = offset;
    op->type = type;

    ecs_member_t *members = ecs_vector_first(ptr->members, ecs_member_t);
    int32_t i, count = ecs_vector_count(ptr->members);

    for (i = 0; i < count; i ++) {
        ecs_member_t *member = &members[i];

        cur = ecs_vector_count(ops);
        ops = serialize_type(world, member->type, offset + member->offset, ops);
        
        op = ops_get(ops, cur);
        if (!op->type) {
            op->type = member->type;
        }

        if (op->count <= 1) {
            op->count = member->count;
        }

        op->name = member->name;
        op->op_count = ecs_vector_count(ops) - cur;
    }

    ops_add(&ops, EcsOpPop);
    ops_get(ops, first)->op_count = ecs_vector_count(ops) - first;

    return ops;
}

static
ecs_vector_t* serialize_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_size_t offset,
    ecs_vector_t *ops)
{
    const EcsMetaType *ptr = ecs_get(world, type, EcsMetaType);

    switch(ptr->kind) {
    case EcsPrimitiveType:
        ops = serialize_primitive(world, type, offset, ops);
        break;

    case EcsEnumType:
        // ops = serialize_enum(world, type, offset, ops);
        break;

    case EcsBitmaskType:
        // ops = serialize_bitmask(world, type, offset, ops);
        break;

    case EcsStructType:
        ops = serialize_struct(world, type, offset, ops);
        break;

    case EcsArrayType:
        ops = serialize_array(world, type, offset, ops);
        break;

    case EcsVectorType:
        ops = serialize_vector(world, type, offset, ops);
        break;
    }

    return ops;
}

void ecs_meta_type_serialized_init(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_vector_t *ops = serialize_type(world, e, 0, NULL);
        ecs_assert(ops != NULL, ECS_INTERNAL_ERROR, NULL);

        EcsMetaTypeSerialized *ptr = ecs_get_mut(
            world, e, EcsMetaTypeSerialized, NULL);
        if (ptr->ops) {
            ecs_vector_free(ptr->ops);
        }

        ptr->ops = ops;
    }
}

#endif
