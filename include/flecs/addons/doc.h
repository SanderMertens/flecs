/**
 * @file doc.h
 * @brief Doc module.
 *
 * The doc module allows for documenting entities (and thus components, systems)
 * by adding brief and/or detailed descriptions as components. Documentation
 * added with the doc module can be retrieved at runtime, and can be used by
 * tooling such as UIs or documentation frameworks.
 */

#ifdef FLECS_DOC

#ifndef FLECS_DOC_H
#define FLECS_DOC_H

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API extern const ecs_entity_t ecs_id(EcsDocDescription);
FLECS_API extern const ecs_entity_t EcsDocBrief;
FLECS_API extern const ecs_entity_t EcsDocDetail;
FLECS_API extern const ecs_entity_t EcsDocLink;
FLECS_API extern const ecs_entity_t EcsDocColor;

typedef struct EcsDocDescription {
    char *value;
} EcsDocDescription;

/** Add human-readable name to entity.
 * Contrary to entity names, human readable names do not have to be unique and
 * can contain special characters used in the query language like '*'.
 * 
 * @param world The world.
 * @param entity The entity to which to add the name.
 * @param name The name to add.
 */
FLECS_API
void ecs_doc_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name);

/** Add brief description to entity.
 * 
 * @param world The world.
 * @param entity The entity to which to add the description.
 * @param description The description to add.
 */
FLECS_API
void ecs_doc_set_brief(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description);

/** Add detailed description to entity.
 * 
 * @param world The world.
 * @param entity The entity to which to add the description.
 * @param description The description to add.
 */
FLECS_API
void ecs_doc_set_detail(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description);

/** Add link to external documentation to entity.
 * 
 * @param world The world.
 * @param entity The entity to which to add the link.
 * @param link The link to add.
 */
FLECS_API
void ecs_doc_set_link(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *link);

/** Add color to entity.
 * UIs can use color as hint to improve visualizing entities.
 * 
 * @param world The world.
 * @param entity The entity to which to add the link.
 * @param color The color to add.
 */
FLECS_API
void ecs_doc_set_color(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *color);

/** Get human readable name from entity.
 * If entity does not have an explicit human readable name, this operation will
 * return the entity name.
 * 
 * To test if an entity has a human readable name, use:
 *   ecs_has_pair(world, e, ecs_id(EcsDescription), EcsName);
 * Or in C++:
 *   e.has<flecs::Description>(flecs::Name);
 * 
 * @param world The world.
 * @param entity The entity from which to get the name.
 * @return The name.
 */
FLECS_API
const char* ecs_doc_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get brief description from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the description.
 * @return The description.
 */
FLECS_API
const char* ecs_doc_get_brief(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get detailed description from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the description.
 * @return The description.
 */
FLECS_API
const char* ecs_doc_get_detail(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get link to external documentation from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the link.
 * @return The link.
 */
FLECS_API
const char* ecs_doc_get_link(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get color from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the link.
 * @return The color.
 */
FLECS_API
const char* ecs_doc_get_color(
    const ecs_world_t *world,
    ecs_entity_t entity);

/* Module import */
FLECS_API
void FlecsDocImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif
