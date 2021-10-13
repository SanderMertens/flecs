
#include "../private_api.h"

#ifdef FLECS_COREDOC

#define URL_ROOT "https://flecs.docsforge.com/master/relations-manual/"

void FlecsCoreDocImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsCoreDoc);

    ECS_IMPORT(world, FlecsMeta);
    ECS_IMPORT(world, FlecsDoc);

    ecs_set_name_prefix(world, "Ecs");

    /* Initialize reflection data for core components */

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(EcsComponent),
        .members = {
            {.name = (char*)"size", .type = ecs_id(ecs_i32_t)},
            {.name = (char*)"alignment", .type = ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(EcsDocDescription),
        .members = {
            {.name = "value", .type = ecs_id(ecs_string_t)}
        }
    });

    /* Initialize documentation data for core components */
    ecs_doc_set_brief(world, EcsFlecs, "Flecs root module");
    ecs_doc_set_brief(world, EcsFlecsCore, "Flecs module with builtin components");

    ecs_doc_set_brief(world, EcsWorld, "Entity associated with world");

    ecs_doc_set_brief(world, ecs_id(EcsComponent), "Component that is added to all components");
    ecs_doc_set_brief(world, EcsModule, "Tag that is added to modules");
    ecs_doc_set_brief(world, EcsPrefab, "Tag that is added to prefabs");
    ecs_doc_set_brief(world, EcsDisabled, "Tag that is added to disabled entities");

    ecs_doc_set_brief(world, ecs_id(EcsIdentifier), "Component used for entity names");
    ecs_doc_set_brief(world, EcsName, "Tag used with EcsIdentifier to signal entity name");
    ecs_doc_set_brief(world, EcsSymbol, "Tag used with EcsIdentifier to signal entity symbol");

    ecs_doc_set_brief(world, EcsTransitive, "Transitive relation property");
    ecs_doc_set_brief(world, EcsInclusive, "Inclusive relation property");
    ecs_doc_set_brief(world, EcsFinal, "Final relation property");
    ecs_doc_set_brief(world, EcsTag, "Tag relation property");
    ecs_doc_set_brief(world, EcsOnDelete, "OnDelete relation cleanup property");
    ecs_doc_set_brief(world, EcsOnDeleteObject, "OnDeleteObject relation cleanup property");
    ecs_doc_set_brief(world, EcsRemove, "Remove relation cleanup property");
    ecs_doc_set_brief(world, EcsDelete, "Delete relation cleanup property");
    ecs_doc_set_brief(world, EcsThrow, "Throw relation cleanup property");
    ecs_doc_set_brief(world, EcsIsA, "Builtin IsA relation");
    ecs_doc_set_brief(world, EcsChildOf, "Builtin ChildOf relation");
    ecs_doc_set_brief(world, EcsOnAdd, "Builtin OnAdd event");
    ecs_doc_set_brief(world, EcsOnRemove, "Builtin OnRemove event");
    ecs_doc_set_brief(world, EcsOnSet, "Builtin OnSet event");
    ecs_doc_set_brief(world, EcsUnSet, "Builtin UnSet event");

    ecs_doc_set_link(world, EcsTransitive, URL_ROOT "#transitive-relations");
    ecs_doc_set_link(world, EcsInclusive, URL_ROOT "#inclusive-relations");
    ecs_doc_set_link(world, EcsFinal, URL_ROOT "#final-entities");
    ecs_doc_set_link(world, EcsTag, URL_ROOT "#tag-relations");
    ecs_doc_set_link(world, EcsOnDelete, URL_ROOT "#relation-cleanup-properties");
    ecs_doc_set_link(world, EcsOnDeleteObject, URL_ROOT "#relation-cleanup-properties");
    ecs_doc_set_link(world, EcsRemove, URL_ROOT "#relation-cleanup-properties");
    ecs_doc_set_link(world, EcsDelete, URL_ROOT "#relation-cleanup-properties");
    ecs_doc_set_link(world, EcsThrow, URL_ROOT "#relation-cleanup-properties");
    ecs_doc_set_link(world, EcsIsA, URL_ROOT "#the-isa-relation");
    ecs_doc_set_link(world, EcsChildOf, URL_ROOT "#the-childof-relation"); 
    
    /* Initialize documentation for meta components */
    ecs_entity_t meta = ecs_lookup_fullpath(world, "flecs.meta");
    ecs_doc_set_brief(world, meta, "Flecs module with reflection components");

    ecs_doc_set_brief(world, ecs_id(EcsMetaType), "Component added to types");
    ecs_doc_set_brief(world, ecs_id(EcsMetaTypeSerialized), "Component that stores reflection data in an optimized format");
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
}

#endif
