/**
 * @file units.h
 * @brief Units module.
 *
 * Builtin standard units. The units addon is not imported by default, even if
 * the addon is included in the build. To import the module, do:
 *
 * In C:
 *   ECS_IMPORT(world, FlecsUnits);
 * 
 * In C++:
 *   world.import<flecs::units>();
 *
 * As a result this module behaves just like an application-defined module, 
 * which means that the ids generated for the entities inside the module are not
 * fixed, and depend on the order in which the module is imported.
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

FLECS_API extern ECS_DECLARE(EcsUnitPrefixes); /* Parent scope for prefixes */

FLECS_API extern ECS_DECLARE(EcsYocto);
FLECS_API extern ECS_DECLARE(EcsZepto);
FLECS_API extern ECS_DECLARE(EcsAtto);
FLECS_API extern ECS_DECLARE(EcsFemto);
FLECS_API extern ECS_DECLARE(EcsPico);
FLECS_API extern ECS_DECLARE(EcsNano);
FLECS_API extern ECS_DECLARE(EcsMicro);
FLECS_API extern ECS_DECLARE(EcsMilli);
FLECS_API extern ECS_DECLARE(EcsCenti);
FLECS_API extern ECS_DECLARE(EcsDeci);
FLECS_API extern ECS_DECLARE(EcsDeca);
FLECS_API extern ECS_DECLARE(EcsHecto);
FLECS_API extern ECS_DECLARE(EcsKilo);
FLECS_API extern ECS_DECLARE(EcsMega);
FLECS_API extern ECS_DECLARE(EcsGiga);
FLECS_API extern ECS_DECLARE(EcsTera);
FLECS_API extern ECS_DECLARE(EcsPeta);
FLECS_API extern ECS_DECLARE(EcsExa);
FLECS_API extern ECS_DECLARE(EcsZetta);
FLECS_API extern ECS_DECLARE(EcsYotta);

FLECS_API extern ECS_DECLARE(EcsKibi);
FLECS_API extern ECS_DECLARE(EcsMebi);
FLECS_API extern ECS_DECLARE(EcsGibi);
FLECS_API extern ECS_DECLARE(EcsTebi);
FLECS_API extern ECS_DECLARE(EcsPebi);
FLECS_API extern ECS_DECLARE(EcsExbi);
FLECS_API extern ECS_DECLARE(EcsZebi);
FLECS_API extern ECS_DECLARE(EcsYobi);


////////////////////////////////////////////////////////////////////////////////
//// Units & quantities
////////////////////////////////////////////////////////////////////////////////

FLECS_API extern ECS_DECLARE(EcsDuration);
FLECS_API extern     ECS_DECLARE(EcsPicoSeconds);
FLECS_API extern     ECS_DECLARE(EcsNanoSeconds);
FLECS_API extern     ECS_DECLARE(EcsMicroSeconds);
FLECS_API extern     ECS_DECLARE(EcsMilliSeconds);
FLECS_API extern     ECS_DECLARE(EcsSeconds);
FLECS_API extern     ECS_DECLARE(EcsMinutes);
FLECS_API extern     ECS_DECLARE(EcsHours);
FLECS_API extern     ECS_DECLARE(EcsDays);

FLECS_API extern ECS_DECLARE(EcsTime);
FLECS_API extern     ECS_DECLARE(EcsDate);

FLECS_API extern ECS_DECLARE(EcsMass);
FLECS_API extern     ECS_DECLARE(EcsGrams);
FLECS_API extern     ECS_DECLARE(EcsKiloGrams);

FLECS_API extern ECS_DECLARE(EcsElectricCurrent);
FLECS_API extern     ECS_DECLARE(EcsAmpere);

FLECS_API extern ECS_DECLARE(EcsAmount);
FLECS_API extern     ECS_DECLARE(EcsMole);

FLECS_API extern ECS_DECLARE(EcsLuminousIntensity);
FLECS_API extern     ECS_DECLARE(EcsCandela);

FLECS_API extern ECS_DECLARE(EcsForce);
FLECS_API extern     ECS_DECLARE(EcsNewton);

FLECS_API extern ECS_DECLARE(EcsLength);
FLECS_API extern     ECS_DECLARE(EcsMeters);
FLECS_API extern         ECS_DECLARE(EcsPicoMeters);
FLECS_API extern         ECS_DECLARE(EcsNanoMeters);
FLECS_API extern         ECS_DECLARE(EcsMicroMeters);
FLECS_API extern         ECS_DECLARE(EcsMilliMeters);
FLECS_API extern         ECS_DECLARE(EcsCentiMeters);
FLECS_API extern         ECS_DECLARE(EcsKiloMeters);
FLECS_API extern     ECS_DECLARE(EcsMiles);

FLECS_API extern ECS_DECLARE(EcsPressure);
FLECS_API extern     ECS_DECLARE(EcsPascal);
FLECS_API extern     ECS_DECLARE(EcsBar);

FLECS_API extern ECS_DECLARE(EcsSpeed);
FLECS_API extern     ECS_DECLARE(EcsMetersPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerHour);
FLECS_API extern     ECS_DECLARE(EcsMilesPerHour);

FLECS_API extern ECS_DECLARE(EcsTemperature);
FLECS_API extern     ECS_DECLARE(EcsKelvin);
FLECS_API extern     ECS_DECLARE(EcsCelsius);
FLECS_API extern     ECS_DECLARE(EcsFahrenheit);

FLECS_API extern ECS_DECLARE(EcsAcceleration);

FLECS_API extern ECS_DECLARE(EcsData);
FLECS_API extern     ECS_DECLARE(EcsBits);
FLECS_API extern         ECS_DECLARE(EcsKiloBits);
FLECS_API extern         ECS_DECLARE(EcsMegaBits);
FLECS_API extern         ECS_DECLARE(EcsGigaBits);
FLECS_API extern     ECS_DECLARE(EcsBytes);
FLECS_API extern         ECS_DECLARE(EcsKiloBytes);
FLECS_API extern         ECS_DECLARE(EcsMegaBytes);
FLECS_API extern         ECS_DECLARE(EcsGigaBytes);
FLECS_API extern         ECS_DECLARE(EcsKibiBytes);
FLECS_API extern         ECS_DECLARE(EcsMebiBytes);
FLECS_API extern         ECS_DECLARE(EcsGibiBytes);

FLECS_API extern ECS_DECLARE(EcsDataRate);
FLECS_API extern     ECS_DECLARE(EcsBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsMegaBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsGigaBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsMegaBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsGigaBytesPerSecond);

FLECS_API extern ECS_DECLARE(EcsPercentage);

FLECS_API extern ECS_DECLARE(EcsAngle);
FLECS_API extern     ECS_DECLARE(EcsRadians);
FLECS_API extern     ECS_DECLARE(EcsDegrees);

FLECS_API extern ECS_DECLARE(EcsBel);
FLECS_API extern ECS_DECLARE(EcsDeciBel);

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
