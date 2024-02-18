/**
 * @file addons/coredoc.c
 * @brief Core doc addon.
 */

#include "../private_api.h"

#ifdef FLECS_COREDOC

#define URL_ROOT "https://www.flecs.dev/flecs/md_docs_2Relationships.html"

void FlecsCoreDocImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsCoreDoc);

    ECS_IMPORT(world, FlecsMeta);
    ECS_IMPORT(world, FlecsDoc);

    ecs_set_name_prefix(world, "Ecs");

    /* Initialize reflection data for core components */

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsComponent),
        .members = {
            {.name = "size", .type = ecs_id(ecs_i32_t)},
            {.name = "alignment", .type = ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(EcsDocDescription),
        .members = {
            {.name = "value", .type = ecs_id(ecs_string_t)}
        }
    });

    /* Initialize documentation data for core components */
    ecs_doc_set_brief(world, EcsFlecs, "Flecs root module");
    ecs_doc_set_link(world, EcsFlecs, "https://github.com/SanderMertens/flecs");
    ecs_doc_set_brief(world, EcsFlecsCore, "Module with builtin components");
    ecs_doc_set_brief(world, EcsFlecsInternals, "Module with internal entities");

    ecs_doc_set_brief(world, EcsWorld, "Entity associated with world");

    ecs_doc_set_brief(world, ecs_id(EcsComponent), "Component that is added to components");
    ecs_doc_set_brief(world, EcsModule, "Tag that is added to modules");
    ecs_doc_set_brief(world, EcsPrefab, "Tag that is added to prefabs");
    ecs_doc_set_brief(world, EcsDisabled, "Tag that is added to disabled entities");
    ecs_doc_set_brief(world, EcsPrivate, "Tag that is added to private components");
    ecs_doc_set_brief(world, EcsFlag, "Internal tag for tracking ids with special id flags");
    ecs_doc_set_brief(world, ecs_id(EcsIterable), "Internal component to make (query) entities iterable");
    ecs_doc_set_brief(world, ecs_id(EcsPoly), "Internal component that stores pointer to poly objects");
    
    ecs_doc_set_brief(world, ecs_id(EcsTarget), "Internal component that stores information for flattened trees");
    ecs_doc_set_brief(world, EcsFlatten, "Tag that when added to assembly automatically flattens tree");

    ecs_doc_set_brief(world, ecs_id(EcsIdentifier), "Component used for entity names");
    ecs_doc_set_brief(world, EcsName, "Tag used with EcsIdentifier to store entity name");
    ecs_doc_set_brief(world, EcsSymbol, "Tag used with EcsIdentifier to store entity symbol");
    ecs_doc_set_brief(world, EcsAlias, "Tag used with EcsIdentifier to store entity alias");
    
    ecs_doc_set_brief(world, EcsQuery, "Tag added to query entities");
    ecs_doc_set_brief(world, EcsObserver, "Tag added to observer entities");

    ecs_doc_set_brief(world, EcsTransitive, "Trait that enables transitive evaluation of relationships");
    ecs_doc_set_brief(world, EcsReflexive, "Trait that enables reflexive evaluation of relationships");
    ecs_doc_set_brief(world, EcsFinal, "Trait that indicates an entity cannot be inherited from");
    ecs_doc_set_brief(world, EcsDontInherit, "Trait that indicates it should not be inherited");
    ecs_doc_set_brief(world, EcsTag, "Trait that ensures a pair cannot contain a value");
    ecs_doc_set_brief(world, EcsAcyclic, "Trait that indicates a relationship is acyclic");
    ecs_doc_set_brief(world, EcsTraversable, "Trait that indicates a relationship is traversable");
    ecs_doc_set_brief(world, EcsExclusive, "Trait that ensures a relationship can only have one target");
    ecs_doc_set_brief(world, EcsSymmetric, "Trait that causes a relationship to be two-way");
    ecs_doc_set_brief(world, EcsWith, "Trait for adding additional components when a component is added");
    ecs_doc_set_brief(world, EcsAlwaysOverride, "Trait that indicates a component should always be overridden");
    ecs_doc_set_brief(world, EcsUnion, "Trait for creating a non-fragmenting relationship");
    ecs_doc_set_brief(world, EcsOneOf, "Trait that enforces target of relationship is a child of <specified>");
    ecs_doc_set_brief(world, EcsOnDelete, "Cleanup trait for specifying what happens when component is deleted");
    ecs_doc_set_brief(world, EcsOnDeleteTarget, "Cleanup trait for specifying what happens when pair target is deleted");
    ecs_doc_set_brief(world, EcsRemove, "Cleanup action used with OnDelete/OnDeleteTarget");
    ecs_doc_set_brief(world, EcsDelete, "Cleanup action used with OnDelete/OnDeleteTarget");
    ecs_doc_set_brief(world, EcsPanic, "Cleanup action used with OnDelete/OnDeleteTarget");
    ecs_doc_set_brief(world, EcsDefaultChildComponent, "Sets default component hint for children of entity");
    ecs_doc_set_brief(world, EcsIsA, "Relationship used for expressing inheritance");
    ecs_doc_set_brief(world, EcsChildOf, "Relationship used for expressing hierarchies");
    ecs_doc_set_brief(world, EcsDependsOn, "Relationship used for expressing dependencies");
    ecs_doc_set_brief(world, EcsSlotOf, "Relationship used for expressing prefab slots");
    ecs_doc_set_brief(world, EcsOnAdd, "Event emitted when component is added");
    ecs_doc_set_brief(world, EcsOnRemove, "Event emitted when component is removed");
    ecs_doc_set_brief(world, EcsOnSet, "Event emitted when component is set");
    ecs_doc_set_brief(world, EcsUnSet, "Event emitted when component is unset");
    ecs_doc_set_brief(world, EcsMonitor, "Marker used to create monitor observers");
    ecs_doc_set_brief(world, EcsOnTableFill, "Event emitted when table becomes non-empty");
    ecs_doc_set_brief(world, EcsOnTableEmpty, "Event emitted when table becomes empty");
    ecs_doc_set_brief(world, EcsOnTableCreate, "Event emitted when table is created");
    ecs_doc_set_brief(world, EcsOnTableDelete, "Event emitted when table is deleted");

    ecs_doc_set_brief(world, EcsThis, "Query marker to express $this variable");
    ecs_doc_set_brief(world, EcsWildcard, "Query marker to express match all wildcard");
    ecs_doc_set_brief(world, EcsAny, "Query marker to express match at least one wildcard");

    ecs_doc_set_brief(world, EcsPredEq, "Query marker to express == operator");
    ecs_doc_set_brief(world, EcsPredMatch, "Query marker to express ~= operator");
    ecs_doc_set_brief(world, EcsPredLookup, "Query marker to express by-name lookup");
    ecs_doc_set_brief(world, EcsScopeOpen, "Query marker to express scope open");
    ecs_doc_set_brief(world, EcsScopeClose, "Query marker to express scope close");
    ecs_doc_set_brief(world, EcsEmpty, "Tag used to indicate a query has no results");
    
    /* Initialize documentation for meta components */
    ecs_entity_t meta = ecs_lookup(world, "flecs.meta");
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

    /* Initialize documentation for doc components */
    ecs_entity_t doc = ecs_lookup(world, "flecs.doc");
    ecs_doc_set_brief(world, doc, "Flecs module with documentation components");

    ecs_doc_set_brief(world, ecs_id(EcsDocDescription), "Component used to add documentation");
    ecs_doc_set_brief(world, EcsDocBrief, "Used as (Description, Brief) to add a brief description");
    ecs_doc_set_brief(world, EcsDocDetail, "Used as (Description, Detail) to add a detailed description");
    ecs_doc_set_brief(world, EcsDocLink, "Used as (Description, Link) to add a link");
}

#endif
