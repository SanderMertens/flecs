/**
 * @file addons/meta/type_support/struct_ts.c
 * @brief Struct type support.
 */

#include "type_support.h"

#ifdef FLECS_META

static void flecs_struct_dtor(
    EcsStruct *ptr) 
{
    ecs_member_t *members = ecs_vec_first_t(&ptr->members, ecs_member_t);
    int32_t i, count = ecs_vec_count(&ptr->members);
    for (i = 0; i < count; i ++) {
        ecs_os_free(ECS_CONST_CAST(char*, members[i].name));
    }
    ecs_vec_fini_t(NULL, &ptr->members, ecs_member_t);
}

static ECS_COPY(EcsStruct, dst, src, {
    flecs_struct_dtor(dst);

    dst->members = ecs_vec_copy_t(NULL, &src->members, ecs_member_t);

    ecs_member_t *members = ecs_vec_first_t(&dst->members, ecs_member_t);
    int32_t m, count = ecs_vec_count(&dst->members);

    for (m = 0; m < count; m ++) {
        members[m].name = ecs_os_strdup(members[m].name);
    }
})

static ECS_MOVE(EcsStruct, dst, src, {
    flecs_struct_dtor(dst);
    dst->members = src->members;
    src->members = (ecs_vec_t){0};
})

static ECS_DTOR(EcsStruct, ptr, { flecs_struct_dtor(ptr); })

static
void flecs_set_struct_member(
    ecs_member_t *member,
    ecs_entity_t entity,
    const char *name,
    ecs_entity_t type,
    int32_t count,
    int32_t offset,
    ecs_entity_t unit,
    EcsMemberRanges *ranges)
{
    member->member = entity;
    member->type = type;
    member->count = count;
    member->unit = unit;
    member->offset = offset;

    ecs_os_strset(ECS_CONST_CAST(char**, &member->name), name);

    if (ranges) {
        member->range = ranges->value;
        member->error_range = ranges->error;
        member->warning_range = ranges->warning;
    } else {
        ecs_os_zeromem(&member->range);
        ecs_os_zeromem(&member->error_range);
        ecs_os_zeromem(&member->warning_range);
    }
}

