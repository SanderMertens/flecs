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

#include "module.h"

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API extern const ecs_entity_t ecs_id(EcsDocDescription);
FLECS_API extern const ecs_entity_t EcsDocBrief;
FLECS_API extern const ecs_entity_t EcsDocDetail;
FLECS_API extern const ecs_entity_t EcsDocLink;

typedef struct EcsDocDescription {
    const char *value;
} EcsDocDescription;

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

/* Module import */
FLECS_API
void FlecsDocImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif
