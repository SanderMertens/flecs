/**
 * @file meta/meta.c
 * @brief Meta addon.
 */

#include "meta.h"

#ifdef FLECS_META

/* ecs_string_t lifecycle */

static ECS_COPY(ecs_string_t, dst, src, {
    ecs_os_free(*(ecs_string_t*)dst);
    *(ecs_string_t*)dst = ecs_os_strdup(*(ecs_string_t*)src);
})

static ECS_MOVE(ecs_string_t, dst, src, {
    ecs_os_free(*(ecs_string_t*)dst);
    *(ecs_string_t*)dst = *(ecs_string_t*)src;
    *(ecs_string_t*)src = NULL;
})

static ECS_DTOR(ecs_string_t, ptr, { 
    ecs_os_free(*(ecs_string_t*)ptr);
    *(ecs_string_t*)ptr = NULL;
})


/* EcsMetaTypeSerialized lifecycle */

void ecs_meta_dtor_serialized(
    EcsMetaTypeSerialized *ptr) 
{
    int32_t i, count = ecs_vector_count(ptr->ops);
    ecs_meta_type_op_t *ops = ecs_vector_first(ptr->ops, ecs_meta_type_op_t);
    
    for (i = 0; i < count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];
        if (op->members) {
            flecs_name_index_free(op->members);
        }
    }

    ecs_vector_free(ptr->ops);
}

static ECS_COPY(EcsMetaTypeSerialized, dst, src, {
    ecs_meta_dtor_serialized(dst);

    dst->ops = ecs_vector_copy(src->ops, ecs_meta_type_op_t);

    int32_t o, count = ecs_vector_count(dst->ops);
    ecs_meta_type_op_t *ops = ecs_vector_first(dst->ops, ecs_meta_type_op_t);
    
    for (o = 0; o < count; o ++) {
        ecs_meta_type_op_t *op = &ops[o];
        if (op->members) {
            op->members = flecs_name_index_copy(op->members);
        }
    }
})

static ECS_MOVE(EcsMetaTypeSerialized, dst, src, {
    ecs_meta_dtor_serialized(dst);
    dst->ops = src->ops;
    src->ops = NULL;
})

static ECS_DTOR(EcsMetaTypeSerialized, ptr, { 
    ecs_meta_dtor_serialized(ptr);
})


/* EcsStruct lifecycle */

static void flecs_struct_dtor(
    EcsStruct *ptr) 
{
    ecs_member_t *members = ecs_vector_first(ptr->members, ecs_member_t);
    int32_t i, count = ecs_vector_count(ptr->members);
    for (i = 0; i < count; i ++) {
        ecs_os_free((char*)members[i].name);
    }
    ecs_vector_free(ptr->members);
}

static ECS_COPY(EcsStruct, dst, src, {
    flecs_struct_dtor(dst);

    dst->members = ecs_vector_copy(src->members, ecs_member_t);

    ecs_member_t *members = ecs_vector_first(dst->members, ecs_member_t);
    int32_t m, count = ecs_vector_count(dst->members);

    for (m = 0; m < count; m ++) {
        members[m].name = ecs_os_strdup(members[m].name);
    }
})

static ECS_MOVE(EcsStruct, dst, src, {
    flecs_struct_dtor(dst);
    dst->members = src->members;
    src->members = NULL;
})

static ECS_DTOR(EcsStruct, ptr, { flecs_struct_dtor(ptr); })


/* EcsEnum lifecycle */

static void flecs_constants_dtor(
    ecs_map_t *constants) 
{
    ecs_map_iter_t it = ecs_map_iter(constants);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t *c = ecs_map_ptr(&it);
        ecs_os_free((char*)c->name);
        ecs_os_free(c);
    }
    ecs_map_fini(constants);
}

static void flecs_constants_copy(
    ecs_map_t *dst,
    ecs_map_t *src)
{
    ecs_map_copy(dst, src);

    ecs_map_iter_t it = ecs_map_iter(dst);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t **r = ecs_map_ref(&it, ecs_enum_constant_t);
        ecs_enum_constant_t *src_c = r[0];
        ecs_enum_constant_t *dst_c = ecs_os_calloc_t(ecs_enum_constant_t);
        *dst_c = *src_c;
        dst_c->name = ecs_os_strdup(dst_c->name);
        r[0] = dst_c;
    }
}

static ECS_COPY(EcsEnum, dst, src, {
    flecs_constants_dtor(&dst->constants);
    flecs_constants_copy(&dst->constants, &src->constants);
})

