#ifndef REFLECS_PRIVATE_H
#define REFLECS_PRIVATE_H

#include "types.h"

/* -- Entity API -- */

EcsHandle ecs_new_w_family(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

void ecs_merge_entity(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle entity,
    EcsRow *staged_row);

/* -- World API -- */

EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

void ecs_world_activate_system(
    EcsWorld *world,
    EcsHandle system,
    bool active);

EcsStage *ecs_get_stage(
    EcsWorld **world_ptr);

/* -- Stage API -- */

void ecs_stage_init(
    EcsStage *stage);

void ecs_stage_deinit(
    EcsStage *stage);

void ecs_stage_merge(
    EcsWorld *world,
    EcsStage *stage);

/* -- Family utility API -- */

EcsFamily ecs_family_from_handle(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle entity);

EcsFamily ecs_family_merge(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily cur_id,
    EcsFamily to_add_id,
    EcsFamily to_remove_id);

EcsHandle ecs_family_contains(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id_1,
    EcsFamily family_id_2,
    bool match_all,
    bool match_prefab);

bool ecs_family_contains_component(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family,
    EcsHandle component);

EcsFamily ecs_family_register(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle to_add,
    EcsArray *set);

EcsFamily ecs_family_add(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family,
    EcsHandle component);

void ecs_family_dump(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

EcsArray* ecs_family_get(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

/* -- Table API -- */

EcsResult ecs_table_init(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table);

EcsResult ecs_table_init_w_size(
    EcsWorld *world,
    EcsTable *table,
    EcsArray *family,
    uint32_t size);

uint32_t ecs_table_insert(
    EcsWorld *world,
    EcsTable *table,
    EcsArray **rows,
    EcsHandle entity);

void ecs_table_delete(
    EcsWorld *world,
    EcsTable *table,
    uint32_t index);

void* ecs_table_get(
    EcsTable *table,
    EcsArray *rows,
    uint32_t index);

uint32_t ecs_table_column_offset(
    EcsTable *table,
    EcsHandle component);

bool ecs_table_has_components(
    EcsTable *table,
    EcsArray *components);

void ecs_table_deinit(
    EcsWorld *world,
    EcsTable *table);

/* -- System API -- */

EcsResult ecs_system_notify_create_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle system,
    EcsTable *table);

void ecs_system_activate_table(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table,
    bool active);

void ecs_run_job(
    EcsWorld *world,
    EcsThread *thread,
    EcsJob *job);

void ecs_system_notify(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle system,
    EcsSystem *system_data,
    EcsTable *table,
    uint32_t table_index,
    uint32_t row_index);

/* -- Worker API -- */

void ecs_schedule_jobs(
    EcsWorld *world,
    EcsHandle system);

void ecs_prepare_jobs(
    EcsWorld *world,
    EcsHandle system);

void ecs_run_jobs(
    EcsWorld *world);

/* -- Private utilities -- */

void ecs_hash(
    const void *key,
    size_t length,
    uint32_t *result);

EcsRow ecs_to_row(
    uint64_t value);

uint64_t ecs_from_row(
    EcsRow row);

EcsResult ecs_parse_component_expr(
    EcsWorld *world,
    const char *sig,
    ecs_parse_action action,
    void *ctx);

#define assert_func(cond) _assert_func(cond, #cond, __FILE__, __LINE__, __func__)
void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    uint32_t line,
    const char *func);

#endif
