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

FLECS_API extern ecs_entity_t EcsUnitPrefixes; /**< Parent scope for prefixes. */

FLECS_API extern ecs_entity_t EcsYocto;  /**< Yocto unit prefix. */
FLECS_API extern ecs_entity_t EcsZepto;  /**< Zepto unit prefix. */
FLECS_API extern ecs_entity_t EcsAtto;   /**< Atto unit prefix. */
FLECS_API extern ecs_entity_t EcsFemto;  /**< Femto unit prefix. */
FLECS_API extern ecs_entity_t EcsPico;   /**< Pico unit prefix. */
FLECS_API extern ecs_entity_t EcsNano;   /**< Nano unit prefix. */
FLECS_API extern ecs_entity_t EcsMicro;  /**< Micro unit prefix. */
FLECS_API extern ecs_entity_t EcsMilli;  /**< Milli unit prefix. */
FLECS_API extern ecs_entity_t EcsCenti;  /**< Centi unit prefix. */
FLECS_API extern ecs_entity_t EcsDeci;   /**< Deci unit prefix. */
FLECS_API extern ecs_entity_t EcsDeca;   /**< Deca unit prefix. */
FLECS_API extern ecs_entity_t EcsHecto;  /**< Hecto unit prefix. */
FLECS_API extern ecs_entity_t EcsKilo;   /**< Kilo unit prefix. */
FLECS_API extern ecs_entity_t EcsMega;   /**< Mega unit prefix. */
FLECS_API extern ecs_entity_t EcsGiga;   /**< Giga unit prefix. */
FLECS_API extern ecs_entity_t EcsTera;   /**< Tera unit prefix. */
FLECS_API extern ecs_entity_t EcsPeta;   /**< Peta unit prefix. */
FLECS_API extern ecs_entity_t EcsExa;    /**< Exa unit prefix. */
FLECS_API extern ecs_entity_t EcsZetta;  /**< Zetta unit prefix. */
FLECS_API extern ecs_entity_t EcsYotta;  /**< Yotta unit prefix. */

FLECS_API extern ecs_entity_t EcsKibi;   /**< Kibi unit prefix. */
FLECS_API extern ecs_entity_t EcsMebi;   /**< Mebi unit prefix. */
FLECS_API extern ecs_entity_t EcsGibi;   /**< Gibi unit prefix. */
FLECS_API extern ecs_entity_t EcsTebi;   /**< Tebi unit prefix. */
FLECS_API extern ecs_entity_t EcsPebi;   /**< Pebi unit prefix. */
FLECS_API extern ecs_entity_t EcsExbi;   /**< Exbi unit prefix. */
FLECS_API extern ecs_entity_t EcsZebi;   /**< Zebi unit prefix. */
FLECS_API extern ecs_entity_t EcsYobi;   /**< Yobi unit prefix. */

/** @} */

/**
 * @defgroup c_addons_units_duration Duration
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsDuration;         /**< Duration quantity. */
FLECS_API extern     ecs_entity_t EcsPicoSeconds;  /**< PicoSeconds duration unit. */
FLECS_API extern     ecs_entity_t EcsNanoSeconds;  /**< NanoSeconds duration unit. */
FLECS_API extern     ecs_entity_t EcsMicroSeconds; /**< MicroSeconds duration unit. */
FLECS_API extern     ecs_entity_t EcsMilliSeconds; /**< MilliSeconds duration unit. */
FLECS_API extern     ecs_entity_t EcsSeconds;      /**< Seconds duration unit. */
FLECS_API extern     ecs_entity_t EcsMinutes;      /**< Minutes duration unit. */
FLECS_API extern     ecs_entity_t EcsHours;        /**< Hours duration unit. */
FLECS_API extern     ecs_entity_t EcsDays;         /**< Days duration unit. */

/** @} */

/**
 * @defgroup c_addons_units_time Time
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsTime;             /**< Time quantity. */
FLECS_API extern     ecs_entity_t EcsDate;         /**< Date unit. */

/** @} */

/**
 * @defgroup c_addons_units_mass Mass
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsMass;             /**< Mass quantity. */
FLECS_API extern     ecs_entity_t EcsGrams;        /**< Grams unit. */
FLECS_API extern     ecs_entity_t EcsKiloGrams;    /**< KiloGrams unit. */

/** @} */

/**
 * @defgroup c_addons_units_electric_Current Electric Current
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsElectricCurrent;  /**< ElectricCurrent quantity. */
FLECS_API extern     ecs_entity_t EcsAmpere;       /**< Ampere unit. */

/** @} */

/**
 * @defgroup c_addons_units_amount Amount
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsAmount;           /**< Amount quantity. */
FLECS_API extern     ecs_entity_t EcsMole;         /**< Mole unit. */

/** @} */

/**
 * @defgroup c_addons_units_luminous_intensity Luminous Intensity
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsLuminousIntensity; /**< LuminousIntensity quantity. */
FLECS_API extern     ecs_entity_t EcsCandela;       /**< Candela unit. */

/** @} */

/**
 * @defgroup c_addons_units_force Force
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsForce;            /**< Force quantity. */
FLECS_API extern     ecs_entity_t EcsNewton;       /**< Newton unit. */

/** @} */

/**
 * @defgroup c_addons_units_length Length
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsLength;              /**< Length quantity. */
FLECS_API extern     ecs_entity_t EcsMeters;          /**< Meters unit. */
FLECS_API extern         ecs_entity_t EcsPicoMeters;  /**< PicoMeters unit. */
FLECS_API extern         ecs_entity_t EcsNanoMeters;  /**< NanoMeters unit. */
FLECS_API extern         ecs_entity_t EcsMicroMeters; /**< MicroMeters unit. */
FLECS_API extern         ecs_entity_t EcsMilliMeters; /**< MilliMeters unit. */
FLECS_API extern         ecs_entity_t EcsCentiMeters; /**< CentiMeters unit. */
FLECS_API extern         ecs_entity_t EcsKiloMeters;  /**< KiloMeters unit. */
FLECS_API extern     ecs_entity_t EcsMiles;           /**< Miles unit. */
FLECS_API extern     ecs_entity_t EcsPixels;          /**< Pixels unit. */

