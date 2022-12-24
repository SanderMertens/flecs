/**
 * @file addons/cpp/mixins/units/decl.hpp
 * @brief Units module declarations.
 */

#pragma once

namespace flecs {
struct units {

/**
 * @defgroup cpp_addons_units Units
 * @brief Common unit annotations for reflection framework.
 * 
 * \ingroup cpp_addons
 * @{
 */

struct Prefixes { };

/**
 * @defgroup cpp_addons_units_prefixes Prefixes
 * @brief Prefixes to indicate unit count (e.g. Kilo, Mega)
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Yocto { };
struct Zepto { };
struct Atto { };
struct Femto { };
struct Pico { };
struct Nano { };
struct Micro { };
struct Milli { };
struct Centi { };
struct Deci { };
struct Deca { };
struct Hecto { };
struct Kilo { };
struct Mega { };
struct Giga { };
struct Tera { };
struct Peta { };
struct Exa { };
struct Zetta { };
struct Yotta { };
struct Kibi { };
struct Mebi { };
struct Gibi { };
struct Tebi { };
struct Pebi { };
struct Exbi { };
struct Zebi { };
struct Yobi { };

/** @} */

/**
 * @defgroup cpp_addons_units_quantities Quantities
 * @brief Quantities that group units (e.g. Length)
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Duration { };
struct Time { };
struct Mass { };
struct ElectricCurrent { };
struct LuminousIntensity { };
struct Force { };
struct Amount { };
struct Length { };
struct Pressure { };
struct Speed { };
struct Temperature { };
struct Data { };
struct DataRate { };
struct Angle { };
struct Frequency { };
struct Uri { };

/** @} */

struct duration {
/**
 * @defgroup cpp_addons_units_duration Duration
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct PicoSeconds { };
struct NanoSeconds { };
struct MicroSeconds { };
struct MilliSeconds { };
struct Seconds { };
struct Minutes { };
struct Hours { };
struct Days { };

/** @} */
};

struct angle {
/**
 * @defgroup cpp_addons_units_angle Angle
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Radians { };
struct Degrees { };

/** @} */
};


struct time {
/**
 * @defgroup cpp_addons_units_time Time
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Date { };

/** @} */
};


struct mass {
/**
 * @defgroup cpp_addons_units_mass Mass
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Grams { };
struct KiloGrams { };

/** @} */
};


struct electric_current {
/**
 * @defgroup cpp_addons_units_electric_current Electric Current
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Ampere { };

/** @} */
};


struct amount {
/**
 * @defgroup cpp_addons_units_amount Amount
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Mole { };

/** @} */
};


struct luminous_intensity {
/**
 * @defgroup cpp_addons_units_luminous_intensity Luminous Intensity
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Candela { };

/** @} */
};


struct force {
/**
 * @defgroup cpp_addons_units_force Force
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Newton { };

/** @} */
};


struct length {
/**
 * @defgroup cpp_addons_units_length Length
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Meters { };
struct PicoMeters { };
struct NanoMeters { };
struct MicroMeters { };
struct MilliMeters { };
struct CentiMeters { };
struct KiloMeters { };
struct Miles { };
struct Pixels { };

/** @} */
};


struct pressure {
/**
 * @defgroup cpp_addons_units_pressure Pressure
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Pascal { };
struct Bar { };

/** @} */
};


struct speed {
/**
 * @defgroup cpp_addons_units_speed Speed
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct MetersPerSecond { };
struct KiloMetersPerSecond { };
struct KiloMetersPerHour { };
struct MilesPerHour { };

/** @} */
};


struct temperature {
/**
 * @defgroup cpp_addons_units_temperature Temperature
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Kelvin { };
struct Celsius { };
struct Fahrenheit { };

/** @} */
};


struct data {
/**
 * @defgroup cpp_addons_units_data Data
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Bits { };
struct KiloBits { };
struct MegaBits { };
struct GigaBits { };
struct Bytes { };
struct KiloBytes { };
struct MegaBytes { };
struct GigaBytes { };
struct KibiBytes { };
struct MebiBytes { };
struct GibiBytes { };

/** @} */
};

struct datarate {
/**
 * @defgroup cpp_addons_units_datarate Data Rate
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct BitsPerSecond { };
struct KiloBitsPerSecond { };
struct MegaBitsPerSecond { };
struct GigaBitsPerSecond { };
struct BytesPerSecond { };
struct KiloBytesPerSecond { };
struct MegaBytesPerSecond { };
struct GigaBytesPerSecond { };

/** @} */
};


struct frequency {
/**
 * @defgroup cpp_addons_units_frequency Frequency
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Hertz { };
struct KiloHertz { };
struct MegaHertz { };
struct GigaHertz { };

/** @} */
};


struct uri {
/**
 * @defgroup cpp_addons_units_uri Uri
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Hyperlink { };
struct Image { };
struct File { };

/** @} */
};


struct Percentage { };
struct Bel { };
struct DeciBel { };

units(flecs::world& world);

/** @} */

};
}
