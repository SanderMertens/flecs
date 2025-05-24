 /**
 * @file query/query.h
 * @brief Query implementation.
 */

#include "compiler/compiler.h"
#include "cache/cache.h"
#include "engine/engine.h"
#include "util.h"

#ifdef FLECS_DEBUG
#define flecs_set_var_label(var, lbl) (var)->label = lbl
#else
#define flecs_set_var_label(var, lbl)
#endif

/* Fast function for finalizing simple queries */
bool flecs_query_finalize_simple(
    ecs_world_t *world,
    ecs_query_t *q,
    const ecs_query_desc_t *desc);

/* Finalize query data & validate */
int flecs_query_finalize_query(
    ecs_world_t *world,
    ecs_query_t *q,
    const ecs_query_desc_t *desc);

/* Copy terms, sizes and ids arrays from stack to heap */
void flecs_query_copy_arrays(
    ecs_query_t *q);

/* Free terms, sizes and ids arrays */
void flecs_query_free_arrays(
    ecs_query_t *q);

/* Internal function for creating iterator, doesn't run aperiodic tasks */
ecs_iter_t flecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *q);

/* Internal function for initializing an iterator after vars are constrained */
void flecs_query_iter_constrain(
    ecs_iter_t *it);

/* Rematch query after cache could have been invalidated */
void flecs_query_rematch(
    ecs_world_t *world,
    ecs_query_t *q);

