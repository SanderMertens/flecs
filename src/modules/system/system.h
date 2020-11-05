#ifndef FLECS_SYSTEM_PRIVATE_H
#define FLECS_SYSTEM_PRIVATE_H

#include "../../private_api.h"

typedef struct EcsSystem {
    ecs_iter_action_t action;       /* Callback to be invoked for matching it */
    void *ctx;                      /* Userdata for system */

    ecs_entity_t entity;                  /* Entity id of system, used for ordering */
    ecs_query_t *query;                   /* System query */
    ecs_on_demand_out_t *on_demand;       /* Keep track of [out] column refs */
    ecs_system_status_action_t status_action; /* Status action */
    void *status_ctx;                     /* User data for status action */    
    ecs_entity_t tick_source;             /* Tick source associated with system */
    
    int32_t invoke_count;                 /* Number of times system is invoked */
    double time_spent;                     /* Time spent on running system */
    double time_passed;                    /* Time passed since last invocation */
} EcsSystem;

/* Invoked when system becomes active / inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate,
    const EcsSystem *system_data);

/* Internal function to run a system */
ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    EcsSystem *system_data,
    double delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param,
    bool ran_by_app);

#endif
