/**
 * @file addons/units.h
 * @brief Units module.
 *
 * Builtin standard units. The units addon is not imported by default, even if
 * the addon is included in the build. To import the module, do:
 *
 * In C:
 *
 * @code
 * ECS_IMPORT(world, FlecsUnits);
 * @endcode
 *
 * In C++:
 *
 * @code
 * world.import<flecs::units>();
 * @endcode
 *
 * As a result this module behaves just like an application-defined module,
 * which means that the ids generated for the entities inside the module are not
 * fixed, and depend on the order in which the module is imported.
 */

#ifdef FLECS_UNITS

/**
 * @defgroup c_addons_units Units.
 * @ingroup c_addons
 * Common unit annotations for reflection framework.
 *
 * @{
 */

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

/**
 * @defgroup c_addons_units_prefixes Prefixes
 * @ingroup c_addons_units
 * Prefixes to indicate unit count (e.g. Kilo, Mega)
 *
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsUnitPrefixes); /**< Parent scope for prefixes. */

FLECS_API extern ECS_DECLARE(EcsYocto);  /**< Yocto unit prefix. */
FLECS_API extern ECS_DECLARE(EcsZepto);  /**< Zepto unit prefix. */
FLECS_API extern ECS_DECLARE(EcsAtto);   /**< Atto unit prefix. */
FLECS_API extern ECS_DECLARE(EcsFemto);  /**< Femto unit prefix. */
FLECS_API extern ECS_DECLARE(EcsPico);   /**< Pico unit prefix. */
FLECS_API extern ECS_DECLARE(EcsNano);   /**< Nano unit prefix. */
FLECS_API extern ECS_DECLARE(EcsMicro);  /**< Micro unit prefix. */
FLECS_API extern ECS_DECLARE(EcsMilli);  /**< Milli unit prefix. */
FLECS_API extern ECS_DECLARE(EcsCenti);  /**< Centi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsDeci);   /**< Deci unit prefix. */
FLECS_API extern ECS_DECLARE(EcsDeca);   /**< Deca unit prefix. */
FLECS_API extern ECS_DECLARE(EcsHecto);  /**< Hecto unit prefix. */
FLECS_API extern ECS_DECLARE(EcsKilo);   /**< Kilo unit prefix. */
FLECS_API extern ECS_DECLARE(EcsMega);   /**< Mega unit prefix. */
FLECS_API extern ECS_DECLARE(EcsGiga);   /**< Giga unit prefix. */
FLECS_API extern ECS_DECLARE(EcsTera);   /**< Tera unit prefix. */
FLECS_API extern ECS_DECLARE(EcsPeta);   /**< Peta unit prefix. */
FLECS_API extern ECS_DECLARE(EcsExa);    /**< Exa unit prefix. */
FLECS_API extern ECS_DECLARE(EcsZetta);  /**< Zetta unit prefix. */
FLECS_API extern ECS_DECLARE(EcsYotta);  /**< Yotta unit prefix. */

FLECS_API extern ECS_DECLARE(EcsKibi);   /**< Kibi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsMebi);   /**< Mebi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsGibi);   /**< Gibi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsTebi);   /**< Tebi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsPebi);   /**< Pebi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsExbi);   /**< Exbi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsZebi);   /**< Zebi unit prefix. */
FLECS_API extern ECS_DECLARE(EcsYobi);   /**< Yobi unit prefix. */

/** @} */

/**
 * @defgroup c_addons_units_duration Duration
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsDuration);         /**< Duration quantity. */
FLECS_API extern     ECS_DECLARE(EcsPicoSeconds);  /**< PicoSeconds duration unit. */
FLECS_API extern     ECS_DECLARE(EcsNanoSeconds);  /**< NanoSeconds duration unit. */
FLECS_API extern     ECS_DECLARE(EcsMicroSeconds); /**< MicroSeconds duration unit. */
FLECS_API extern     ECS_DECLARE(EcsMilliSeconds); /**< MilliSeconds duration unit. */
FLECS_API extern     ECS_DECLARE(EcsSeconds);      /**< Seconds duration unit. */
FLECS_API extern     ECS_DECLARE(EcsMinutes);      /**< Minutes duration unit. */
FLECS_API extern     ECS_DECLARE(EcsHours);        /**< Hours duration unit. */
FLECS_API extern     ECS_DECLARE(EcsDays);         /**< Days duration unit. */

