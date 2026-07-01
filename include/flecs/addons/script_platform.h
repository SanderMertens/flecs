/**
 * @file addons/script_platform.h
 * @brief Platform constants for Flecs script.
 */

#ifdef FLECS_SCRIPT_PLATFORM

#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif

/**
 * @defgroup c_addons_script_platform Script Platform
 * @ingroup c_addons
 * Platform constants for Flecs script.
 * @{
 */

#ifndef FLECS_SCRIPT_PLATFORM_H
#define FLECS_SCRIPT_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/** Script platform import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsScriptPlatform)
 * @endcode
 *
 * @param world The world.
 */
FLECS_API
void FlecsScriptPlatformImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
