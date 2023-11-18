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

    case EcsBool:
    case EcsByte:
    case EcsUPtr:
    case EcsIPtr:
    case EcsString:
    case EcsEntity:
    case EcsId:
        return false;
    default:
        ecs_abort(ECS_INVALID_PARAMETER, NULL);
    }
}

ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsPrimitive, { desc->kind });

    flecs_resume_readonly(world, &rs);
    return t;
}

ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

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
    flecs_resume_readonly(world, &rs);

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
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

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
    flecs_resume_readonly(world, &rs);

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
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsArray, {
        .type = desc->type,
        .count = desc->count
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsVector, {
        .type = desc->type
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

static
bool flecs_member_range_overlaps(
    const ecs_member_value_range_t *range,
    const ecs_member_value_range_t *with)
{
    if (ECS_EQ(with->min, with->max)) {
        return false;
    }

    if (ECS_EQ(range->min, range->max)) {
        return false;
    }

    if (range->min < with->min || 
        range->max > with->max) 
    {
        return true;
    }

    return false;
}

ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

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
            goto error;
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
        const ecs_member_value_range_t *range = &m_desc->range;
        const ecs_member_value_range_t *error = &m_desc->error_range;
        const ecs_member_value_range_t *warning = &m_desc->warning_range;
        if (ECS_NEQ(range->min, range->max)) {
            ranges = ecs_get_mut(world, m, EcsMemberRanges);
            if (range->min > range->max) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("member '%s' has an invalid value range [%d..%d]",
                    member_name, range->min, range->max);
                ecs_os_free(member_name);
                goto error;
            }
            ranges->value.min = range->min;
            ranges->value.max = range->max;
        }
        if (ECS_NEQ(error->min, error->max)) {
            if (error->min > error->max) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("member '%s' has an invalid error range [%d..%d]",
                    member_name, error->min, error->max);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(error, range)) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("error range of member '%s' overlaps with value range",
                    member_name);
                ecs_os_free(member_name);
                goto error;
            }
            if (!ranges) {
                ranges = ecs_get_mut(world, m, EcsMemberRanges);
            }
            ranges->error.min = error->min;
            ranges->error.max = error->max;
        }

        if (ECS_NEQ(warning->min, warning->max)) {
            if (warning->min > warning->max) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("member '%s' has an invalid warning range [%d..%d]",
                    member_name, warning->min, warning->max);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(warning, range)) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("warning range of member '%s' overlaps with value "
                    "range", member_name);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(warning, error)) {
                char *member_name = ecs_get_fullpath(world, m);
                ecs_err("warning range of member '%s' overlaps with error "
                    "range", member_name);
                ecs_os_free(member_name);
                goto error;
            }

            if (!ranges) {
                ranges = ecs_get_mut(world, m, EcsMemberRanges);
            }
            ranges->warning.min = warning->min;
            ranges->warning.max = warning->max;
        }

        if (ranges && !flecs_type_is_number(world, m_desc->type)) {
            char *member_name = ecs_get_fullpath(world, m);
            ecs_err("member '%s' has an value/error/warning range, but is not a "
                    "number", member_name);
            ecs_os_free(member_name);
            goto error;
        }

        if (ranges) {
            ecs_modified(world, m, EcsMemberRanges);
        }
    }

    ecs_set_scope(world, old_scope);
    flecs_resume_readonly(world, &rs);

    if (i == 0) {
        ecs_err("struct '%s' has no members", ecs_get_name(world, t));
        goto error;
    }

    if (!ecs_has(world, t, EcsStruct)) {
        goto error;
    }

    return t;
error:
    flecs_resume_readonly(world, &rs);
    if (t) {
        ecs_delete(world, t);
    }
    return 0;
}

ecs_entity_t ecs_opaque_init(
    ecs_world_t *world,
    const ecs_opaque_desc_t *desc)
{
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->type.as_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set_ptr(world, t, EcsOpaque, &desc->type);

    flecs_resume_readonly(world, &rs);

    return t;
}

ecs_entity_t ecs_unit_init(
    ecs_world_t *world,
    const ecs_unit_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

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

    flecs_resume_readonly(world, &rs);
    return t;
error:
    if (t) {
        ecs_delete(world, t);
    }
    flecs_resume_readonly(world, &rs);
    return 0;
}

ecs_entity_t ecs_unit_prefix_init(
    ecs_world_t *world,
    const ecs_unit_prefix_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsUnitPrefix, {
        .symbol = ECS_CONST_CAST(char*, desc->symbol),
        .translation = desc->translation
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

ecs_entity_t ecs_quantity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = ecs_entity_init(world, desc);
    if (!t) {
        return 0;
    }

    ecs_add_id(world, t, EcsQuantity);

    flecs_resume_readonly(world, &rs);

    return t;
}

#endif
