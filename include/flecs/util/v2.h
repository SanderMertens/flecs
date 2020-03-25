#ifndef FLECS_V2_H
#define FLECS_V2_H

/* Experimental v2 functions. Use at your own risk, API is not stable! */

#ifdef __cplusplus
extern "C" {
#endif

FLECS_EXPORT
ecs_entity_t ecs_run_w_filter_v2(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    uint32_t offset,
    uint32_t limit,
    const ecs_filter_t *filter,
    void *param);

#ifdef __cplusplus
}
#endif

#endif