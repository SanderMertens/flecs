/**
 * @file addons/doc.c
 * @brief Doc addon.
 */

#include "../private_api.h"

#ifdef FLECS_DOC

static ECS_COPY(EcsDocDescription, dst, src, {
    ecs_os_strset((char**)&dst->value, src->value);

})

static ECS_MOVE(EcsDocDescription, dst, src, {
    ecs_os_free((char*)dst->value);
    dst->value = src->value;
    src->value = NULL;
})

static ECS_DTOR(EcsDocDescription, ptr, { 
    ecs_os_free((char*)ptr->value);
})

static
void flecs_doc_set(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t kind,
    const char *value)
{
    if (value) {
        ecs_set_pair(world, entity, EcsDocDescription, kind, {
            /* Safe, value gets copied by copy hook */
            .value = ECS_CONST_CAST(char*, value)
        });
    } else {
        ecs_remove_pair(world, entity, ecs_id(EcsDocDescription), kind);
    }
}

void ecs_doc_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    flecs_doc_set(world, entity, EcsName, name);
}

void ecs_doc_set_brief(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *brief)
{
    flecs_doc_set(world, entity, EcsDocBrief, brief);
}

void ecs_doc_set_detail(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *detail)
{
    flecs_doc_set(world, entity, EcsDocDetail, detail);
}

void ecs_doc_set_link(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *link)
{
    flecs_doc_set(world, entity, EcsDocLink, link);
}

void ecs_doc_set_color(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *color)
{
    flecs_doc_set(world, entity, EcsDocColor, color);
}

const char* ecs_doc_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsName);
    if (ptr) {
        return ptr->value;
    } else {
        return ecs_get_name(world, entity);
    }
}

const char* ecs_doc_get_brief(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocBrief);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_doc_get_detail(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocDetail);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_doc_get_link(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocLink);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_doc_get_color(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    const EcsDocDescription *ptr = ecs_get_pair(
        world, entity, EcsDocDescription, EcsDocColor);
    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

/* Doc definitions for core components */
static
void flecs_doc_import_core_definitions(
    ecs_world_t *world)
{
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
    ecs_doc_set_brief(world, ecs_id(EcsPoly), "Internal component that stores pointer to poly objects");

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
    ecs_doc_set_brief(world, EcsPairIsTag, "Trait that ensures a pair cannot contain a value");
    ecs_doc_set_brief(world, EcsAcyclic, "Trait that indicates a relationship is acyclic");
    ecs_doc_set_brief(world, EcsTraversable, "Trait that indicates a relationship is traversable");
    ecs_doc_set_brief(world, EcsExclusive, "Trait that ensures a relationship can only have one target");
    ecs_doc_set_brief(world, EcsSymmetric, "Trait that causes a relationship to be two-way");
    ecs_doc_set_brief(world, EcsWith, "Trait for adding additional components when a component is added");
    ecs_doc_set_brief(world, EcsOneOf, "Trait that enforces target of relationship is a child of <specified>");
    ecs_doc_set_brief(world, EcsOnDelete, "Cleanup trait for specifying what happens when component is deleted");
    ecs_doc_set_brief(world, EcsOnDeleteTarget, "Cleanup trait for specifying what happens when pair target is deleted");
    ecs_doc_set_brief(world, EcsRemove, "Cleanup action used with OnDelete/OnDeleteTarget");
    ecs_doc_set_brief(world, EcsDelete, "Cleanup action used with OnDelete/OnDeleteTarget");
    ecs_doc_set_brief(world, EcsPanic, "Cleanup action used with OnDelete/OnDeleteTarget");
    ecs_doc_set_brief(world, ecs_id(EcsDefaultChildComponent), "Sets default component hint for children of entity");
    ecs_doc_set_brief(world, EcsIsA, "Relationship used for expressing inheritance");
    ecs_doc_set_brief(world, EcsChildOf, "Relationship used for expressing hierarchies");
    ecs_doc_set_brief(world, EcsDependsOn, "Relationship used for expressing dependencies");
    ecs_doc_set_brief(world, EcsSlotOf, "Relationship used for expressing prefab slots");
    ecs_doc_set_brief(world, EcsOnAdd, "Event emitted when component is added");
    ecs_doc_set_brief(world, EcsOnRemove, "Event emitted when component is removed");
    ecs_doc_set_brief(world, EcsOnSet, "Event emitted when component is set");
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
}

/* Doc definitions for doc components */
static
void flecs_doc_import_doc_definitions(
    ecs_world_t *world)
{
    ecs_entity_t doc = ecs_lookup(world, "flecs.doc");
    ecs_doc_set_brief(world, doc, "Flecs module with documentation components");

    ecs_doc_set_brief(world, EcsDocBrief, "Brief description");
    ecs_doc_set_brief(world, EcsDocDetail, "Detailed description");
    ecs_doc_set_brief(world, EcsDocLink, "Link to additional documentation");
    ecs_doc_set_brief(world, EcsDocColor, "Color hint for entity");   

    ecs_doc_set_brief(world, ecs_id(EcsDocDescription), "Component used to add documentation");
    ecs_doc_set_brief(world, EcsDocBrief, "Used as (Description, Brief) to add a brief description");
    ecs_doc_set_brief(world, EcsDocDetail, "Used as (Description, Detail) to add a detailed description");
    ecs_doc_set_brief(world, EcsDocLink, "Used as (Description, Link) to add a link");
}

void FlecsDocImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsDoc);

    ecs_set_name_prefix(world, "EcsDoc");

    flecs_bootstrap_component(world, EcsDocDescription);
    flecs_bootstrap_tag(world, EcsDocBrief);
    flecs_bootstrap_tag(world, EcsDocDetail);
    flecs_bootstrap_tag(world, EcsDocLink);
    flecs_bootstrap_tag(world, EcsDocColor);

    ecs_set_hooks(world, EcsDocDescription, { 
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsDocDescription),
        .copy = ecs_copy(EcsDocDescription),
        .dtor = ecs_dtor(EcsDocDescription)
    });

    ecs_add_pair(world, ecs_id(EcsDocDescription), EcsOnInstantiate, EcsDontInherit);
    ecs_add_id(world, ecs_id(EcsDocDescription), EcsPrivate);

    flecs_doc_import_core_definitions(world);
    flecs_doc_import_doc_definitions(world);
}

#endif
