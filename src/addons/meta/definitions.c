
/**
 * @file addons/meta/definitions.c
 * @brief Reflection definitions for builtin types.
 */

#include "meta.h"

#ifdef FLECS_META

/* Opaque type serializatior addon vector */
static
int flecs_addon_vec_serialize(const ecs_serializer_t *ser, const void *ptr) {
    char ***data = ECS_CONST_CAST(char***, ptr);
    char **addons = data[0];
    do {
        ser->value(ser, ecs_id(ecs_string_t), addons);
    } while((++ addons)[0]);
    return 0;
}

static
size_t flecs_addon_vec_count(const void *ptr) {
    int32_t count = 0;
    char ***data = ECS_CONST_CAST(char***, ptr);
    char **addons = data[0];
    do {
        ++ count;
    } while(addons[count]);
    return flecs_ito(size_t, count);
}

/* Initialize reflection data for core components */
static
void flecs_meta_import_core_definitions(
    ecs_world_t *world)
{
    ecs_struct(world, {
        .entity = ecs_id(EcsComponent),
        .members = {
            { .name = "size", .type = ecs_id(ecs_i32_t) },
            { .name = "alignment", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsDefaultChildComponent),
        .members = {
            { .name = "component", .type = ecs_id(ecs_entity_t) }
        }
    });

    ecs_entity_t string_vec = ecs_vector(world, {
        .entity = ecs_entity(world, { 
            .name = "flecs.core.string_vec_t",
            .root_sep = ""
        }),
        .type = ecs_id(ecs_string_t)
    });

    ecs_entity_t addon_vec = ecs_opaque(world, {
        .entity = ecs_component(world, { 
            .type = {
                .name = "flecs.core.addon_vec_t",
                .size = ECS_SIZEOF(char**),
                .alignment = ECS_ALIGNOF(char**)
            }
        }),
        .type = {
            .as_type = string_vec,
            .serialize = flecs_addon_vec_serialize,
            .count = flecs_addon_vec_count,
        }
    });

    ecs_struct(world, {
        .entity = ecs_entity(world, { 
            .name = "flecs.core.build_info_t",
            .root_sep = ""
        }),
        .members = {
            { .name = "compiler", .type = ecs_id(ecs_string_t) },
            { .name = "addons", .type = addon_vec },
            { .name = "version", .type = ecs_id(ecs_string_t) },
            { .name = "version_major", .type = ecs_id(ecs_i16_t) },
            { .name = "version_minor", .type = ecs_id(ecs_i16_t) },
            { .name = "version_patch", .type = ecs_id(ecs_i16_t) },
            { .name = "debug", .type = ecs_id(ecs_bool_t) },
            { .name = "sanitize", .type = ecs_id(ecs_bool_t) },
            { .name = "perf_trace", .type = ecs_id(ecs_bool_t) }
        }
    });
}

/* Initialize reflection data for doc components */
static
void flecs_meta_import_doc_definitions(
    ecs_world_t *world)
{
    (void)world;
#ifdef FLECS_DOC
    ecs_struct(world, {
        .entity = ecs_id(EcsDocDescription),
        .members = {
            { .name = "value", .type = ecs_id(ecs_string_t) }
        }
    });
#endif
}

/* Initialize reflection data for meta components */
static
void flecs_meta_import_meta_definitions(
    ecs_world_t *world)
{
    ecs_entity_t type_kind = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, { .name = "TypeKind" }),
        .constants = {
            { .name = "PrimitiveType" },
            { .name = "BitmaskType" },
            { .name = "EnumType" },
            { .name = "StructType" },
            { .name = "ArrayType" },
            { .name = "VectorType" },
            { .name = "OpaqueType" }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsType),
        .members = {
            { .name = "kind", .type = type_kind }
        }
    });

    ecs_entity_t primitive_kind = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, { .name = "PrimitiveKind" }),
        .constants = {
            { .name = "Bool", 1 }, 
            { .name = "Char" }, 
            { .name = "Byte" }, 
            { .name = "U8" }, 
            { .name = "U16" }, 
            { .name = "U32" }, 
            { .name = "U64 "},
            { .name = "I8" }, 
            { .name = "I16" }, 
            { .name = "I32" }, 
            { .name = "I64" }, 
            { .name = "F32" }, 
            { .name = "F64" }, 
            { .name = "UPtr "},
            { .name = "IPtr" }, 
            { .name = "String" }, 
            { .name = "Entity" },
            { .name = "Id" }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsPrimitive),
        .members = {
            { .name = "kind", .type = primitive_kind }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsMember),
        .members = {
            { .name = "type", .type = ecs_id(ecs_entity_t) },
            { .name = "count", .type = ecs_id(ecs_i32_t) },
            { .name = "unit", .type = ecs_id(ecs_entity_t) },
            { .name = "offset", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t vr = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, { .name = "value_range" }),
        .members = {
            { .name = "min", .type = ecs_id(ecs_f64_t) },
            { .name = "max", .type = ecs_id(ecs_f64_t) }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsMemberRanges),
        .members = {
            { .name = "value", .type = vr },
            { .name = "warning", .type = vr },
            { .name = "error", .type = vr }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsArray),
        .members = {
            { .name = "type", .type = ecs_id(ecs_entity_t) },
            { .name = "count", .type = ecs_id(ecs_i32_t) },
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsVector),
        .members = {
            { .name = "type", .type = ecs_id(ecs_entity_t) }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsOpaque),
        .members = {
            { .name = "as_type", .type = ecs_id(ecs_entity_t) }
        }
    });

    ecs_entity_t ut = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, { .name = "unit_translation" }),
        .members = {
            { .name = "factor", .type = ecs_id(ecs_i32_t) },
            { .name = "power", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsUnit),
        .members = {
            { .name = "symbol", .type = ecs_id(ecs_string_t) },
            { .name = "prefix", .type = ecs_id(ecs_entity_t) },
            { .name = "base", .type = ecs_id(ecs_entity_t) },
            { .name = "over", .type = ecs_id(ecs_entity_t) },
            { .name = "translation", .type = ut }
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsUnitPrefix),
        .members = {
            { .name = "symbol", .type = ecs_id(ecs_string_t) },
            { .name = "translation", .type = ut }
        }
    });

    /* Meta doc definitions */
