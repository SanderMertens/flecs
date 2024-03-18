 /**
 * @file query/query.h
 * @brief Query implementation.
 */

#include "compiler.h"
#include "cache.h"
#include "cache_iter.h"
#include "change_detection.h"
#include "trav_cache.h"
#include "trivial_iter.h"
#include "util.h"

#ifdef FLECS_DEBUG
#define flecs_set_var_label(var, lbl) (var)->label = lbl
#else
#define flecs_set_var_label(var, lbl)
#endif

/* Finalize query data & validate */
int flecs_query_finalize_query(
    ecs_world_t *world,
    ecs_query_t *q,
    const ecs_query_desc_t *desc);

/* Internal function for creating iterator, doesn't run aperiodic tasks */
ecs_iter_t flecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *q);
