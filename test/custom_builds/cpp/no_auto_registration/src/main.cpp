#include <no_auto_registration.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world world;

    world.import<flecs::units>();
    world.import<flecs::alerts>();
    world.import<flecs::stats>();

    // check if builtin components are registered

    // core
    world.id<flecs::Component>();
    world.id<flecs::Identifier>();
    world.id<flecs::Poly>();

    // alerts
    world.id<flecs::alerts::AlertsActive>();
    world.id<flecs::alerts::Instance>();
    world.id<flecs::alerts::Alert>();
    world.id<flecs::alerts::Info>();
    world.id<flecs::alerts::Warning>();
    world.id<flecs::alerts::Error>();

    // metrics
    world.id<flecs::metrics::Value>();
    world.id<flecs::metrics::Source>();
    world.id<flecs::metrics::Instance>();
    world.id<flecs::metrics::Metric>();
    world.id<flecs::metrics::Counter>();
    world.id<flecs::metrics::CounterIncrement>();
    world.id<flecs::metrics::CounterId>();
    world.id<flecs::metrics::Gauge>();

    // doc
    world.id<flecs::doc::Description>();

    // monitor
    world.id<flecs::WorldSummary>();
    world.id<flecs::WorldStats>();
    world.id<flecs::PipelineStats>();

    // meta
    world.id<flecs::bool_t>();
    world.id<flecs::char_t>();
    world.id<flecs::u8_t>();
    world.id<flecs::u16_t>();
    world.id<flecs::u32_t>();
    world.id<flecs::u64_t>();
    world.id<flecs::uptr_t>();
    world.id<flecs::i8_t>();
    world.id<flecs::i16_t>();
    world.id<flecs::i32_t>();
    world.id<flecs::i64_t>();
    world.id<flecs::iptr_t>();
    world.id<flecs::f32_t>();
    world.id<flecs::f64_t>();

    world.id<flecs::member_t>();
    world.id<flecs::enum_constant_t>();
    world.id<flecs::bitmask_constant_t>();

    world.id<flecs::Type>();
    world.id<flecs::TypeSerializer>();
    world.id<flecs::Primitive>();
    world.id<flecs::Enum>();
    world.id<flecs::Bitmask>();
    world.id<flecs::Member>();
    world.id<flecs::MemberRanges>();
    world.id<flecs::Struct>();
    world.id<flecs::Array>();
    world.id<flecs::Vector>();
    world.id<flecs::Unit>();

    // rest
    world.id<flecs::Rest>();

    // timer
    world.id<flecs::Timer>();
    world.id<flecs::RateFilter>();

    // units
    world.id<flecs::units::Yocto>();
    world.id<flecs::units::Zepto>();
    world.id<flecs::units::Atto>();
    world.id<flecs::units::Femto>();
    world.id<flecs::units::Pico>();
    world.id<flecs::units::Nano>();
    world.id<flecs::units::Micro>();
    world.id<flecs::units::Milli>();
    world.id<flecs::units::Centi>();
    world.id<flecs::units::Deci>();
    world.id<flecs::units::Deca>();
    world.id<flecs::units::Hecto>();
    world.id<flecs::units::Kilo>();
    world.id<flecs::units::Mega>();
    world.id<flecs::units::Giga>();
    world.id<flecs::units::Tera>();
    world.id<flecs::units::Peta>();
    world.id<flecs::units::Exa>();
    world.id<flecs::units::Zetta>();
    world.id<flecs::units::Yotta>();
    world.id<flecs::units::Kibi>();
    world.id<flecs::units::Mebi>();
    world.id<flecs::units::Gibi>();
    world.id<flecs::units::Tebi>();
    world.id<flecs::units::Pebi>();
    world.id<flecs::units::Exbi>();
    world.id<flecs::units::Zebi>();
    world.id<flecs::units::Yobi>();

    world.id<flecs::units::Duration>();
    world.id<flecs::units::Time>();
    world.id<flecs::units::Mass>();
    world.id<flecs::units::ElectricCurrent>();
    world.id<flecs::units::LuminousIntensity>();
    world.id<flecs::units::Force>();
    world.id<flecs::units::Amount>();
    world.id<flecs::units::Length>();
    world.id<flecs::units::Pressure>();
    world.id<flecs::units::Speed>();
    world.id<flecs::units::Temperature>();
    world.id<flecs::units::Data>();
    world.id<flecs::units::DataRate>();
    world.id<flecs::units::Angle>();
    world.id<flecs::units::Frequency>();
    world.id<flecs::units::Uri>();

    world.id<flecs::units::duration::PicoSeconds>();
    world.id<flecs::units::duration::NanoSeconds>();
    world.id<flecs::units::duration::MicroSeconds>();
    world.id<flecs::units::duration::MilliSeconds>();
    world.id<flecs::units::duration::Seconds>();
    world.id<flecs::units::duration::Minutes>();
    world.id<flecs::units::duration::Hours>();
    world.id<flecs::units::duration::Days>();

    world.id<flecs::units::angle::Radians>();
    world.id<flecs::units::angle::Degrees>();

    world.id<flecs::units::time::Date>();

    world.id<flecs::units::mass::Grams>();
    world.id<flecs::units::mass::KiloGrams>();

    world.id<flecs::units::electric_current::Ampere>();

    world.id<flecs::units::amount::Mole>();

    world.id<flecs::units::luminous_intensity::Candela>();

    world.id<flecs::units::force::Newton>();

    world.id<flecs::units::length::Meters>();
    world.id<flecs::units::length::PicoMeters>();
    world.id<flecs::units::length::NanoMeters>();
    world.id<flecs::units::length::MicroMeters>();
    world.id<flecs::units::length::MilliMeters>();
    world.id<flecs::units::length::CentiMeters>();
    world.id<flecs::units::length::KiloMeters>();
    world.id<flecs::units::length::Miles>();
    world.id<flecs::units::length::Pixels>();

    world.id<flecs::units::pressure::Pascal>();
    world.id<flecs::units::pressure::Bar>();

    world.id<flecs::units::speed::MetersPerSecond>();
    world.id<flecs::units::speed::KiloMetersPerSecond>();
    world.id<flecs::units::speed::KiloMetersPerHour>();
    world.id<flecs::units::speed::MilesPerHour>();

    world.id<flecs::units::temperature::Kelvin>();
    world.id<flecs::units::temperature::Celsius>();
    world.id<flecs::units::temperature::Fahrenheit>();

    world.id<flecs::units::data::Bits>();
    world.id<flecs::units::data::KiloBits>();
    world.id<flecs::units::data::MegaBits>();
    world.id<flecs::units::data::GigaBits>();
    world.id<flecs::units::data::Bytes>();
    world.id<flecs::units::data::KiloBytes>();
    world.id<flecs::units::data::MegaBytes>();
    world.id<flecs::units::data::GigaBytes>();
    world.id<flecs::units::data::KibiBytes>();
    world.id<flecs::units::data::MebiBytes>();
    world.id<flecs::units::data::GibiBytes>();

    world.id<flecs::units::datarate::BitsPerSecond>();
    world.id<flecs::units::datarate::KiloBitsPerSecond>();
    world.id<flecs::units::datarate::MegaBitsPerSecond>();
    world.id<flecs::units::datarate::GigaBitsPerSecond>();
    world.id<flecs::units::datarate::BytesPerSecond>();
    world.id<flecs::units::datarate::KiloBytesPerSecond>();
    world.id<flecs::units::datarate::MegaBytesPerSecond>();
    world.id<flecs::units::datarate::GigaBytesPerSecond>();

    world.id<flecs::units::frequency::Hertz>();
    world.id<flecs::units::frequency::KiloHertz>();
    world.id<flecs::units::frequency::MegaHertz>();
    world.id<flecs::units::frequency::GigaHertz>();

    world.id<flecs::units::uri::Hyperlink>();
    world.id<flecs::units::uri::Image>();
    world.id<flecs::units::uri::File>();

    world.id<flecs::units::Percentage>();
    world.id<flecs::units::Bel>();
    world.id<flecs::units::DeciBel>();

    return 0;
}