static
int flecs_add_member_to_struct(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t member,
    EcsMember *m,
    EcsMemberRanges *ranges)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(member != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    const char *name = ecs_get_name(world, member);
    if (!name) {
        char *path = ecs_get_path(world, type);
        ecs_err("member for struct '%s' does not have a name", path);
        ecs_os_free(path);
        return -1;
    }

    if (!m->type) {
        char *path = ecs_get_path(world, member);
        ecs_err("member '%s' does not have a type", path);
        ecs_os_free(path);
        return -1;
    }

    if (ecs_get_typeid(world, m->type) == 0) {
        char *path = ecs_get_path(world, member);
        char *ent_path = ecs_get_path(world, m->type);
        ecs_err("member '%s.type' is '%s' which is not a type", path, ent_path);
        ecs_os_free(path);
        ecs_os_free(ent_path);
        return -1;
    }

    ecs_entity_t unit = m->unit;
    if (unit) {
        if (!ecs_has(world, unit, EcsUnit)) {
            ecs_err("entity '%s' for member '%s' is not a unit",
                ecs_get_name(world, unit), name);
            return -1;
        }

        if (ecs_has(world, m->type, EcsUnit) && m->type != unit) {
            ecs_err("unit mismatch for type '%s' and unit '%s' for member '%s'",
                ecs_get_name(world, m->type), ecs_get_name(world, unit), name);
            return -1;
        }
    } else {
        if (ecs_has(world, m->type, EcsUnit)) {
            ecs_entity_t unit_base = ecs_get_target_for(
                world, m->type, EcsIsA, EcsUnit);
            if (unit_base) {
                unit = m->unit = unit_base;
            } else {
                unit = m->unit = m->type;
            }
        }
    }

    EcsStruct *s = ecs_ensure(world, type, EcsStruct);
    ecs_assert(s != NULL, ECS_INTERNAL_ERROR, NULL);

    /* First check if member is already added to struct */
    ecs_member_t *members = ecs_vec_first_t(&s->members, ecs_member_t);
    int32_t i, count = ecs_vec_count(&s->members);
    for (i = 0; i < count; i ++) {
        if (members[i].member == member) {
            flecs_set_struct_member(&members[i], member, name, m->type, 
                m->count, m->offset, unit, ranges);
            break;
        }
    }

    /* If member wasn't added yet, add a new element to vector */
    if (i == count) {
        ecs_vec_init_if_t(&s->members, ecs_member_t);
        ecs_member_t *elem = ecs_vec_append_t(NULL, &s->members, ecs_member_t);
        elem->name = NULL;
        flecs_set_struct_member(elem, member, name, m->type, 
            m->count, m->offset, unit, ranges);

        /* Reobtain members array in case it was reallocated */
        members = ecs_vec_first_t(&s->members, ecs_member_t);
        count ++;
    }

    bool explicit_offset = m->offset || m->use_offset;

    /* Compute member offsets and size & alignment of struct */
    ecs_size_t size = 0;
    ecs_size_t alignment = 0;

    if (!explicit_offset) {
        for (i = 0; i < count; i ++) {
            ecs_member_t *elem = &members[i];

            ecs_assert(elem->name != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(elem->type != 0, ECS_INTERNAL_ERROR, NULL);

            /* Get component of member type to get its size & alignment */
            const EcsComponent *mbr_comp = ecs_get(world, elem->type, EcsComponent);
            if (!mbr_comp) {
                char *path = ecs_get_path(world, elem->type);
                ecs_err("member '%s' is not a type", path);
                ecs_os_free(path);
                return -1;
            }

            ecs_size_t member_size = mbr_comp->size;
            ecs_size_t member_alignment = mbr_comp->alignment;

            if (!member_size || !member_alignment) {
                char *path = ecs_get_path(world, elem->type);
                ecs_err("member '%s' has 0 size/alignment", path);
                ecs_os_free(path);
                return -1;
            }

            member_size *= elem->count ? elem->count : 1;
            size = ECS_ALIGN(size, member_alignment);
            elem->size = member_size;
            elem->offset = size;

            /* Synchronize offset with Member component */
            if (elem->member == member) {
                m->offset = elem->offset;
            } else {
                EcsMember *other = ecs_ensure(world, elem->member, EcsMember);
                other->offset = elem->offset;
            }

            size += member_size;

            if (member_alignment > alignment) {
                alignment = member_alignment;
            }
        }
    } else {
        /* If members have explicit offsets, we can't rely on computed 
         * size/alignment values. Calculate size as if this is the last member
         * instead, since this will validate if the member fits in the struct.
         * It doesn't matter if the size is smaller than the actual struct size
         * because flecs_init_type function compares computed size with actual
         * (component) size to determine if the type is partial. */
        ecs_member_t *elem = &members[i];

        ecs_assert(elem->name != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(elem->type != 0, ECS_INTERNAL_ERROR, NULL);

        /* Get component of member type to get its size & alignment */
        const EcsComponent *mbr_comp = ecs_get(world, elem->type, EcsComponent);
        if (!mbr_comp) {
            char *path = ecs_get_path(world, elem->type);
            ecs_err("member '%s' is not a type", path);
            ecs_os_free(path);
            return -1;
        }

        ecs_size_t member_size = mbr_comp->size;
        ecs_size_t member_alignment = mbr_comp->alignment;

        if (!member_size || !member_alignment) {
            char *path = ecs_get_path(world, elem->type);
            ecs_err("member '%s' has 0 size/alignment", path);
            ecs_os_free(path);
            return -1;
        }

        member_size *= elem->count ? elem->count : 1;
        elem->size = member_size;
        size = elem->offset + member_size;

        const EcsComponent* comp = ecs_get(world, type, EcsComponent);
        if (comp) {
            alignment = comp->alignment;
        } else {
            alignment = member_alignment;
        }
    }

    if (size == 0) {
        ecs_err("struct '%s' has 0 size", ecs_get_name(world, type));
        return -1;
    }

    if (alignment == 0) {
        ecs_err("struct '%s' has 0 alignment", ecs_get_name(world, type));
        return -1;
    }

    /* Align struct size to struct alignment */
    size = ECS_ALIGN(size, alignment);

    ecs_modified(world, type, EcsStruct);

    /* Do this last as it triggers the update of EcsTypeSerializer */
    if (flecs_init_type(world, type, EcsStructType, size, alignment)) {
        return -1;
    }

    /* If current struct is also a member, assign to itself */
    if (ecs_has(world, type, EcsMember)) {
        EcsMember *type_mbr = ecs_ensure(world, type, EcsMember);
        ecs_assert(type_mbr != NULL, ECS_INTERNAL_ERROR, NULL);

        type_mbr->type = type;
        type_mbr->count = 0;

        ecs_modified(world, type, EcsMember);
    }

    return 0;
}

static
void flecs_set_member(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMember *member = ecs_field(it, EcsMember, 0);
    EcsMemberRanges *ranges = ecs_table_get_id(world, it->table, 
        ecs_id(EcsMemberRanges), it->offset);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
        if (!parent) {
            ecs_err("missing parent for member '%s'", ecs_get_name(world, e));
            continue;
        }

        flecs_add_member_to_struct(world, parent, e, &member[i], 
            ranges ? &ranges[i] : NULL);
    }
}

static
void flecs_set_member_ranges(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMemberRanges *ranges = ecs_field(it, EcsMemberRanges, 0);
    EcsMember *member = ecs_table_get_id(world, it->table, 
        ecs_id(EcsMember), it->offset);
    if (!member) {
        return;
    }

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
        if (!parent) {
            ecs_err("missing parent for member '%s'", ecs_get_name(world, e));
            continue;
        }

        flecs_add_member_to_struct(world, parent, e, &member[i], 
            &ranges[i]);
    }
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

        ecs_entity_t m = ecs_new_from_path(world, t, m_desc->name);

        ecs_set(world, m, EcsMember, {
            .type = m_desc->type, 
            .count = m_desc->count,
            .offset = m_desc->offset,
            .unit = m_desc->unit,
            .use_offset = m_desc->use_offset
        });

        EcsMemberRanges *ranges = NULL;
        const ecs_member_value_range_t *range = &m_desc->range;
        const ecs_member_value_range_t *error = &m_desc->error_range;
        const ecs_member_value_range_t *warning = &m_desc->warning_range;
        if (ECS_NEQ(range->min, range->max)) {
            ranges = ecs_ensure(world, m, EcsMemberRanges);
            if (range->min > range->max) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("member '%s' has an invalid value range [%f..%f]",
                    member_name, range->min, range->max);
                ecs_os_free(member_name);
                goto error;
            }
            ranges->value.min = range->min;
            ranges->value.max = range->max;
        }
        if (ECS_NEQ(error->min, error->max)) {
            if (error->min > error->max) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("member '%s' has an invalid error range [%f..%f]",
                    member_name, error->min, error->max);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(error, range)) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("error range of member '%s' overlaps with value range",
                    member_name);
                ecs_os_free(member_name);
                goto error;
            }
            if (!ranges) {
                ranges = ecs_ensure(world, m, EcsMemberRanges);
            }
            ranges->error.min = error->min;
            ranges->error.max = error->max;
        }

        if (ECS_NEQ(warning->min, warning->max)) {
            if (warning->min > warning->max) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("member '%s' has an invalid warning range [%f..%f]",
                    member_name, warning->min, warning->max);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(warning, range)) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("warning range of member '%s' overlaps with value "
                    "range", member_name);
                ecs_os_free(member_name);
                goto error;
            }
            if (flecs_member_range_overlaps(warning, error)) {
                char *member_name = ecs_get_path(world, m);
                ecs_err("warning range of member '%s' overlaps with error "
                    "range", member_name);
                ecs_os_free(member_name);
                goto error;
            }

            if (!ranges) {
                ranges = ecs_ensure(world, m, EcsMemberRanges);
            }
            ranges->warning.min = warning->min;
            ranges->warning.max = warning->max;
        }

        if (ranges && !flecs_type_is_number(world, m_desc->type)) {
            char *member_name = ecs_get_path(world, m);
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

void flecs_meta_struct_init(
    ecs_world_t *world)
{
    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsMember),
            .name = "member", .symbol = "EcsMember",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsMember),
        .type.alignment = ECS_ALIGNOF(EcsMember)
    });

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsMemberRanges),
            .name = "member_ranges", .symbol = "EcsMemberRanges",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsMemberRanges),
        .type.alignment = ECS_ALIGNOF(EcsMemberRanges)
    });

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsStruct),
            .name = "struct", .symbol = "EcsStruct",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsStruct),
        .type.alignment = ECS_ALIGNOF(EcsStruct)
    });

    ecs_set_hooks(world, EcsStruct, { 
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsStruct),
        .copy = ecs_copy(EcsStruct),
        .dtor = ecs_dtor(EcsStruct)
    });

    ecs_set_hooks(world, EcsMember, { 
        .ctor = flecs_default_ctor,
    });

    ecs_set_hooks(world, EcsMemberRanges, { 
        .ctor = flecs_default_ctor
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsMember) },
        .events = {EcsOnSet},
        .callback = flecs_set_member
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsMemberRanges) },
        .events = {EcsOnSet},
        .callback = flecs_set_member_ranges
    });

    ecs_set(world, ecs_id(EcsStruct),  EcsDefaultChildComponent, {ecs_id(EcsMember)});
    ecs_set(world, ecs_id(EcsMember),  EcsDefaultChildComponent, {ecs_id(EcsMember)});
}

#endif
