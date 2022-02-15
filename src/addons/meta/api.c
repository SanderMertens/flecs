#include "meta.h"

#ifdef FLECS_META

ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_set(world, t, EcsPrimitive, { desc->kind });

    return t;
}

ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_add(world, t, EcsEnum);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_enum_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = m_desc->name
        });

        if (!m_desc->value) {
            ecs_add_id(world, c, EcsConstant);
        } else {
            ecs_set_pair_object(world, c, EcsConstant, ecs_i32_t, 
                {m_desc->value});
        }
    }

    ecs_set_scope(world, old_scope);

    if (i == 0) {
        ecs_err("enum '%s' has no constants", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

ecs_entity_t ecs_bitmask_init(
    ecs_world_t *world,
    const ecs_bitmask_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_add(world, t, EcsBitmask);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_bitmask_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = m_desc->name
        });

        if (!m_desc->value) {
            ecs_add_id(world, c, EcsConstant);
        } else {
            ecs_set_pair_object(world, c, EcsConstant, ecs_u32_t, 
                {m_desc->value});
        }
    }

    ecs_set_scope(world, old_scope);

    if (i == 0) {
        ecs_err("bitmask '%s' has no constants", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

ecs_entity_t ecs_array_init(
    ecs_world_t *world,
    const ecs_array_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_set(world, t, EcsArray, {
        .type = desc->type,
        .count = desc->count
    });

    return t;
}

ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_set(world, t, EcsVector, {
        .type = desc->type
    });

    return t;
}

ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, &desc->entity);
    if (!t) {
        return 0;
    }

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_member_t *m_desc = &desc->members[i];
        if (!m_desc->type) {
            break;
        }

        if (!m_desc->name) {
            ecs_err("member %d of struct '%s' does not have a name", i, 
                ecs_get_name(world, t));
            ecs_delete(world, t);
            return 0;
        }

        ecs_entity_t m = ecs_entity_init(world, &(ecs_entity_desc_t) {
            .name = m_desc->name
        });

        ecs_set(world, m, EcsMember, {
            .type = m_desc->type, 
            .count = m_desc->count
        });
    }

    ecs_set_scope(world, old_scope);

    if (i == 0) {
        ecs_err("struct '%s' has no members", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

#endif
