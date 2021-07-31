#ifdef FLECS_PLECS

#ifndef FLECS_PLECS_H
#define FLECS_PLECS_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
int ecs_plecs_from_str(
    ecs_world_t *world,
    const char *name,
    const char *str);

FLECS_API
int ecs_plecs_from_file(
    ecs_world_t *world,
    const char *filename);

#ifdef __cplusplus
}
#endif

#endif

#endif