static ECS_MOVE(EcsEnum, dst, src, {
    flecs_constants_dtor(&dst->constants);
    dst->constants = src->constants;
    ecs_os_zeromem(&src->constants);
})

static ECS_DTOR(EcsEnum, ptr, { flecs_constants_dtor(&ptr->constants); })


/* EcsBitmask lifecycle */

static ECS_COPY(EcsBitmask, dst, src, {
    /* bitmask constant & enum constant have the same layout */
    flecs_constants_dtor(&dst->constants);
    flecs_constants_copy(&dst->constants, &src->constants);
})

static ECS_MOVE(EcsBitmask, dst, src, {
    flecs_constants_dtor(&dst->constants);
    dst->constants = src->constants;
    ecs_os_zeromem(&src->constants);
})

static ECS_DTOR(EcsBitmask, ptr, { flecs_constants_dtor(&ptr->constants); })


/* EcsUnit lifecycle */

static void dtor_unit(
    EcsUnit *ptr) 
{
    ecs_os_free(ptr->symbol);
}

static ECS_COPY(EcsUnit, dst, src, {
    dtor_unit(dst);
    dst->symbol = ecs_os_strdup(src->symbol);
    dst->base = src->base;
    dst->over = src->over;
    dst->prefix = src->prefix;
    dst->translation = src->translation;
})

static ECS_MOVE(EcsUnit, dst, src, {
    dtor_unit(dst);
    dst->symbol = src->symbol;
    dst->base = src->base;
    dst->over = src->over;
    dst->prefix = src->prefix;
    dst->translation = src->translation;

    src->symbol = NULL;
    src->base = 0;
    src->over = 0;
    src->prefix = 0;
    src->translation = (ecs_unit_translation_t){0};
})

static ECS_DTOR(EcsUnit, ptr, { dtor_unit(ptr); })


/* EcsUnitPrefix lifecycle */

static void dtor_unit_prefix(
    EcsUnitPrefix *ptr) 
{
    ecs_os_free(ptr->symbol);
}

static ECS_COPY(EcsUnitPrefix, dst, src, {
    dtor_unit_prefix(dst);
    dst->symbol = ecs_os_strdup(src->symbol);
    dst->translation = src->translation;
})

static ECS_MOVE(EcsUnitPrefix, dst, src, {
    dtor_unit_prefix(dst);
    dst->symbol = src->symbol;
    dst->translation = src->translation;

    src->symbol = NULL;
    src->translation = (ecs_unit_translation_t){0};
})

static ECS_DTOR(EcsUnitPrefix, ptr, { dtor_unit_prefix(ptr); })


/* Type initialization */

static
int flecs_init_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_type_kind_t kind,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);

    EcsMetaType *meta_type = ecs_get_mut(world, type, EcsMetaType);
    if (meta_type->kind == 0) {
        meta_type->existing = ecs_has(world, type, EcsComponent);

        /* Ensure that component has a default constructor, to prevent crashing
         * serializers on uninitialized values. */
        ecs_type_info_t *ti = flecs_type_info_ensure(world, type);
        if (!ti->hooks.ctor) {
            ti->hooks.ctor = ecs_default_ctor;
        }
    } else {
        if (meta_type->kind != kind) {
            ecs_err("type '%s' reregistered with different kind", 
                ecs_get_name(world, type));
            return -1;
        }
    }

    if (!meta_type->existing) {
        EcsComponent *comp = ecs_get_mut(world, type, EcsComponent);
        comp->size = size;
        comp->alignment = alignment;
        ecs_modified(world, type, EcsComponent);
    } else {
        const EcsComponent *comp = ecs_get(world, type, EcsComponent);
        if (comp->size < size) {
            ecs_err("computed size (%d) for '%s' is larger than actual type (%d)", 
                size, ecs_get_name(world, type), comp->size);
            return -1;
        }
        if (comp->alignment < alignment) {
            ecs_err("computed alignment (%d) for '%s' is larger than actual type (%d)", 
                alignment, ecs_get_name(world, type), comp->alignment);
            return -1;
        }
        if (comp->size == size && comp->alignment != alignment) {
            ecs_err("computed size for '%s' matches with actual type but "
                "alignment is different (%d vs. %d)", ecs_get_name(world, type),
                    alignment, comp->alignment);
            return -1;
        }
        
        meta_type->partial = comp->size != size;
    }

    meta_type->kind = kind;
    meta_type->size = size;
    meta_type->alignment = alignment;
    ecs_modified(world, type, EcsMetaType);

    return 0;
}

