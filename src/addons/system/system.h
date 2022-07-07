#ifndef FLECS_SYSTEM_PRIVATE_H
#define FLECS_SYSTEM_PRIVATE_H

#ifdef FLECS_SYSTEM

#include "../../private_api.h"

#define ecs_system_t_magic     (0x65637383)
#define ecs_system_t_tag       EcsSystem

extern ecs_mixins_t ecs_system_t_mixins;

typedef struct ecs_system_t {
    ecs_header_t hdr;

    ecs_run_action_t run;           /* See ecs_system_desc_t */
    ecs_iter_action_t action;       /* See ecs_system_desc_t */

    ecs_query_t *query;             /* System query */
    ecs_entity_t query_entity;      /* Entity associated with query */
    ecs_entity_t tick_source;       /* Tick source associated with system */
    
    /* Schedule parameters */
    bool multi_threaded;
    bool no_staging;

    int32_t invoke_count;           /* Number of times system is invoked */
    float time_spent;               /* Time spent on running system */
    ecs_ftime_t time_passed;        /* Time passed since last invocation */
    int32_t last_frame;             /* Last frame for which the system was considered */

    void *ctx;                      /* Userdata for system */
    void *binding_ctx;              /* Optional language binding context */

    ecs_ctx_free_t ctx_free;
    ecs_ctx_free_t binding_ctx_free;

    /* Mixins */
    ecs_world_t *world;
    ecs_entity_t entity;
    ecs_poly_dtor_t dtor;      
} ecs_system_t;

/* Invoked when system becomes active / inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate,
    const ecs_system_t *system_data);

/* Internal function to run a system */
ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_system_t *system_data,
    int32_t stage_current,
    int32_t stage_count,
    ecs_ftime_t delta_time,
    int32_t offset,
    int32_t limit,
    void *param);

#endif

#endif