/** @} */

/**
 * @defgroup c_addons_units_time Time
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsTime);             /**< Time quantity. */
FLECS_API extern     ECS_DECLARE(EcsDate);         /**< Date unit. */

/** @} */

/**
 * @defgroup c_addons_units_mass Mass
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsMass);             /**< Mass quantity. */
FLECS_API extern     ECS_DECLARE(EcsGrams);        /**< Grams unit. */
FLECS_API extern     ECS_DECLARE(EcsKiloGrams);    /**< KiloGrams unit. */

/** @} */

/**
 * @defgroup c_addons_units_electric_Current Electric Current
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsElectricCurrent);  /**< ElectricCurrent quantity. */
FLECS_API extern     ECS_DECLARE(EcsAmpere);       /**< Ampere unit. */

/** @} */

/**
 * @defgroup c_addons_units_amount Amount
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsAmount);           /**< Amount quantity. */
FLECS_API extern     ECS_DECLARE(EcsMole);         /**< Mole unit. */

/** @} */

/**
 * @defgroup c_addons_units_luminous_intensity Luminous Intensity
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsLuminousIntensity); /**< LuminousIntensity quantity. */
FLECS_API extern     ECS_DECLARE(EcsCandela);       /**< Candela unit. */

/** @} */

/**
 * @defgroup c_addons_units_force Force
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsForce);            /**< Force quantity. */
FLECS_API extern     ECS_DECLARE(EcsNewton);       /**< Newton unit. */

/** @} */

/**
 * @defgroup c_addons_units_length Length
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsLength);              /**< Length quantity. */
FLECS_API extern     ECS_DECLARE(EcsMeters);          /**< Meters unit. */
FLECS_API extern         ECS_DECLARE(EcsPicoMeters);  /**< PicoMeters unit. */
FLECS_API extern         ECS_DECLARE(EcsNanoMeters);  /**< NanoMeters unit. */
FLECS_API extern         ECS_DECLARE(EcsMicroMeters); /**< MicroMeters unit. */
FLECS_API extern         ECS_DECLARE(EcsMilliMeters); /**< MilliMeters unit. */
FLECS_API extern         ECS_DECLARE(EcsCentiMeters); /**< CentiMeters unit. */
FLECS_API extern         ECS_DECLARE(EcsKiloMeters);  /**< KiloMeters unit. */
FLECS_API extern     ECS_DECLARE(EcsMiles);           /**< Miles unit. */
FLECS_API extern     ECS_DECLARE(EcsPixels);          /**< Pixels unit. */

/** @} */

/**
 * @defgroup c_addons_units_pressure Pressure
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsPressure);          /**< Pressure quantity. */
FLECS_API extern     ECS_DECLARE(EcsPascal);        /**< Pascal unit. */
FLECS_API extern     ECS_DECLARE(EcsBar);           /**< Bar unit. */

/** @} */

/**
 * @defgroup c_addons_units_speed Speed
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsSpeed);                   /**< Speed quantity. */
FLECS_API extern     ECS_DECLARE(EcsMetersPerSecond);     /**< MetersPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerSecond); /**< KiloMetersPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerHour);   /**< KiloMetersPerHour unit. */
FLECS_API extern     ECS_DECLARE(EcsMilesPerHour);        /**< MilesPerHour unit. */

/** @} */

/**
 * @defgroup c_addons_units_temperature Temperature
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsTemperature);       /**< Temperature quantity. */
FLECS_API extern     ECS_DECLARE(EcsKelvin);        /**< Kelvin unit. */
FLECS_API extern     ECS_DECLARE(EcsCelsius);       /**< Celsius unit. */
FLECS_API extern     ECS_DECLARE(EcsFahrenheit);    /**< Fahrenheit unit. */

/** @} */