#define init_type_t(world, type, kind, T) \
    flecs_init_type(world, type, kind, ECS_SIZEOF(T), ECS_ALIGNOF(T))

static
void flecs_set_struct_member(
    ecs_member_t *member,
    ecs_entity_t entity,
    const char *name,
    ecs_entity_t type,
    int32_t count,
    int32_t offset,
    ecs_entity_t unit)
{
    member->member = entity;
    member->type = type;
    member->count = count;
    member->unit = unit;
    member->offset = offset;

    if (!count) {
        member->count = 1;
    }

    ecs_os_strset((char**)&member->name, name);
}

static
int flecs_add_member_to_struct(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t member,
    EcsMember *m)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(member != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    const char *name = ecs_get_name(world, member);
    if (!name) {
        char *path = ecs_get_fullpath(world, type);
        ecs_err("member for struct '%s' does not have a name", path);
        ecs_os_free(path);
        return -1;
    }

    if (!m->type) {
        char *path = ecs_get_fullpath(world, member);
        ecs_err("member '%s' does not have a type", path);
        ecs_os_free(path);
        return -1;
    }

    if (ecs_get_typeid(world, m->type) == 0) {
        char *path = ecs_get_fullpath(world, member);
        char *ent_path = ecs_get_fullpath(world, m->type);
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
            unit = m->type;
            m->unit = unit;
        }
    }

    EcsStruct *s = ecs_get_mut(world, type, EcsStruct);
    ecs_assert(s != NULL, ECS_INTERNAL_ERROR, NULL);

    /* First check if member is already added to struct */
    ecs_member_t *members = ecs_vector_first(s->members, ecs_member_t);
    int32_t i, count = ecs_vector_count(s->members);
    for (i = 0; i < count; i ++) {
        if (members[i].member == member) {
            flecs_set_struct_member(
                &members[i], member, name, m->type, m->count, m->offset, unit);
            break;
        }
    }

    /* If member wasn't added yet, add a new element to vector */
    if (i == count) {
        ecs_member_t *elem = ecs_vector_add(&s->members, ecs_member_t);
        elem->name = NULL;
        flecs_set_struct_member(elem, member, name, m->type, 
            m->count, m->offset, unit);

        /* Reobtain members array in case it was reallocated */
        members = ecs_vector_first(s->members, ecs_member_t);
        count ++;
    }

    bool explicit_offset = false;
    if (m->offset) {
        explicit_offset = true;
    }

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
                char *path = ecs_get_fullpath(world, member);
                ecs_err("member '%s' is not a type", path);
                ecs_os_free(path);
                return -1;
            }

            ecs_size_t member_size = mbr_comp->size;
            ecs_size_t member_alignment = mbr_comp->alignment;

            if (!member_size || !member_alignment) {
                char *path = ecs_get_fullpath(world, member);
                ecs_err("member '%s' has 0 size/alignment");
                ecs_os_free(path);
                return -1;
            }

            member_size *= elem->count;
            size = ECS_ALIGN(size, member_alignment);
            elem->size = member_size;
            elem->offset = size;

            size += member_size;

            if (member_alignment > alignment) {
                alignment = member_alignment;
            }
        }
    } else {
        /* If members have explicit offsets, we can't rely on computed 
         * size/alignment values. Grab size of just added member instead. It
         * doesn't matter if the size doesn't correspond to the actual struct
         * size. The flecs_init_type function compares computed size with actual
         * (component) size to determine if the type is partial. */
        const EcsComponent *cptr = ecs_get(world, m->type, EcsComponent);
        ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);
        size = cptr->size;
        alignment = cptr->alignment;
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

    /* Do this last as it triggers the update of EcsMetaTypeSerialized */
    if (flecs_init_type(world, type, EcsStructType, size, alignment)) {
        return -1;
    }

    /* If current struct is also a member, assign to itself */
    if (ecs_has(world, type, EcsMember)) {
        EcsMember *type_mbr = ecs_get_mut(world, type, EcsMember);
        ecs_assert(type_mbr != NULL, ECS_INTERNAL_ERROR, NULL);

        type_mbr->type = type;
        type_mbr->count = 1;

        ecs_modified(world, type, EcsMember);
    }

    return 0;
}

