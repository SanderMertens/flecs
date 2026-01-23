/**
 * @file addons/meta/meta.c
 * @brief Meta addon.
 */

#include "meta.h"
#include "type_support/type_support.h"

#ifdef FLECS_META

void flecs_type_serializer_dtor(
    EcsTypeSerializer *ptr) 
{
    int32_t i, count = ecs_vec_count(&ptr->ops);
    ecs_meta_op_t *ops = ecs_vec_first(&ptr->ops);
    
    for (i = 0; i < count; i ++) {
        ecs_meta_op_t *op = &ops[i];
        if (op->kind == EcsOpPushStruct) {
            if (op->is.members) {
                flecs_name_index_free(op->is.members);
            }
        }
    }

    ecs_vec_fini_t(NULL, &ptr->ops, ecs_meta_op_t);
}

static ECS_COPY(EcsTypeSerializer, dst, src, {
    flecs_type_serializer_dtor(dst);

    dst->ops = ecs_vec_copy_t(NULL, &src->ops, ecs_meta_op_t);

    int32_t o, count = ecs_vec_count(&dst->ops);
    ecs_meta_op_t *ops = ecs_vec_first_t(&dst->ops, ecs_meta_op_t);
    
    for (o = 0; o < count; o ++) {
        ecs_meta_op_t *op = &ops[o];
        if (op->kind == EcsOpPushStruct) {
            if (op->is.members) {
                op->is.members = flecs_name_index_copy(op->is.members);
            }
        }
    }
})

static ECS_MOVE(EcsTypeSerializer, dst, src, {
    flecs_type_serializer_dtor(dst);
    dst->ops = src->ops;
    src->ops = (ecs_vec_t){0};
})

static ECS_DTOR(EcsTypeSerializer, ptr, { 
    flecs_type_serializer_dtor(ptr);
})

static
const char* flecs_type_kind_str(
    ecs_type_kind_t kind)
{
    switch(kind) {
    case EcsPrimitiveType: return "Primitive";
    case EcsBitmaskType: return "Bitmask";
    case EcsEnumType: return "Enum";
    case EcsStructType: return "Struct";
    case EcsArrayType: return "Array";
    case EcsVectorType: return "Vector";
    case EcsOpaqueType: return "Opaque";
    default: return "unknown";
    }
}

int flecs_init_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_type_kind_t kind,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);

    EcsType *meta_type = ecs_ensure(world, type, EcsType);
    if (meta_type->kind == 0) {
        meta_type->kind = kind;

        /* Determine if this is an existing type or a reflection-defined type 
         * (runtime type) */
        meta_type->existing = ecs_has(world, type, EcsComponent);

        /* For existing types, ensure that component has a default constructor, 
         * to prevent crashing serializers on uninitialized values. For runtime 
         * types (rtt), the default hooks are set by 
         * flecs_meta_rtt_init_default_hooks */
        ecs_type_info_t *ti = flecs_type_info_ensure(world, type);
        if (meta_type->existing) {
            if(!ti->hooks.ctor) {
                ti->hooks.ctor = flecs_default_ctor;
            }
            if(kind == EcsEnumType) {
                /* Generate compare/equals hooks for enums, copying
                   the underlying type's hooks, which should be 
                   any of the default primitive integral compare hooks,
                   i.e. ecs_compare_i8, _i16 _32... */
                const EcsEnum* enum_info = ecs_get(world, type, EcsEnum);
                ecs_assert(enum_info != NULL, ECS_INTERNAL_ERROR, NULL);
                const ecs_type_hooks_t *enum_hooks = ecs_get_hooks_id(
                    world, enum_info->underlying_type);
                ecs_assert(!(enum_hooks->flags & 
                    (ECS_TYPE_HOOK_CMP_ILLEGAL|ECS_TYPE_HOOK_EQUALS_ILLEGAL)), 
                        ECS_INTERNAL_ERROR, NULL);
                ti->hooks.cmp = enum_hooks->cmp;
                ti->hooks.equals = enum_hooks->equals;
                ti->hooks.flags &= 
                    ~(ECS_TYPE_HOOK_CMP_ILLEGAL|ECS_TYPE_HOOK_EQUALS_ILLEGAL);
                ti->hooks.flags |= ECS_TYPE_HOOK_CMP|ECS_TYPE_HOOK_EQUALS; 
            }
        } 
    } else {
        if (meta_type->kind != kind) {
            ecs_err("type '%s' reregistered as '%s' (was '%s')", 
                ecs_get_name(world, type), 
                flecs_type_kind_str(kind),
                flecs_type_kind_str(meta_type->kind));
            return -1;
        }
    }

    if (!meta_type->existing) {
        EcsComponent *comp = ecs_ensure(world, type, EcsComponent);
        comp->size = size;
        comp->alignment = alignment;
        ecs_modified(world, type, EcsComponent);
    } else {
        const EcsComponent *comp = ecs_get(world, type, EcsComponent);
        if (comp->size < size) {
            ecs_err(
                "computed size (%d) for '%s' is larger than actual type (%d)", 
                size, ecs_get_name(world, type), comp->size);
            return -1;
        }
        if (comp->alignment < alignment) {
            ecs_err(
                "computed alignment (%d) for '%s' is larger than actual type (%d)", 
                alignment, ecs_get_name(world, type), comp->alignment);
            return -1;
        }
        if (comp->size == size && comp->alignment != alignment) {
            if (comp->alignment < alignment) {
                ecs_err("computed size for '%s' matches with actual type but "
                    "alignment is different (%d vs. %d)", 
                        ecs_get_name(world, type), alignment, comp->alignment);
            }
        }
        
        meta_type->partial = comp->size != size;
    }

    ecs_modified(world, type, EcsType);

    return 0;
}

void FlecsMetaImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsMeta);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsTypeSerializer);

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsType),
            .name = "type", .symbol = "EcsType",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsType),
        .type.alignment = ECS_ALIGNOF(EcsType),
    });

    ecs_set_hooks(world, EcsType, { .ctor = flecs_default_ctor });

    ecs_set_hooks(world, EcsTypeSerializer, { 
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsTypeSerializer),
        .copy = ecs_copy(EcsTypeSerializer),
        .dtor = ecs_dtor(EcsTypeSerializer)
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsType) },
        .events = {EcsOnSet},
        .callback = flecs_meta_type_serializer_init
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsType) },
        .events = {EcsOnSet},
        .callback = flecs_rtt_init_default_hooks
    });

    /* Import type support for different type kinds */
    flecs_meta_primitives_init(world);
    flecs_meta_enum_init(world);
    flecs_meta_struct_init(world);
    flecs_meta_array_init(world);
    flecs_meta_opaque_init(world);
    flecs_meta_units_init(world);

    /* Import reflection definitions for builtin types */
    flecs_meta_import_definitions(world);
}

#endif
