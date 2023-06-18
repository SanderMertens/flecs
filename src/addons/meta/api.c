/**
 * @file meta/api.c
 * @brief API for creating entities with reflection data.
 */

#include "meta.h"

#ifdef FLECS_META

static
bool flecs_type_is_number(
    ecs_world_t *world,
    ecs_entity_t type)
{
    const EcsPrimitive *p = ecs_get(world, type, EcsPrimitive);
    if (!p) {
        return false;
    }

    switch(p->kind) {
    case EcsChar:
    case EcsU8:
    case EcsU16:
    case EcsU32:
    case EcsU64:
    case EcsI8:
    case EcsI16:
    case EcsI32:
    case EcsI64:
    case EcsF32:
    case EcsF64:
        return true;
    default:
        return false;
    }
}

ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc)
{
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsPrimitive, { desc->kind });

    return t;
}

ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc)
{
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_add(world, t, EcsEnum);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_enum_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity(world, {
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
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_add(world, t, EcsBitmask);

    ecs_entity_t old_scope = ecs_set_scope(world, t);

    int i;
    for (i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i ++) {
        const ecs_bitmask_constant_t *m_desc = &desc->constants[i];
        if (!m_desc->name) {
            break;
        }

        ecs_entity_t c = ecs_entity(world, {
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
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
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
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
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
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
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

        ecs_entity_t m = ecs_entity(world, {
            .name = m_desc->name
        });

        ecs_set(world, m, EcsMember, {
            .type = m_desc->type, 
            .count = m_desc->count,
            .offset = m_desc->offset,
            .unit = m_desc->unit
        });

        EcsMemberRanges *ranges = NULL;
        if (m_desc->error.min != m_desc->error.max) {
            if (m_desc->error.min > m_desc->error.max) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("member '%s' has an invalid error range [%d..%d]",
                    member_name, m_desc->error.min, m_desc->error.max);
                ecs_os_free(member_name);
                ecs_delete(world, t);
                return 0;
            }
            ranges = ecs_get_mut(world, m, EcsMemberRanges);
            ranges->error.min = m_desc->error.min;
            ranges->error.max = m_desc->error.max;
        }

        if (m_desc->warning.min != m_desc->warning.max) {
            if (m_desc->warning.min > m_desc->warning.max) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("member '%s' has an invalid warning range [%d..%d]",
                    member_name, m_desc->warning.min, m_desc->warning.max);
                ecs_os_free(member_name);
                ecs_delete(world, t);
                return 0;
            }
            if (ranges) {
                if (m_desc->warning.min < m_desc->error.min || 
                    m_desc->warning.max > m_desc->error.max) 
                {
                    char *member_name = ecs_get_fullpath(world, m);
                    ecs_err("warning range of member '%s' overlaps with error "
                        "range", member_name);
                    ecs_os_free(member_name);
                    ecs_delete(world, t);
                    return 0;
                }
            } else {
                ranges = ecs_get_mut(world, m, EcsMemberRanges);
            }
            ranges->warning.min = m_desc->warning.min;
            ranges->warning.max = m_desc->warning.max;
        }

        if (ranges && !flecs_type_is_number(world, m_desc->type)) {
            char *member_name = ecs_get_fullpath(world, m);
            ecs_err("member '%s' has an error/warning range, but is not a "
                    "number", member_name);
            ecs_os_free(member_name);
            ecs_delete(world, t);
            return 0;
        }
    }

    ecs_set_scope(world, old_scope);

    if (i == 0) {
        ecs_err("struct '%s' has no members", ecs_get_name(world, t));
        ecs_delete(world, t);
        return 0;
    }

    if (!ecs_has(world, t, EcsStruct)) {
        /* Invalid members */
        ecs_delete(world, t);
        return 0;
    }

    return t;
}

ecs_entity_t ecs_opaque_init(
    ecs_world_t *world,
    const ecs_opaque_desc_t *desc)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->type.as_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set_ptr(world, t, EcsOpaque, &desc->type);

    return t;
}

ecs_entity_t ecs_unit_init(
    ecs_world_t *world,
    const ecs_unit_desc_t *desc)
{
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_entity_t quantity = desc->quantity;
    if (quantity) {
        if (!ecs_has_id(world, quantity, EcsQuantity)) {
            ecs_err("entity '%s' for unit '%s' is not a quantity",
                ecs_get_name(world, quantity), ecs_get_name(world, t));
            goto error;
        }

        ecs_add_pair(world, t, EcsQuantity, desc->quantity);
    } else {
        ecs_remove_pair(world, t, EcsQuantity, EcsWildcard);
    }

    EcsUnit *value = ecs_get_mut(world, t, EcsUnit);
    value->base = desc->base;
    value->over = desc->over;
    value->translation = desc->translation;
    value->prefix = desc->prefix;
    ecs_os_strset(&value->symbol, desc->symbol);

    if (!flecs_unit_validate(world, t, value)) {
        goto error;
    }

    ecs_modified(world, t, EcsUnit);

    return t;
error:
    if (t) {
        ecs_delete(world, t);
    }
    return 0;
}

ecs_entity_t ecs_unit_prefix_init(
    ecs_world_t *world,
    const ecs_unit_prefix_desc_t *desc)
{
    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsUnitPrefix, {
        .symbol = (char*)desc->symbol,
        .translation = desc->translation
    });

    return t;
}

ecs_entity_t ecs_quantity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_entity_t t = ecs_entity_init(world, desc);
    if (!t) {
        return 0;
    }

    ecs_add_id(world, t, EcsQuantity);

    return t;
}

#endif