static
int flecs_add_constant_to_enum(
    ecs_world_t *world, 
    ecs_entity_t type, 
    ecs_entity_t e,
    ecs_id_t constant_id)
{
    EcsEnum *ptr = ecs_get_mut(world, type, EcsEnum);
    
    /* Remove constant from map if it was already added */
    ecs_map_iter_t it = ecs_map_iter(&ptr->constants);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t *c = ecs_map_ptr(&it);
        if (c->constant == e) {
            ecs_os_free((char*)c->name);
            ecs_map_remove_free(&ptr->constants, ecs_map_key(&it));
        }
    }

    /* Check if constant sets explicit value */
    int32_t value = 0;
    bool value_set = false;
    if (ecs_id_is_pair(constant_id)) {
        if (ecs_pair_second(world, constant_id) != ecs_id(ecs_i32_t)) {
            char *path = ecs_get_fullpath(world, e);
            ecs_err("expected i32 type for enum constant '%s'", path);
            ecs_os_free(path);
            return -1;
        }

        const int32_t *value_ptr = ecs_get_pair_object(
            world, e, EcsConstant, ecs_i32_t);
        ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        value = *value_ptr;
        value_set = true;
    }

    /* Make sure constant value doesn't conflict if set / find the next value */
    it = ecs_map_iter(&ptr->constants);
    while (ecs_map_next(&it)) {
        ecs_enum_constant_t *c = ecs_map_ptr(&it);
        if (value_set) {
            if (c->value == value) {
                char *path = ecs_get_fullpath(world, e);
                ecs_err("conflicting constant value for '%s' (other is '%s')",
                    path, c->name);
                ecs_os_free(path);
                return -1;
            }
        } else {
            if (c->value >= value) {
                value = c->value + 1;
            }
        }
    }

    ecs_map_init_if(&ptr->constants, &world->allocator);
    ecs_enum_constant_t *c = ecs_map_insert_alloc_t(&ptr->constants, 
        ecs_enum_constant_t, (ecs_map_key_t)value);
    c->name = ecs_os_strdup(ecs_get_name(world, e));
    c->value = value;
    c->constant = e;

    ecs_i32_t *cptr = ecs_get_mut_pair_object(
        world, e, EcsConstant, ecs_i32_t);
    ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);
    cptr[0] = value;

    return 0;
}

static
int flecs_add_constant_to_bitmask(
    ecs_world_t *world, 
    ecs_entity_t type, 
    ecs_entity_t e,
    ecs_id_t constant_id)
{
    EcsBitmask *ptr = ecs_get_mut(world, type, EcsBitmask);
    
    /* Remove constant from map if it was already added */
    ecs_map_iter_t it = ecs_map_iter(&ptr->constants);
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        if (c->constant == e) {
            ecs_os_free((char*)c->name);
            ecs_map_remove_free(&ptr->constants, ecs_map_key(&it));
        }
    }

    /* Check if constant sets explicit value */
    uint32_t value = 1;
    if (ecs_id_is_pair(constant_id)) {
        if (ecs_pair_second(world, constant_id) != ecs_id(ecs_u32_t)) {
            char *path = ecs_get_fullpath(world, e);
            ecs_err("expected u32 type for bitmask constant '%s'", path);
            ecs_os_free(path);
            return -1;
        }

        const uint32_t *value_ptr = ecs_get_pair_object(
            world, e, EcsConstant, ecs_u32_t);
        ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        value = *value_ptr;
    } else {
        value = 1u << (ecs_u32_t)ecs_map_count(&ptr->constants);
    }

    /* Make sure constant value doesn't conflict */
    it = ecs_map_iter(&ptr->constants);
    while  (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
        if (c->value == value) {
            char *path = ecs_get_fullpath(world, e);
            ecs_err("conflicting constant value for '%s' (other is '%s')",
                path, c->name);
            ecs_os_free(path);
            return -1;
        }
    }

    ecs_map_init_if(&ptr->constants, &world->allocator);

    ecs_bitmask_constant_t *c = ecs_map_insert_alloc_t(&ptr->constants, 
        ecs_bitmask_constant_t, value);
    c->name = ecs_os_strdup(ecs_get_name(world, e));
    c->value = value;
    c->constant = e;

    ecs_u32_t *cptr = ecs_get_mut_pair_object(
        world, e, EcsConstant, ecs_u32_t);
    ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);
    cptr[0] = value;

    return 0;
}

