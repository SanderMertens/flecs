/**
 * @file coredoc.h
 * @brief Core doc module.
 *
 * The core doc module imports documentation and reflection data for core
 * components, tags and systems.
 */

#ifdef FLECS_COREDOC

#ifndef FLECS_DOC
#define FLECS_DOC
#endif

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_COREDOC_H
#define FLECS_COREDOC_H

#include "doc.h"
#include "meta.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Module import boilerplate */

typedef struct FlecsCoreDoc {
    int32_t dummy; 
} FlecsCoreDoc;

FLECS_API
void FlecsCoreDocImport(
    ecs_world_t *world);

#define FlecsCoreDocImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif

#endif
