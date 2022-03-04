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
//// Unit prefixes
////////////////////////////////////////////////////////////////////////////////

extern ECS_DECLARE(EcsPico);
extern ECS_DECLARE(EcsNano);
extern ECS_DECLARE(EcsMicro);
extern ECS_DECLARE(EcsMilli);
extern ECS_DECLARE(EcsCenti);
extern ECS_DECLARE(EcsDeci);
extern ECS_DECLARE(EcsDeca);
extern ECS_DECLARE(EcsHecto);
extern ECS_DECLARE(EcsKilo);
extern ECS_DECLARE(EcsMega);
extern ECS_DECLARE(EcsGiga);
extern ECS_DECLARE(EcsTera);
extern ECS_DECLARE(EcsPeta);
extern ECS_DECLARE(EcsZetta);
extern ECS_DECLARE(EcsYotta);

extern ECS_DECLARE(EcsKibi);
extern ECS_DECLARE(EcsMebi);
extern ECS_DECLARE(EcsGibi);
extern ECS_DECLARE(EcsTebi);
extern ECS_DECLARE(EcsPebi);
extern ECS_DECLARE(EcsExbi);
extern ECS_DECLARE(EcsZebi);
extern ECS_DECLARE(EcsYobi);


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