static
void flecs_set_primitive(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsPrimitive *type = ecs_field(it, EcsPrimitive, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        switch(type->kind) {
        case EcsBool:
            init_type_t(world, e, EcsPrimitiveType, bool);
            break;
        case EcsChar:
            init_type_t(world, e, EcsPrimitiveType, char);
            break;
        case EcsByte:
            init_type_t(world, e, EcsPrimitiveType, bool);
            break;
        case EcsU8:
            init_type_t(world, e, EcsPrimitiveType, uint8_t);
            break;
        case EcsU16:
            init_type_t(world, e, EcsPrimitiveType, uint16_t);
            break;
        case EcsU32:
            init_type_t(world, e, EcsPrimitiveType, uint32_t);
            break;
        case EcsU64:
            init_type_t(world, e, EcsPrimitiveType, uint64_t);
            break;
        case EcsI8:
            init_type_t(world, e, EcsPrimitiveType, int8_t);
            break;
        case EcsI16:
            init_type_t(world, e, EcsPrimitiveType, int16_t);
            break;
        case EcsI32:
            init_type_t(world, e, EcsPrimitiveType, int32_t);
            break;
        case EcsI64:
            init_type_t(world, e, EcsPrimitiveType, int64_t);
            break;
        case EcsF32:
            init_type_t(world, e, EcsPrimitiveType, float);
            break;
        case EcsF64:
            init_type_t(world, e, EcsPrimitiveType, double);
            break;
        case EcsUPtr:
            init_type_t(world, e, EcsPrimitiveType, uintptr_t);
            break;
        case EcsIPtr:
            init_type_t(world, e, EcsPrimitiveType, intptr_t);
            break;
        case EcsString:
            init_type_t(world, e, EcsPrimitiveType, char*);
            break;
        case EcsEntity:
            init_type_t(world, e, EcsPrimitiveType, ecs_entity_t);
            break;
        }
    }
}

static
void flecs_set_member(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMember *member = ecs_field(it, EcsMember, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
        if (!parent) {
            ecs_err("missing parent for member '%s'", ecs_get_name(world, e));
            continue;
        }

        flecs_add_member_to_struct(world, parent, e, &member[i]);
    }
}

static
void flecs_add_enum(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (init_type_t(world, e, EcsEnumType, ecs_i32_t)) {
            continue;
        }

        ecs_add_id(world, e, EcsExclusive);
        ecs_add_id(world, e, EcsOneOf);
        ecs_add_id(world, e, EcsTag);
    }
}

static
void flecs_add_bitmask(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (init_type_t(world, e, EcsBitmaskType, ecs_u32_t)) {
            continue;
        }
    }
}

static
void flecs_add_constant(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
        if (!parent) {
            ecs_err("missing parent for constant '%s'", ecs_get_name(world, e));
            continue;
        }

        if (ecs_has(world, parent, EcsEnum)) {
            flecs_add_constant_to_enum(world, parent, e, it->event_id);
        } else if (ecs_has(world, parent, EcsBitmask)) {
            flecs_add_constant_to_bitmask(world, parent, e, it->event_id);
        }
    }
}

static
void flecs_set_array(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsArray *array = ecs_field(it, EcsArray, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t elem_type = array[i].type;
        int32_t elem_count = array[i].count;

        if (!elem_type) {
            ecs_err("array '%s' has no element type", ecs_get_name(world, e));
            continue;
        }

        if (!elem_count) {
            ecs_err("array '%s' has size 0", ecs_get_name(world, e));
            continue;
        }

        const EcsComponent *elem_ptr = ecs_get(world, elem_type, EcsComponent);
        if (flecs_init_type(world, e, EcsArrayType, 
            elem_ptr->size * elem_count, elem_ptr->alignment)) 
        {
            continue;
        }
    }
}

static
void flecs_set_vector(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsVector *array = ecs_field(it, EcsVector, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t elem_type = array[i].type;

        if (!elem_type) {
            ecs_err("vector '%s' has no element type", ecs_get_name(world, e));
            continue;
        }

        if (init_type_t(world, e, EcsVectorType, ecs_vector_t*)) {
            continue;
        }
    }
}

