/**
 * @file addons/cpp/mixins/units/decl.hpp
 * @brief Units module declarations.
 */

#pragma once

namespace flecs {
/** Units module. */
struct units {

/**
 * @defgroup cpp_addons_units Units
 * @ingroup cpp_addons
 * Common unit annotations for reflection framework.
 *
 * @{
 */

/** Prefixes scope. */
struct Prefixes { };

/**
 * @defgroup cpp_addons_units_prefixes Prefixes
 * @ingroup cpp_addons_units
 * Prefixes to indicate unit count (e.g. Kilo, Mega)
 *
 * @{
 */

/** Yocto prefix (10^-24). */
struct Yocto { };
/** Zepto prefix (10^-21). */
struct Zepto { };
/** Atto prefix (10^-18). */
struct Atto { };
/** Femto prefix (10^-15). */
struct Femto { };
/** Pico prefix (10^-12). */
struct Pico { };
/** Nano prefix (10^-9). */
struct Nano { };
/** Micro prefix (10^-6). */
struct Micro { };
/** Milli prefix (10^-3). */
struct Milli { };
/** Centi prefix (10^-2). */
struct Centi { };
/** Deci prefix (10^-1). */
struct Deci { };
/** Deca prefix (10^1). */
struct Deca { };
/** Hecto prefix (10^2). */
struct Hecto { };
/** Kilo prefix (10^3). */
struct Kilo { };
/** Mega prefix (10^6). */
struct Mega { };
/** Giga prefix (10^9). */
struct Giga { };
/** Tera prefix (10^12). */
struct Tera { };
/** Peta prefix (10^15). */
struct Peta { };
/** Exa prefix (10^18). */
struct Exa { };
/** Zetta prefix (10^21). */
struct Zetta { };
/** Yotta prefix (10^24). */
struct Yotta { };
/** Kibi prefix (2^10). */
struct Kibi { };
/** Mebi prefix (2^20). */
struct Mebi { };
/** Gibi prefix (2^30). */
struct Gibi { };
/** Tebi prefix (2^40). */
struct Tebi { };
/** Pebi prefix (2^50). */
struct Pebi { };
/** Exbi prefix (2^60). */
struct Exbi { };
/** Zebi prefix (2^70). */
struct Zebi { };
/** Yobi prefix (2^80). */
struct Yobi { };

/** @} */

/**
 * @defgroup cpp_addons_units_quantities Quantities
 * @ingroup cpp_addons_units
 * Quantities that group units (e.g. Length)
 *
 * @{
 */

/** Duration quantity. */
struct Duration { };
/** Time quantity. */
struct Time { };
/** Mass quantity. */
struct Mass { };
/** Electric current quantity. */
struct ElectricCurrent { };
/** Luminous intensity quantity. */
struct LuminousIntensity { };
/** Force quantity. */
struct Force { };
/** Amount quantity. */
struct Amount { };
/** Length quantity. */
struct Length { };
/** Pressure quantity. */
struct Pressure { };
/** Speed quantity. */
struct Speed { };
/** Temperature quantity. */
struct Temperature { };
/** Data quantity. */
struct Data { };
/** Data rate quantity. */
struct DataRate { };
/** Angle quantity. */
struct Angle { };
/** Frequency quantity. */
struct Frequency { };
/** URI quantity. */
struct Uri { };
/** Color quantity. */
struct Color { };

/** @} */

/** Duration units. */
struct duration {
/**
 * @defgroup cpp_addons_units_duration Duration
 * @ingroup cpp_addons_units
 * @{
 */

/** PicoSeconds unit. */
struct PicoSeconds { };
/** NanoSeconds unit. */
struct NanoSeconds { };
/** MicroSeconds unit. */
struct MicroSeconds { };
/** MilliSeconds unit. */
struct MilliSeconds { };
/** Seconds unit. */
struct Seconds { };
/** Minutes unit. */
struct Minutes { };
/** Hours unit. */
struct Hours { };
/** Days unit. */
struct Days { };

/** @} */
};

/** Angle units. */
struct angle {
/**
 * @defgroup cpp_addons_units_angle Angle
 * @ingroup cpp_addons_units
 * @{
 */

/** Radians unit. */
struct Radians { };
/** Degrees unit. */
struct Degrees { };

/** @} */
};


/** Time units. */
struct time {
/**
 * @defgroup cpp_addons_units_time Time
 * @ingroup cpp_addons_units
 * @{
 */

/** Date unit. */
struct Date { };

/** @} */
};


/** Mass units. */
struct mass {
/**
 * @defgroup cpp_addons_units_mass Mass
 * @ingroup cpp_addons_units
 * @{
 */

/** Grams unit. */
struct Grams { };
/** KiloGrams unit. */
struct KiloGrams { };

/** @} */
};


/** Electric current units. */
struct electric_current {
/**
 * @defgroup cpp_addons_units_electric_current Electric Current
 * @ingroup cpp_addons_units
 * @{
 */

/** Ampere unit. */
struct Ampere { };

/** @} */
};


/** Amount units. */
struct amount {
/**
 * @defgroup cpp_addons_units_amount Amount
 * @ingroup cpp_addons_units
 * @{
 */

/** Mole unit. */
struct Mole { };

/** @} */
};


/** Luminous intensity units. */
struct luminous_intensity {
/**
 * @defgroup cpp_addons_units_luminous_intensity Luminous Intensity
 * @ingroup cpp_addons_units
 * @{
 */

/** Candela unit. */
struct Candela { };

/** @} */
};


/** Force units. */
struct force {
/**
 * @defgroup cpp_addons_units_force Force
 * @ingroup cpp_addons_units
 * @{
 */

/** Newton unit. */
struct Newton { };

/** @} */
};


/** Length units. */
struct length {
/**
 * @defgroup cpp_addons_units_length Length
 * @ingroup cpp_addons_units
 * @{
 */

/** Meters unit. */
struct Meters { };
/** PicoMeters unit. */
struct PicoMeters { };
/** NanoMeters unit. */
struct NanoMeters { };
/** MicroMeters unit. */
struct MicroMeters { };
/** MilliMeters unit. */
struct MilliMeters { };
/** CentiMeters unit. */
struct CentiMeters { };
/** KiloMeters unit. */
struct KiloMeters { };
/** Miles unit. */
struct Miles { };
/** Pixels unit. */
struct Pixels { };

/** @} */
};


/** Pressure units. */
struct pressure {
/**
 * @defgroup cpp_addons_units_pressure Pressure
 * @ingroup cpp_addons_units
 * @{
 */

/** Pascal unit. */
struct Pascal { };
/** Bar unit. */
struct Bar { };

/** @} */
};


/** Speed units. */
struct speed {
/**
 * @defgroup cpp_addons_units_speed Speed
 * @ingroup cpp_addons_units
 * @{
 */

/** MetersPerSecond unit. */
struct MetersPerSecond { };
/** KiloMetersPerSecond unit. */
struct KiloMetersPerSecond { };
/** KiloMetersPerHour unit. */
struct KiloMetersPerHour { };
/** MilesPerHour unit. */
struct MilesPerHour { };

/** @} */
};


/** Temperature units. */
struct temperature {
/**
 * @defgroup cpp_addons_units_temperature Temperature
 * @ingroup cpp_addons_units
 * @{
 */

/** Kelvin unit. */
struct Kelvin { };
/** Celsius unit. */
struct Celsius { };
/** Fahrenheit unit. */
struct Fahrenheit { };

/** @} */
};


/** Data units. */
struct data {
/**
 * @defgroup cpp_addons_units_data Data
 * @ingroup cpp_addons_units
 * @{
 */

/** Bits unit. */
struct Bits { };
/** KiloBits unit. */
struct KiloBits { };
/** MegaBits unit. */
struct MegaBits { };
/** GigaBits unit. */
struct GigaBits { };
/** Bytes unit. */
struct Bytes { };
/** KiloBytes unit. */
struct KiloBytes { };
/** MegaBytes unit. */
struct MegaBytes { };
/** GigaBytes unit. */
struct GigaBytes { };
/** KibiBytes unit. */
struct KibiBytes { };
/** MebiBytes unit. */
struct MebiBytes { };
/** GibiBytes unit. */
struct GibiBytes { };

/** @} */
};

/** Data rate units. */
struct datarate {
/**
 * @defgroup cpp_addons_units_datarate Data Rate
 * @ingroup cpp_addons_units
 * @{
 */

/** BitsPerSecond unit. */
struct BitsPerSecond { };
/** KiloBitsPerSecond unit. */
struct KiloBitsPerSecond { };
/** MegaBitsPerSecond unit. */
struct MegaBitsPerSecond { };
/** GigaBitsPerSecond unit. */
struct GigaBitsPerSecond { };
/** BytesPerSecond unit. */
struct BytesPerSecond { };
/** KiloBytesPerSecond unit. */
struct KiloBytesPerSecond { };
/** MegaBytesPerSecond unit. */
struct MegaBytesPerSecond { };
/** GigaBytesPerSecond unit. */
struct GigaBytesPerSecond { };

/** @} */
};


/** Frequency units. */
struct frequency {
/**
 * @defgroup cpp_addons_units_frequency Frequency
 * @ingroup cpp_addons_units
 * @{
 */

/** Hertz unit. */
struct Hertz { };
/** KiloHertz unit. */
struct KiloHertz { };
/** MegaHertz unit. */
struct MegaHertz { };
/** GigaHertz unit. */
struct GigaHertz { };

/** @} */
};


/** URI units. */
struct uri {
/**
 * @defgroup cpp_addons_units_uri Uri
 * @ingroup cpp_addons_units
 * @{
 */

/** Hyperlink unit. */
struct Hyperlink { };
/** Image unit. */
struct Image { };
/** File unit. */
struct File { };

/** @} */
};


/** Color units. */
struct color {
/**
 * @defgroup cpp_addons_units_color Color
 * @ingroup cpp_addons_units
 * @{
 */

/** RGB color unit. */
struct Rgb { };
/** HSL color unit. */
struct Hsl { };
/** CSS color unit. */
struct Css { };

/** @} */
};

/** Percentage unit. */
struct Percentage { };
/** Bel unit. */
struct Bel { };
/** DeciBel unit. */
struct DeciBel { };

/** Construct the units module. */
units(flecs::world& world);

/** @} */

};
}
