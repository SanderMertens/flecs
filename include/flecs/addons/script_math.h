/**
 * @file addons/script_math.h
 * @brief Math functions for flecs script.
 */

#ifdef FLECS_SCRIPT_MATH

#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif

/**
 * @defgroup c_addons_script_math Script Math
 * @ingroup c_addons
 * Math functions for flecs script.
 * @{
 */

#ifndef FLECS_SCRIPT_MATH_H
#define FLECS_SCRIPT_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptRng);

/* Randon number generator */
typedef struct {
    uint64_t seed;
    void *impl;
} EcsScriptRng;

/** Script math import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsScriptMath)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsScriptMathImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
