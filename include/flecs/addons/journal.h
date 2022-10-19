/**
 * @file journal.h
 * @brief Journaling addon that logs API functions.
 *
 * The journaling addon traces API calls. The trace is formatted as runnable
 * C code, which allows for (partially) reproducing the behavior of an app
 * with the journaling trace.
 * 
 * The journaling addon is disabled by default. Enabling it can have a 
 * significant impact on performance.
 */

#ifdef FLECS_JOURNAL

#ifndef FLECS_LOG
#define FLECS_LOG
#endif

#ifndef FLECS_JOURNAL_H
#define FLECS_JOURNAL_H

/* Trace when log level is at or higher than level */
#define FLECS_JOURNAL_LOG_LEVEL (0)

#ifdef __cplusplus
extern "C" {
#endif

/* Journaling API as used by the internals. Not intended to be called by apps */

typedef enum ecs_journal_kind_t {
    EcsJournalNew,
    EcsJournalMove,
    EcsJournalClear,
    EcsJournalDelete,
} ecs_journal_kind_t;

FLECS_DBG_API
char* flecs_journal_entitystr(
    ecs_world_t *world,
    ecs_entity_t entity);

FLECS_DBG_API
char* flecs_journal_idstr(
    ecs_world_t *world,
    ecs_id_t id);

FLECS_DBG_API
void flecs_journal_begin(
    ecs_world_t *world,
    ecs_journal_kind_t kind,
    ecs_entity_t entity,
    ecs_type_t *add,
    ecs_type_t *remove);

FLECS_DBG_API
void flecs_journal_end(void);

#define flecs_journal(...)\
    flecs_journal_begin(__VA_ARGS__);\
    flecs_journal_end();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // FLECS_JOURNAL_H
#else
#define flecs_journal_begin(...)
#define flecs_journal_end(...)
#define flecs_journal(...)
#endif // FLECS_JOURNAL
