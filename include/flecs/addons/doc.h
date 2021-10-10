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

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_DOC_H
#define FLECS_DOC_H

#include "meta.h"

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API extern const ecs_entity_t ecs_id(EcsDescription);
FLECS_API extern const ecs_entity_t EcsBrief;
FLECS_API extern const ecs_entity_t EcsDetail;

typedef struct EcsDescription {
    const char *value;
} EcsDescription;

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

/* Module import boilerplate */

typedef struct FlecsDoc {
    int32_t dummy; 
} FlecsDoc;

FLECS_API
void FlecsDocImport(
    ecs_world_t *world);

#define FlecsDocImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif

#endif
