/**
 * @file addons/system/system.h
 * @brief Internal types and functions for system addon.
 */

#ifndef FLECS_SYSTEM_PRIVATE_H
#define FLECS_SYSTEM_PRIVATE_H

#ifdef FLECS_SYSTEM

#include "../../private_api.h"

#define ecs_system_t_magic     (0x65637383)
#define ecs_system_t_tag       EcsSystem

extern ecs_mixins_t ecs_system_t_mixins;

/* Invoked when system becomes active / inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate,
    const ecs_system_t *system_data);

/* Internal function to run a system */
ecs_entity_t flecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_system_t *system_data,
    int32_t stage_current,
    int32_t stage_count,
    ecs_ftime_t delta_time,
    void *param);

#endif

#endif
