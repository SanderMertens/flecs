/**
 * @file addons/doc.h
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

/**
 * @defgroup c_addons_doc Doc
 * @ingroup c_addons
 * Utilities for documenting entities, components and systems.
 *
 * @{
 */

FLECS_API extern const ecs_entity_t ecs_id(EcsDocDescription); /**< Component id for EcsDocDescription. */

/** Tag for adding brief descriptions to entities. 
 * Added to an entity as (EcsDocDescription, EcsBrief) by ecs_doc_set_brief().
 */
FLECS_API extern const ecs_entity_t EcsDocBrief;

/** Tag for adding detailed descriptions to entities. 
 * Added to an entity as (EcsDocDescription, EcsDocDetail) by ecs_doc_set_detail().
 */
FLECS_API extern const ecs_entity_t EcsDocDetail;

/** Tag for adding a link to entities. 
 * Added to an entity as (EcsDocDescription, EcsDocLink) by ecs_doc_set_link().
 */
FLECS_API extern const ecs_entity_t EcsDocLink;

/** Tag for adding a color to entities. 
 * Added to an entity as (EcsDocDescription, EcsDocColor) by ecs_doc_set_link().
 */
FLECS_API extern const ecs_entity_t EcsDocColor;

/** Component that stores description.
 * Used as pair together with the following tags to store entity documentation:
 * - EcsName
 * - EcsDocBrief
 * - EcsDocDetail
 * - EcsDocLink
 * - EcsDocColor
 */
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
 *
 * @see ecs_doc_get_name()
 * @see flecs::doc::set_name()
 * @see flecs::entity_builder::set_doc_name()
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
 *
 * @see ecs_doc_get_brief()
 * @see flecs::doc::set_brief()
 * @see flecs::entity_builder::set_doc_brief()
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
 *
 * @see ecs_doc_get_detail()
 * @see flecs::doc::set_detail()
 * @see flecs::entity_builder::set_doc_detail()
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
 *
 * @see ecs_doc_get_link()
 * @see flecs::doc::set_link()
 * @see flecs::entity_builder::set_doc_link()
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
 *
 * @see ecs_doc_get_color()
 * @see flecs::doc::set_color()
 * @see flecs::entity_builder::set_doc_color()
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
 *
 * @code
 * ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsName);
 * @endcode
 *
 * Or in C++:
 *
 * @code
 * e.has<flecs::doc::Description>(flecs::Name);
 * @endcode
 *
 * @param world The world.
 * @param entity The entity from which to get the name.
 * @return The name.
 *
 * @see ecs_doc_set_name()
 * @see flecs::doc::get_name()
 * @see flecs::entity_view::get_doc_name()
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
 *
 * @see ecs_doc_set_brief()
 * @see flecs::doc::get_brief()
 * @see flecs::entity_view::get_doc_brief()
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
 *
 * @see ecs_doc_set_detail()
 * @see flecs::doc::get_detail()
 * @see flecs::entity_view::get_doc_detail()
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
 *
 * @see ecs_doc_set_link()
 * @see flecs::doc::get_link()
 * @see flecs::entity_view::get_doc_link()
 */
FLECS_API
const char* ecs_doc_get_link(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get color from entity.
 *
 * @param world The world.
 * @param entity The entity from which to get the color.
 * @return The color.
 *
 * @see ecs_doc_set_color()
 * @see flecs::doc::get_color()
 * @see flecs::entity_view::get_doc_color()
 */
FLECS_API
const char* ecs_doc_get_color(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Doc module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsDoc)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsDocImport(
    ecs_world_t *world);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

#endif