#ifdef FLECS_DOC
    ecs_entity_t meta = ecs_lookup(world, "flecs.meta");
    ecs_doc_set_brief(world, meta, "Flecs module with reflection components");

    ecs_doc_set_brief(world, ecs_id(EcsType), "Component added to types");
    ecs_doc_set_brief(world, ecs_id(EcsTypeSerializer), "Component that stores reflection data in an optimized format");
    ecs_doc_set_brief(world, ecs_id(EcsPrimitive), "Component added to primitive types");
    ecs_doc_set_brief(world, ecs_id(EcsEnum), "Component added to enumeration types");
    ecs_doc_set_brief(world, ecs_id(EcsBitmask), "Component added to bitmask types");
    ecs_doc_set_brief(world, ecs_id(EcsMember), "Component added to struct members");
    ecs_doc_set_brief(world, ecs_id(EcsStruct), "Component added to struct types");
    ecs_doc_set_brief(world, ecs_id(EcsArray), "Component added to array types");
    ecs_doc_set_brief(world, ecs_id(EcsVector), "Component added to vector types");

    ecs_doc_set_brief(world, ecs_id(ecs_bool_t), "bool component");
    ecs_doc_set_brief(world, ecs_id(ecs_char_t), "char component");
    ecs_doc_set_brief(world, ecs_id(ecs_byte_t), "byte component");
    ecs_doc_set_brief(world, ecs_id(ecs_u8_t), "8 bit unsigned int component");
    ecs_doc_set_brief(world, ecs_id(ecs_u16_t), "16 bit unsigned int component");
    ecs_doc_set_brief(world, ecs_id(ecs_u32_t), "32 bit unsigned int component");
    ecs_doc_set_brief(world, ecs_id(ecs_u64_t), "64 bit unsigned int component");
    ecs_doc_set_brief(world, ecs_id(ecs_uptr_t), "word sized unsigned int component");
    ecs_doc_set_brief(world, ecs_id(ecs_i8_t), "8 bit signed int component");
    ecs_doc_set_brief(world, ecs_id(ecs_i16_t), "16 bit signed int component");
    ecs_doc_set_brief(world, ecs_id(ecs_i32_t), "32 bit signed int component");
    ecs_doc_set_brief(world, ecs_id(ecs_i64_t), "64 bit signed int component");
    ecs_doc_set_brief(world, ecs_id(ecs_iptr_t), "word sized signed int component");
    ecs_doc_set_brief(world, ecs_id(ecs_f32_t), "32 bit floating point component");
    ecs_doc_set_brief(world, ecs_id(ecs_f64_t), "64 bit floating point component");
    ecs_doc_set_brief(world, ecs_id(ecs_string_t), "string component");
    ecs_doc_set_brief(world, ecs_id(ecs_entity_t), "entity component");
#endif
}

void flecs_meta_import_definitions(
    ecs_world_t *world)
{
    flecs_meta_import_core_definitions(world);
    flecs_meta_import_doc_definitions(world);
    flecs_meta_import_meta_definitions(world);
}

#endif