/**
 * @defgroup c_addons_units_data Data
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsData);               /**< Data quantity. */
FLECS_API extern     ECS_DECLARE(EcsBits);           /**< Bits unit. */
FLECS_API extern         ECS_DECLARE(EcsKiloBits);   /**< KiloBits unit. */
FLECS_API extern         ECS_DECLARE(EcsMegaBits);   /**< MegaBits unit. */
FLECS_API extern         ECS_DECLARE(EcsGigaBits);   /**< GigaBits unit. */
FLECS_API extern     ECS_DECLARE(EcsBytes);          /**< Bytes unit. */
FLECS_API extern         ECS_DECLARE(EcsKiloBytes);  /**< KiloBytes unit. */
FLECS_API extern         ECS_DECLARE(EcsMegaBytes);  /**< MegaBytes unit. */
FLECS_API extern         ECS_DECLARE(EcsGigaBytes);  /**< GigaBytes unit. */
FLECS_API extern         ECS_DECLARE(EcsKibiBytes);  /**< KibiBytes unit. */
FLECS_API extern         ECS_DECLARE(EcsMebiBytes);  /**< MebiBytes unit. */
FLECS_API extern         ECS_DECLARE(EcsGibiBytes);  /**< GibiBytes unit. */

/** @} */

/**
 * @defgroup c_addons_units_datarate Data Rate
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsDataRate);               /**< DataRate quantity. */
FLECS_API extern     ECS_DECLARE(EcsBitsPerSecond);      /**< BitsPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsKiloBitsPerSecond);  /**< KiloBitsPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsMegaBitsPerSecond);  /**< MegaBitsPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsGigaBitsPerSecond);  /**< GigaBitsPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsBytesPerSecond);     /**< BytesPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsKiloBytesPerSecond); /**< KiloBytesPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsMegaBytesPerSecond); /**< MegaBytesPerSecond unit. */
FLECS_API extern     ECS_DECLARE(EcsGigaBytesPerSecond); /**< GigaBytesPerSecond unit. */

/** @} */

/**
 * @defgroup c_addons_units_duration Duration
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsAngle);            /**< Angle quantity. */
FLECS_API extern     ECS_DECLARE(EcsRadians);      /**< Radians unit. */
FLECS_API extern     ECS_DECLARE(EcsDegrees);      /**< Degrees unit. */

/** @} */

/**
 * @defgroup c_addons_units_angle Angle
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsFrequency);        /**< Frequency quantity. */
FLECS_API extern     ECS_DECLARE(EcsHertz);        /**< Hertz unit. */
FLECS_API extern     ECS_DECLARE(EcsKiloHertz);    /**< KiloHertz unit. */
FLECS_API extern     ECS_DECLARE(EcsMegaHertz);    /**< MegaHertz unit. */
FLECS_API extern     ECS_DECLARE(EcsGigaHertz);    /**< GigaHertz unit. */

/** @} */

/**
 * @defgroup c_addons_units_uri Uri
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsUri);              /**< URI quantity. */
FLECS_API extern     ECS_DECLARE(EcsUriHyperlink); /**< UriHyperlink unit. */
FLECS_API extern     ECS_DECLARE(EcsUriImage);     /**< UriImage unit. */
FLECS_API extern     ECS_DECLARE(EcsUriFile);      /**< UriFile unit. */

/** @} */

/**
 * @defgroup c_addons_units_color Color
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsColor);            /**< Color quantity. */
FLECS_API extern     ECS_DECLARE(EcsColorRgb);     /**< ColorRgb unit. */
FLECS_API extern     ECS_DECLARE(EcsColorHsl);     /**< ColorHsl unit. */
FLECS_API extern     ECS_DECLARE(EcsColorCss);     /**< ColorCss unit. */

/** @} */


FLECS_API extern ECS_DECLARE(EcsAcceleration);     /**< Acceleration unit. */
FLECS_API extern ECS_DECLARE(EcsPercentage);       /**< Percentage unit. */
FLECS_API extern ECS_DECLARE(EcsBel);              /**< Bel unit. */
FLECS_API extern ECS_DECLARE(EcsDeciBel);          /**< DeciBel unit. */

////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

/** Units module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsUnits)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsUnitsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
