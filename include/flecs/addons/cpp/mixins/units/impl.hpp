#pragma once

namespace flecs {

inline units::units(flecs::world& world) {
    FlecsUnitsImport(world);
    
    world.module<units>();


    // Initialize world.entity(prefixes) scope
    world.entity<Prefixes>("::flecs::units::prefixes");

    // Initialize prefixes
    world.entity<Yocto>("::flecs::units::prefixes::Yocto");
    world.entity<Zepto>("::flecs::units::prefixes::Zepto");
    world.entity<Atto>("::flecs::units::prefixes::Atto");
    world.entity<Femto>("::flecs::units::prefixes::Femto");
    world.entity<Pico>("::flecs::units::prefixes::Pico");
    world.entity<Nano>("::flecs::units::prefixes::Nano");
    world.entity<Micro>("::flecs::units::prefixes::Micro");
    world.entity<Milli>("::flecs::units::prefixes::Milli");
    world.entity<Centi>("::flecs::units::prefixes::Centi");
    world.entity<Deci>("::flecs::units::prefixes::Deci");
    world.entity<Deca>("::flecs::units::prefixes::Deca");
    world.entity<Hecto>("::flecs::units::prefixes::Hecto");
    world.entity<Kilo>("::flecs::units::prefixes::Kilo");
    world.entity<Mega>("::flecs::units::prefixes::Mega");
    world.entity<Giga>("::flecs::units::prefixes::Giga");
    world.entity<Tera>("::flecs::units::prefixes::Tera");
    world.entity<Peta>("::flecs::units::prefixes::Peta");
    world.entity<Exa>("::flecs::units::prefixes::Exa");
    world.entity<Zetta>("::flecs::units::prefixes::Zetta");
    world.entity<Yotta>("::flecs::units::prefixes::Yotta");
    world.entity<Kibi>("::flecs::units::prefixes::Kibi");
    world.entity<Mebi>("::flecs::units::prefixes::Mebi");
    world.entity<Gibi>("::flecs::units::prefixes::Gibi");
    world.entity<Tebi>("::flecs::units::prefixes::Tebi");
    world.entity<Pebi>("::flecs::units::prefixes::Pebi");
    world.entity<Exbi>("::flecs::units::prefixes::Exbi");
    world.entity<Zebi>("::flecs::units::prefixes::Zebi");
    world.entity<Yobi>("::flecs::units::prefixes::Yobi");

    // Initialize quantities
    world.entity<Duration>("::flecs::units::Duration");
    world.entity<Time>("::flecs::units::Time");
    world.entity<Length>("::flecs::units::Length");
    world.entity<Data>("::flecs::units::Data");
    world.entity<DataRate>("::flecs::units::DataRate");

    // Initialize duration units
    world.entity<duration::PicoSeconds>(
        "::flecs::units::Duration::PicoSeconds");
    world.entity<duration::NanoSeconds>(
        "::flecs::units::Duration::NanoSeconds");
    world.entity<duration::MicroSeconds>(
        "::flecs::units::Duration::MicroSeconds");
    world.entity<duration::MilliSeconds>(
        "::flecs::units::Duration::MilliSeconds");
    world.entity<duration::Seconds>(
        "::flecs::units::Duration::Seconds");
    world.entity<duration::Minutes>(
        "::flecs::units::Duration::Minutes");
    world.entity<duration::Hours>(
        "::flecs::units::Duration::Hours");
    world.entity<duration::Days>(
        "::flecs::units::Duration::Days");

    // Initialize time units
    world.entity<time::Date>("::flecs::units::Time::Date");

    // Initialize length units
    world.entity<length::Meters>("::flecs::units::Length::Meters");
    world.entity<length::PicoMeters>("::flecs::units::Length::PicoMeters");
    world.entity<length::NanoMeters>("::flecs::units::Length::NanoMeters");
    world.entity<length::MicroMeters>("::flecs::units::Length::MicroMeters");
    world.entity<length::MilliMeters>("::flecs::units::Length::MilliMeters");
    world.entity<length::CentiMeters>("::flecs::units::Length::CentiMeters");
    world.entity<length::KiloMeters>("::flecs::units::Length::KiloMeters");
    world.entity<length::Miles>("::flecs::units::Length::Miles");

    // Initialize data units
    world.entity<data::Bits>(
        "::flecs::units::Data::Bits");
    world.entity<data::KiloBits>(
        "::flecs::units::Data::KiloBits");
    world.entity<data::MegaBits>(
        "::flecs::units::Data::MegaBits");
    world.entity<data::GigaBits>(
        "::flecs::units::Data::GigaBits");
    world.entity<data::Bytes>(
        "::flecs::units::Data::Bytes");
    world.entity<data::KiloBytes>(
        "::flecs::units::Data::KiloBytes");
    world.entity<data::MegaBytes>(
        "::flecs::units::Data::MegaBytes");
    world.entity<data::GigaBytes>(
        "::flecs::units::Data::GigaBytes");
    world.entity<data::KibiBytes>(
        "::flecs::units::Data::KibiBytes");
    world.entity<data::MebiBytes>(
        "::flecs::units::Data::MebiBytes");
    world.entity<data::GibiBytes>(
        "::flecs::units::Data::GibiBytes");

    // Initialize datarate units
    world.entity<datarate::BitsPerSecond>(
        "::flecs::units::DataRate::BitsPerSecond");
    world.entity<datarate::KiloBitsPerSecond>(
        "::flecs::units::DataRate::KiloBitsPerSecond");
    world.entity<datarate::MegaBitsPerSecond>(
        "::flecs::units::DataRate::MegaBitsPerSecond");
    world.entity<datarate::GigaBitsPerSecond>(
        "::flecs::units::DataRate::GigaBitsPerSecond");
    world.entity<datarate::BytesPerSecond>(
        "::flecs::units::DataRate::BytesPerSecond");
    world.entity<datarate::KiloBytesPerSecond>(
        "::flecs::units::DataRate::KiloBytesPerSecond");
    world.entity<datarate::MegaBytesPerSecond>(
        "::flecs::units::DataRate::MegaBytesPerSecond");
    world.entity<datarate::GigaBytesPerSecond>(
        "::flecs::units::DataRate::GigaBytesPerSecond");

    // Initialize percentage units
    world.entity<Percentage>("::flecs::units::Percentage");
}

}