bool flecs_unit_validate(
    ecs_world_t *world,
    ecs_entity_t t,
    EcsUnit *data)
{
    char *derived_symbol = NULL;
    const char *symbol = data->symbol;

    ecs_entity_t base = data->base;
    ecs_entity_t over = data->over;
    ecs_entity_t prefix = data->prefix;
    ecs_unit_translation_t translation = data->translation;

    if (base) {
        if (!ecs_has(world, base, EcsUnit)) {
            ecs_err("entity '%s' for unit '%s' used as base is not a unit",
                ecs_get_name(world, base), ecs_get_name(world, t));
            goto error;
        }
    }

    if (over) {
        if (!base) {
            ecs_err("invalid unit '%s': cannot specify over without base",
                ecs_get_name(world, t));
            goto error;
        }
        if (!ecs_has(world, over, EcsUnit)) {
            ecs_err("entity '%s' for unit '%s' used as over is not a unit",
                ecs_get_name(world, over), ecs_get_name(world, t));
            goto error;
        }
    }

    if (prefix) {
        if (!base) {
            ecs_err("invalid unit '%s': cannot specify prefix without base",
                ecs_get_name(world, t));
            goto error;
        }
        const EcsUnitPrefix *prefix_ptr = ecs_get(world, prefix, EcsUnitPrefix);
        if (!prefix_ptr) {
            ecs_err("entity '%s' for unit '%s' used as prefix is not a prefix",
                ecs_get_name(world, over), ecs_get_name(world, t));
            goto error;
        }

        if (translation.factor || translation.power) {
            if (prefix_ptr->translation.factor != translation.factor ||
                prefix_ptr->translation.power != translation.power)
            {
                ecs_err(
                    "factor for unit '%s' is inconsistent with prefix '%s'",
                    ecs_get_name(world, t), ecs_get_name(world, prefix));
                goto error;
            }
        } else {
            translation = prefix_ptr->translation;
        }
    }

    if (base) {
        bool must_match = false; /* Must base symbol match symbol? */
        ecs_strbuf_t sbuf = ECS_STRBUF_INIT;
        if (prefix) {
            const EcsUnitPrefix *ptr = ecs_get(world, prefix, EcsUnitPrefix);
            ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            if (ptr->symbol) {
                ecs_strbuf_appendstr(&sbuf, ptr->symbol);
                must_match = true;
            }
        }

        const EcsUnit *uptr = ecs_get(world, base, EcsUnit);
        ecs_assert(uptr != NULL, ECS_INTERNAL_ERROR, NULL);
        if (uptr->symbol) {
            ecs_strbuf_appendstr(&sbuf, uptr->symbol);
        }

        if (over) {
            uptr = ecs_get(world, over, EcsUnit);
            ecs_assert(uptr != NULL, ECS_INTERNAL_ERROR, NULL);
            if (uptr->symbol) {
                ecs_strbuf_appendch(&sbuf, '/');
                ecs_strbuf_appendstr(&sbuf, uptr->symbol);
                must_match = true;
            }
        }

        derived_symbol = ecs_strbuf_get(&sbuf);
        if (derived_symbol && !ecs_os_strlen(derived_symbol)) {
            ecs_os_free(derived_symbol);
            derived_symbol = NULL;
        }

        if (derived_symbol && symbol && ecs_os_strcmp(symbol, derived_symbol)) {
            if (must_match) {
                ecs_err("symbol '%s' for unit '%s' does not match base"
                    " symbol '%s'", symbol, 
                        ecs_get_name(world, t), derived_symbol);
                goto error;
            }
        }
        if (!symbol && derived_symbol && (prefix || over)) {
            ecs_os_free(data->symbol);
            data->symbol = derived_symbol;
        } else {
            ecs_os_free(derived_symbol);
        }
    }

    data->base = base;
    data->over = over;
    data->prefix = prefix;
    data->translation = translation;

    return true;
error:
    ecs_os_free(derived_symbol);
    return false;
}

static
void flecs_set_unit(ecs_iter_t *it) {
    EcsUnit *u = ecs_field(it, EcsUnit, 1);

    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        flecs_unit_validate(world, e, &u[i]);
    }
}

static
void flecs_unit_quantity_monitor(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    if (it->event == EcsOnAdd) {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = it->entities[i];
            ecs_add_pair(world, e, EcsQuantity, e);
        }
    } else {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = it->entities[i];
            ecs_remove_pair(world, e, EcsQuantity, e);
        }
    }
}

static
void ecs_meta_type_init_default_ctor(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMetaType *type = ecs_field(it, EcsMetaType, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        /* If a component is defined from reflection data, configure it with the
         * default constructor. This ensures that a new component value does not
         * contain uninitialized memory, which could cause serializers to crash
         * when for example inspecting string fields. */
        if (!type->existing) {
            ecs_set_hooks_id(world, it->entities[i], 
                &(ecs_type_hooks_t){ 
                    .ctor = ecs_default_ctor
                });
        }
    }
}

