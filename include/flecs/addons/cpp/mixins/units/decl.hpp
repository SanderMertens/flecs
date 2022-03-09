#pragma once

namespace flecs {
struct units {

////////////////////////////////////////////////////////////////////////////////
//// Unit prefixes
////////////////////////////////////////////////////////////////////////////////

struct Prefixes { };

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


////////////////////////////////////////////////////////////////////////////////
//// Quantities
////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////
//// Units
////////////////////////////////////////////////////////////////////////////////

struct duration {
struct PicoSeconds { };
struct NanoSeconds { };
struct MicroSeconds { };
struct MilliSeconds { };
struct Seconds { };
struct Minutes { };
struct Hours { };
struct Days { };
};

struct angle {
struct Radians { };
struct Degrees { };
};

struct time {
struct Date { };
};

struct mass {
struct Grams { };
struct KiloGrams { };
};

struct electric_current {
struct Ampere { };
};

struct amount {
struct Mole { };
};

struct luminous_intensity {
struct Candela { };
};

struct force {
struct Newton { };
};

struct length {
struct Meters { };
struct PicoMeters { };
struct NanoMeters { };
struct MicroMeters { };
struct MilliMeters { };
struct CentiMeters { };
struct KiloMeters { };
struct Miles { };
};

struct pressure {
struct Pascal { };
struct Bar { };
};

struct speed {
struct MetersPerSecond { };
struct KiloMetersPerSecond { };
struct KiloMetersPerHour { };
struct MilesPerHour { };
};

struct temperature {
struct Kelvin { };
struct Celsius { };
struct Fahrenheit { };
};

struct data {
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
};

struct datarate {
struct BitsPerSecond { };
struct KiloBitsPerSecond { };
struct MegaBitsPerSecond { };
struct GigaBitsPerSecond { };
struct BytesPerSecond { };
struct KiloBytesPerSecond { };
struct MegaBytesPerSecond { };
struct GigaBytesPerSecond { };
};

struct Percentage { };
struct Bel { };
struct DeciBel { };

units(flecs::world& world);

};
}