/** @} */

/**
 * @defgroup c_addons_units_pressure Pressure
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsPressure;          /**< Pressure quantity. */
FLECS_API extern     ecs_entity_t EcsPascal;        /**< Pascal unit. */
FLECS_API extern     ecs_entity_t EcsBar;           /**< Bar unit. */

/** @} */

/**
 * @defgroup c_addons_units_speed Speed
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsSpeed;                   /**< Speed quantity. */
FLECS_API extern     ecs_entity_t EcsMetersPerSecond;     /**< MetersPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsKiloMetersPerSecond; /**< KiloMetersPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsKiloMetersPerHour;   /**< KiloMetersPerHour unit. */
FLECS_API extern     ecs_entity_t EcsMilesPerHour;        /**< MilesPerHour unit. */

/** @} */

/**
 * @defgroup c_addons_units_temperature Temperature
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsTemperature;       /**< Temperature quantity. */
FLECS_API extern     ecs_entity_t EcsKelvin;        /**< Kelvin unit. */
FLECS_API extern     ecs_entity_t EcsCelsius;       /**< Celsius unit. */
FLECS_API extern     ecs_entity_t EcsFahrenheit;    /**< Fahrenheit unit. */

/** @} */

/**
 * @defgroup c_addons_units_data Data
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsData;               /**< Data quantity. */
FLECS_API extern     ecs_entity_t EcsBits;           /**< Bits unit. */
FLECS_API extern         ecs_entity_t EcsKiloBits;   /**< KiloBits unit. */
FLECS_API extern         ecs_entity_t EcsMegaBits;   /**< MegaBits unit. */
FLECS_API extern         ecs_entity_t EcsGigaBits;   /**< GigaBits unit. */
FLECS_API extern     ecs_entity_t EcsBytes;          /**< Bytes unit. */
FLECS_API extern         ecs_entity_t EcsKiloBytes;  /**< KiloBytes unit. */
FLECS_API extern         ecs_entity_t EcsMegaBytes;  /**< MegaBytes unit. */
FLECS_API extern         ecs_entity_t EcsGigaBytes;  /**< GigaBytes unit. */
FLECS_API extern         ecs_entity_t EcsKibiBytes;  /**< KibiBytes unit. */
FLECS_API extern         ecs_entity_t EcsMebiBytes;  /**< MebiBytes unit. */
FLECS_API extern         ecs_entity_t EcsGibiBytes;  /**< GibiBytes unit. */

/** @} */

/**
 * @defgroup c_addons_units_datarate Data Rate
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsDataRate;               /**< DataRate quantity. */
FLECS_API extern     ecs_entity_t EcsBitsPerSecond;      /**< BitsPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsKiloBitsPerSecond;  /**< KiloBitsPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsMegaBitsPerSecond;  /**< MegaBitsPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsGigaBitsPerSecond;  /**< GigaBitsPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsBytesPerSecond;     /**< BytesPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsKiloBytesPerSecond; /**< KiloBytesPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsMegaBytesPerSecond; /**< MegaBytesPerSecond unit. */
FLECS_API extern     ecs_entity_t EcsGigaBytesPerSecond; /**< GigaBytesPerSecond unit. */

/** @} */

/**
 * @defgroup c_addons_units_duration Duration
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsAngle;            /**< Angle quantity. */
FLECS_API extern     ecs_entity_t EcsRadians;      /**< Radians unit. */
FLECS_API extern     ecs_entity_t EcsDegrees;      /**< Degrees unit. */

/** @} */

/**
 * @defgroup c_addons_units_angle Angle
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsFrequency;        /**< Frequency quantity. */
FLECS_API extern     ecs_entity_t EcsHertz;        /**< Hertz unit. */
FLECS_API extern     ecs_entity_t EcsKiloHertz;    /**< KiloHertz unit. */
FLECS_API extern     ecs_entity_t EcsMegaHertz;    /**< MegaHertz unit. */
FLECS_API extern     ecs_entity_t EcsGigaHertz;    /**< GigaHertz unit. */

/** @} */

/**
 * @defgroup c_addons_units_uri Uri
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsUri;              /**< URI quantity. */
FLECS_API extern     ecs_entity_t EcsUriHyperlink; /**< UriHyperlink unit. */
FLECS_API extern     ecs_entity_t EcsUriImage;     /**< UriImage unit. */
FLECS_API extern     ecs_entity_t EcsUriFile;      /**< UriFile unit. */

/** @} */

/**
 * @defgroup c_addons_units_color Color
 * @ingroup c_addons_units
 * @{
 */

FLECS_API extern ecs_entity_t EcsColor;            /**< Color quantity. */
FLECS_API extern     ecs_entity_t EcsColorRgb;     /**< ColorRgb unit. */
FLECS_API extern     ecs_entity_t EcsColorHsl;     /**< ColorHsl unit. */
FLECS_API extern     ecs_entity_t EcsColorCss;     /**< ColorCss unit. */

/** @} */


FLECS_API extern ecs_entity_t EcsAcceleration;     /**< Acceleration unit. */
FLECS_API extern ecs_entity_t EcsPercentage;       /**< Percentage unit. */
FLECS_API extern ecs_entity_t EcsBel;              /**< Bel unit. */
FLECS_API extern ecs_entity_t EcsDeciBel;          /**< DeciBel unit. */

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