static
void flecs_member_on_set(ecs_iter_t *it) {
    EcsMember *mbr = it->ptrs[0];
    if (!mbr->count) {
        mbr->count = 1;
    }
}

void FlecsMetaImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsMeta);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsMetaType);
    flecs_bootstrap_component(world, EcsMetaTypeSerialized);
    flecs_bootstrap_component(world, EcsPrimitive);
    flecs_bootstrap_component(world, EcsEnum);
    flecs_bootstrap_component(world, EcsBitmask);
    flecs_bootstrap_component(world, EcsMember);
    flecs_bootstrap_component(world, EcsStruct);
    flecs_bootstrap_component(world, EcsArray);
    flecs_bootstrap_component(world, EcsVector);
    flecs_bootstrap_component(world, EcsUnit);
    flecs_bootstrap_component(world, EcsUnitPrefix);

    flecs_bootstrap_tag(world, EcsConstant);
    flecs_bootstrap_tag(world, EcsQuantity);

    ecs_set_hooks(world, EcsMetaType, { .ctor = ecs_default_ctor });

    ecs_set_hooks(world, EcsMetaTypeSerialized, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsMetaTypeSerialized),
        .copy = ecs_copy(EcsMetaTypeSerialized),
        .dtor = ecs_dtor(EcsMetaTypeSerialized)
    });

    ecs_set_hooks(world, EcsStruct, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsStruct),
        .copy = ecs_copy(EcsStruct),
        .dtor = ecs_dtor(EcsStruct)
    });

    ecs_set_hooks(world, EcsMember, { 
        .ctor = ecs_default_ctor,
        .on_set = flecs_member_on_set
    });

    ecs_set_hooks(world, EcsEnum, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsEnum),
        .copy = ecs_copy(EcsEnum),
        .dtor = ecs_dtor(EcsEnum)
    });

    ecs_set_hooks(world, EcsBitmask, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsBitmask),
        .copy = ecs_copy(EcsBitmask),
        .dtor = ecs_dtor(EcsBitmask)
    });

    ecs_set_hooks(world, EcsUnit, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsUnit),
        .copy = ecs_copy(EcsUnit),
        .dtor = ecs_dtor(EcsUnit)
    });

    ecs_set_hooks(world, EcsUnitPrefix, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsUnitPrefix),
        .copy = ecs_copy(EcsUnitPrefix),
        .dtor = ecs_dtor(EcsUnitPrefix)
    });

    /* Register triggers to finalize type information from component data */
    ecs_entity_t old_scope = ecs_set_scope( /* Keep meta scope clean */
        world, EcsFlecsInternals);
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsPrimitive), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = flecs_set_primitive
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsMember), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = flecs_set_member
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsEnum), .src.flags = EcsSelf },
        .events = {EcsOnAdd},
        .callback = flecs_add_enum
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsBitmask), .src.flags = EcsSelf },
        .events = {EcsOnAdd},
        .callback = flecs_add_bitmask
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = EcsConstant, .src.flags = EcsSelf },
        .events = {EcsOnAdd},
        .callback = flecs_add_constant
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_pair(EcsConstant, EcsWildcard), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = flecs_add_constant
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsArray), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = flecs_set_array
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsVector), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = flecs_set_vector
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsUnit), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = flecs_set_unit
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsMetaType), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = ecs_meta_type_serialized_init
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_id(EcsMetaType), .src.flags = EcsSelf },
        .events = {EcsOnSet},
        .callback = ecs_meta_type_init_default_ctor
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { .id = ecs_id(EcsUnit) },
            { .id = EcsQuantity }
        },
        .events = { EcsMonitor },
        .callback = flecs_unit_quantity_monitor
    });
    ecs_set_scope(world, old_scope);

    /* Initialize primitive types */
    #define ECS_PRIMITIVE(world, type, primitive_kind)\
        ecs_entity_init(world, &(ecs_entity_desc_t){\
            .id = ecs_id(ecs_##type##_t),\
            .name = #type,\
            .symbol = #type });\
        ecs_set(world, ecs_id(ecs_##type##_t), EcsPrimitive, {\
            .kind = primitive_kind\
        });

    ECS_PRIMITIVE(world, bool, EcsBool);
    ECS_PRIMITIVE(world, char, EcsChar);
    ECS_PRIMITIVE(world, byte, EcsByte);
    ECS_PRIMITIVE(world, u8, EcsU8);
    ECS_PRIMITIVE(world, u16, EcsU16);
    ECS_PRIMITIVE(world, u32, EcsU32);
    ECS_PRIMITIVE(world, u64, EcsU64);
    ECS_PRIMITIVE(world, uptr, EcsUPtr);
    ECS_PRIMITIVE(world, i8, EcsI8);
    ECS_PRIMITIVE(world, i16, EcsI16);
    ECS_PRIMITIVE(world, i32, EcsI32);
    ECS_PRIMITIVE(world, i64, EcsI64);
    ECS_PRIMITIVE(world, iptr, EcsIPtr);
    ECS_PRIMITIVE(world, f32, EcsF32);
    ECS_PRIMITIVE(world, f64, EcsF64);
    ECS_PRIMITIVE(world, string, EcsString);
    ECS_PRIMITIVE(world, entity, EcsEntity);

    #undef ECS_PRIMITIVE

    ecs_set_hooks(world, ecs_string_t, {
        .ctor = ecs_default_ctor,
        .copy = ecs_copy(ecs_string_t),
        .move = ecs_move(ecs_string_t),
        .dtor = ecs_dtor(ecs_string_t)
    });

    /* Set default child components */
    ecs_add_pair(world, ecs_id(EcsStruct), 
        EcsDefaultChildComponent, ecs_id(EcsMember));

    ecs_add_pair(world, ecs_id(EcsMember), 
        EcsDefaultChildComponent, ecs_id(EcsMember));

    ecs_add_pair(world, ecs_id(EcsEnum), 
        EcsDefaultChildComponent, EcsConstant);

    ecs_add_pair(world, ecs_id(EcsBitmask), 
        EcsDefaultChildComponent, EcsConstant);

    /* Relationship properties */
    ecs_add_id(world, EcsQuantity, EcsExclusive);
    ecs_add_id(world, EcsQuantity, EcsTag);

    /* Initialize reflection data for meta components */
    ecs_entity_t type_kind = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, { .name = "TypeKind" }),
        .constants = {
            {.name = "PrimitiveType"},
            {.name = "BitmaskType"},
            {.name = "EnumType"},
            {.name = "StructType"},
            {.name = "ArrayType"},
            {.name = "VectorType"}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsMetaType),
        .members = {
            {.name = (char*)"kind", .type = type_kind}
        }
    });

    ecs_entity_t primitive_kind = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, { .name = "PrimitiveKind" }),
        .constants = {
            {.name = "Bool", 1}, 
            {.name = "Char"}, 
            {.name = "Byte"}, 
            {.name = "U8"}, 
            {.name = "U16"}, 
            {.name = "U32"}, 
            {.name = "U64"},
            {.name = "I8"}, 
            {.name = "I16"}, 
            {.name = "I32"}, 
            {.name = "I64"}, 
            {.name = "F32"}, 
            {.name = "F64"}, 
            {.name = "UPtr"},
            {.name = "IPtr"}, 
            {.name = "String"}, 
            {.name = "Entity"}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsPrimitive),
        .members = {
            {.name = (char*)"kind", .type = primitive_kind}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsMember),
        .members = {
            {.name = (char*)"type", .type = ecs_id(ecs_entity_t)},
            {.name = (char*)"count", .type = ecs_id(ecs_i32_t)},
            {.name = (char*)"unit", .type = ecs_id(ecs_entity_t)},
            {.name = (char*)"offset", .type = ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsArray),
        .members = {
            {.name = (char*)"type", .type = ecs_id(ecs_entity_t)},
            {.name = (char*)"count", .type = ecs_id(ecs_i32_t)},
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsVector),
        .members = {
            {.name = (char*)"type", .type = ecs_id(ecs_entity_t)}
        }
    });

    ecs_entity_t ut = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, { .name = "unit_translation" }),
        .members = {
            {.name = (char*)"factor", .type = ecs_id(ecs_i32_t)},
            {.name = (char*)"power", .type = ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsUnit),
        .members = {
            {.name = (char*)"symbol", .type = ecs_id(ecs_string_t)},
            {.name = (char*)"prefix", .type = ecs_id(ecs_entity_t)},
            {.name = (char*)"base", .type = ecs_id(ecs_entity_t)},
            {.name = (char*)"over", .type = ecs_id(ecs_entity_t)},
            {.name = (char*)"translation", .type = ut}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsUnitPrefix),
        .members = {
            {.name = (char*)"symbol", .type = ecs_id(ecs_string_t)},
            {.name = (char*)"translation", .type = ut}
        }
    });
}

#endif
