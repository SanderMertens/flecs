/**
 * @file units.h
 * @brief Units module.
 *
 * Builtin standard units.
 */

#ifdef FLECS_UNITS

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_UNITS_H
#define FLECS_UNITS_H

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Units & quantities
////////////////////////////////////////////////////////////////////////////////

extern ECS_DECLARE(EcsDuration);
extern     ECS_DECLARE(EcsSeconds);

extern ECS_DECLARE(EcsTime);
extern     ECS_DECLARE(EcsDate);

extern ECS_DECLARE(EcsPercentage);

extern ECS_DECLARE(EcsLength);
extern     ECS_DECLARE(EcsMeters);
extern     ECS_DECLARE(EcsMiles);

extern ECS_DECLARE(EcsInformation);
extern     ECS_DECLARE(EcsBits);
extern     ECS_DECLARE(EcsBytes);


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void FlecsUnitsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif
