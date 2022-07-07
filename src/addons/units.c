
#include "../private_api.h"

#ifdef FLECS_UNITS

ECS_DECLARE(EcsUnitPrefixes);

ECS_DECLARE(EcsYocto);
ECS_DECLARE(EcsZepto);
ECS_DECLARE(EcsAtto);
ECS_DECLARE(EcsFemto);
ECS_DECLARE(EcsPico);
ECS_DECLARE(EcsNano);
ECS_DECLARE(EcsMicro);
ECS_DECLARE(EcsMilli);
ECS_DECLARE(EcsCenti);
ECS_DECLARE(EcsDeci);
ECS_DECLARE(EcsDeca);
ECS_DECLARE(EcsHecto);
ECS_DECLARE(EcsKilo);
ECS_DECLARE(EcsMega);
ECS_DECLARE(EcsGiga);
ECS_DECLARE(EcsTera);
ECS_DECLARE(EcsPeta);
ECS_DECLARE(EcsExa);
ECS_DECLARE(EcsZetta);
ECS_DECLARE(EcsYotta);

ECS_DECLARE(EcsKibi);
ECS_DECLARE(EcsMebi);
ECS_DECLARE(EcsGibi);
ECS_DECLARE(EcsTebi);
ECS_DECLARE(EcsPebi);
ECS_DECLARE(EcsExbi);
ECS_DECLARE(EcsZebi);
ECS_DECLARE(EcsYobi);

ECS_DECLARE(EcsDuration);
    ECS_DECLARE(EcsPicoSeconds);
    ECS_DECLARE(EcsNanoSeconds);
    ECS_DECLARE(EcsMicroSeconds);
    ECS_DECLARE(EcsMilliSeconds);
    ECS_DECLARE(EcsSeconds);
    ECS_DECLARE(EcsMinutes);
    ECS_DECLARE(EcsHours);
    ECS_DECLARE(EcsDays);

ECS_DECLARE(EcsTime);
    ECS_DECLARE(EcsDate);

ECS_DECLARE(EcsMass);
    ECS_DECLARE(EcsGrams);
    ECS_DECLARE(EcsKiloGrams);

ECS_DECLARE(EcsElectricCurrent);
    ECS_DECLARE(EcsAmpere);

ECS_DECLARE(EcsAmount);
    ECS_DECLARE(EcsMole);

ECS_DECLARE(EcsLuminousIntensity);
    ECS_DECLARE(EcsCandela);

ECS_DECLARE(EcsForce);
    ECS_DECLARE(EcsNewton);

ECS_DECLARE(EcsLength);
    ECS_DECLARE(EcsMeters);
        ECS_DECLARE(EcsPicoMeters);
        ECS_DECLARE(EcsNanoMeters);
        ECS_DECLARE(EcsMicroMeters);
        ECS_DECLARE(EcsMilliMeters);
        ECS_DECLARE(EcsCentiMeters);
        ECS_DECLARE(EcsKiloMeters);
    ECS_DECLARE(EcsMiles);

ECS_DECLARE(EcsPressure);
    ECS_DECLARE(EcsPascal);
    ECS_DECLARE(EcsBar);

ECS_DECLARE(EcsSpeed);
    ECS_DECLARE(EcsMetersPerSecond);
    ECS_DECLARE(EcsKiloMetersPerSecond);
    ECS_DECLARE(EcsKiloMetersPerHour);
    ECS_DECLARE(EcsMilesPerHour);

ECS_DECLARE(EcsAcceleration);

ECS_DECLARE(EcsTemperature);
    ECS_DECLARE(EcsKelvin);
    ECS_DECLARE(EcsCelsius);
    ECS_DECLARE(EcsFahrenheit);

ECS_DECLARE(EcsData);
    ECS_DECLARE(EcsBits);
        ECS_DECLARE(EcsKiloBits);
        ECS_DECLARE(EcsMegaBits);
        ECS_DECLARE(EcsGigaBits);
    ECS_DECLARE(EcsBytes);
        ECS_DECLARE(EcsKiloBytes);
        ECS_DECLARE(EcsMegaBytes);
        ECS_DECLARE(EcsGigaBytes);
        ECS_DECLARE(EcsKibiBytes);
        ECS_DECLARE(EcsGibiBytes);
        ECS_DECLARE(EcsMebiBytes);

ECS_DECLARE(EcsDataRate);
    ECS_DECLARE(EcsBitsPerSecond);
    ECS_DECLARE(EcsKiloBitsPerSecond);
    ECS_DECLARE(EcsMegaBitsPerSecond);
    ECS_DECLARE(EcsGigaBitsPerSecond);
    ECS_DECLARE(EcsBytesPerSecond);
    ECS_DECLARE(EcsKiloBytesPerSecond);
    ECS_DECLARE(EcsMegaBytesPerSecond);
    ECS_DECLARE(EcsGigaBytesPerSecond);

ECS_DECLARE(EcsPercentage);

ECS_DECLARE(EcsAngle);
    ECS_DECLARE(EcsRadians);
    ECS_DECLARE(EcsDegrees);

ECS_DECLARE(EcsBel);
ECS_DECLARE(EcsDeciBel);

void FlecsUnitsImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsUnits);

    ecs_set_name_prefix(world, "Ecs");

    EcsUnitPrefixes = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "prefixes",
        .add = { EcsModule }
    });

    /* Initialize unit prefixes */

    ecs_entity_t prev_scope = ecs_set_scope(world, EcsUnitPrefixes);

    EcsYocto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Yocto",
        .symbol = "y",
        .translation = { .factor = 10, .power = -24 }
    });
    EcsZepto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Zepto",
        .symbol = "z",
        .translation = { .factor = 10, .power = -21 }
    });
    EcsAtto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Atto",
        .symbol = "a",
        .translation = { .factor = 10, .power = -18 }
    });
    EcsFemto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Femto",
        .symbol = "a",
        .translation = { .factor = 10, .power = -15 }
    });
    EcsPico = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Pico",
        .symbol = "p",
        .translation = { .factor = 10, .power = -12 }
    });
    EcsNano = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Nano",
        .symbol = "n",
        .translation = { .factor = 10, .power = -9 }
    });
    EcsMicro = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Micro",
        .symbol = "μ",
        .translation = { .factor = 10, .power = -6 }
    });
    EcsMilli = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Milli",
        .symbol = "m",
        .translation = { .factor = 10, .power = -3 }
    });
    EcsCenti = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Centi",
        .symbol = "c",
        .translation = { .factor = 10, .power = -2 }
    });
    EcsDeci = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Deci",
        .symbol = "d",
        .translation = { .factor = 10, .power = -1 }
    });
    EcsDeca = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Deca",
        .symbol = "da",
        .translation = { .factor = 10, .power = 1 }
    });
    EcsHecto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Hecto",
        .symbol = "h",
        .translation = { .factor = 10, .power = 2 }
    });
    EcsKilo = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Kilo",
        .symbol = "k",
        .translation = { .factor = 10, .power = 3 }
    });
    EcsMega = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Mega",
        .symbol = "M",
        .translation = { .factor = 10, .power = 6 }
    });
    EcsGiga = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Giga",
        .symbol = "G",
        .translation = { .factor = 10, .power = 9 }
    });
    EcsTera = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Tera",
        .symbol = "T",
        .translation = { .factor = 10, .power = 12 }
    });
    EcsPeta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Peta",
        .symbol = "P",
        .translation = { .factor = 10, .power = 15 }
    });
    EcsExa = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Exa",
        .symbol = "E",
        .translation = { .factor = 10, .power = 18 }
    });
    EcsZetta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Zetta",
        .symbol = "Z",
        .translation = { .factor = 10, .power = 21 }
    });
    EcsYotta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Yotta",
        .symbol = "Y",
        .translation = { .factor = 10, .power = 24 }
    });

    EcsKibi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Kibi",
        .symbol = "Ki",
        .translation = { .factor = 1024, .power = 1 }
    });
    EcsMebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Mebi",
        .symbol = "Mi",
        .translation = { .factor = 1024, .power = 2 }
    });
    EcsGibi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Gibi",
        .symbol = "Gi",
        .translation = { .factor = 1024, .power = 3 }
    });
    EcsTebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Tebi",
        .symbol = "Ti",
        .translation = { .factor = 1024, .power = 4 }
    });
    EcsPebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Pebi",
        .symbol = "Pi",
        .translation = { .factor = 1024, .power = 5 }
    });
    EcsExbi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Exbi",
        .symbol = "Ei",
        .translation = { .factor = 1024, .power = 6 }
    });
    EcsZebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Zebi",
        .symbol = "Zi",
        .translation = { .factor = 1024, .power = 7 }
    });
    EcsYobi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t){
        .entity.name = "Yobi",
        .symbol = "Yi",
        .translation = { .factor = 1024, .power = 8 }
    });

    ecs_set_scope(world, prev_scope);

    /* Duration units */

    EcsDuration = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Duration" });
    prev_scope = ecs_set_scope(world, EcsDuration);

        EcsSeconds = ecs_unit_init(world, &(ecs_unit_desc_t){
            .entity.name = "Seconds",
            .quantity = EcsDuration,
            .symbol = "s" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsSeconds,
            .kind = EcsF32
        });
            EcsPicoSeconds = ecs_unit_init(world, &(ecs_unit_desc_t){
                .entity.name = "PicoSeconds",
                .quantity = EcsDuration,
                .base = EcsSeconds,
                .prefix = EcsPico });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsPicoSeconds,
                .kind = EcsF32
            });


            EcsNanoSeconds = ecs_unit_init(world, &(ecs_unit_desc_t){
                .entity.name = "NanoSeconds",
                .quantity = EcsDuration,
                .base = EcsSeconds,
                .prefix = EcsNano });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsNanoSeconds,
                .kind = EcsF32
            });

            EcsMicroSeconds = ecs_unit_init(world, &(ecs_unit_desc_t){
                .entity.name = "MicroSeconds",
                .quantity = EcsDuration,
                .base = EcsSeconds,
                .prefix = EcsMicro });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMicroSeconds,
                .kind = EcsF32
            });

            EcsMilliSeconds = ecs_unit_init(world, &(ecs_unit_desc_t){
                .entity.name = "MilliSeconds",
                .quantity = EcsDuration,
                .base = EcsSeconds,
                .prefix = EcsMilli });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMilliSeconds,
                .kind = EcsF32
            });

        EcsMinutes = ecs_unit_init(world, &(ecs_unit_desc_t){
            .entity.name = "Minutes",
            .quantity = EcsDuration,
            .base = EcsSeconds,
            .symbol = "min",
            .translation = { .factor = 60, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsMinutes,
            .kind = EcsU32
        });

        EcsHours = ecs_unit_init(world, &(ecs_unit_desc_t){
            .entity.name = "Hours",
            .quantity = EcsDuration,
            .base = EcsMinutes,
            .symbol = "h",
            .translation = { .factor = 60, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsHours,
            .kind = EcsU32
        });

        EcsDays = ecs_unit_init(world, &(ecs_unit_desc_t){
            .entity.name = "Days",
            .quantity = EcsDuration,
            .base = EcsHours,
            .symbol = "d",
            .translation = { .factor = 24, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsDays,
            .kind = EcsU32
        });
    ecs_set_scope(world, prev_scope);

    /* Time units */

    EcsTime = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Time" });
    prev_scope = ecs_set_scope(world, EcsTime);

        EcsDate = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Date",
            .quantity = EcsTime });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsDate,
            .kind = EcsU32
        });
    ecs_set_scope(world, prev_scope);

    /* Mass units */

    EcsMass = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Mass" });
    prev_scope = ecs_set_scope(world, EcsMass);
        EcsGrams = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Grams",
            .quantity = EcsMass,
            .symbol = "g" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsGrams,
            .kind = EcsF32
        });
        EcsKiloGrams = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "KiloGrams",
            .quantity = EcsMass,
            .prefix = EcsKilo,
            .base = EcsGrams });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsKiloGrams,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Electric current units */

    EcsElectricCurrent = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "ElectricCurrent" });
    prev_scope = ecs_set_scope(world, EcsElectricCurrent);
        EcsAmpere = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Ampere",
            .quantity = EcsElectricCurrent,
            .symbol = "A" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsAmpere,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Amount of substance units */

    EcsAmount = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Amount" });
    prev_scope = ecs_set_scope(world, EcsAmount);
        EcsMole = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Mole",
            .quantity = EcsAmount,
            .symbol = "mol" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsMole,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Luminous intensity units */

    EcsLuminousIntensity = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "LuminousIntensity" });
    prev_scope = ecs_set_scope(world, EcsLuminousIntensity);
        EcsCandela = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Candela",
            .quantity = EcsLuminousIntensity,
            .symbol = "cd" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsCandela,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Force units */

    EcsForce = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Force" });
    prev_scope = ecs_set_scope(world, EcsForce);
        EcsNewton = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Newton",
            .quantity = EcsForce,
            .symbol = "N" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsNewton,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Length units */

    EcsLength = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Length" });
    prev_scope = ecs_set_scope(world, EcsLength);
        EcsMeters = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Meters",
            .quantity = EcsLength,
            .symbol = "m" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsMeters,
            .kind = EcsF32
        });

            EcsPicoMeters = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "PicoMeters",
                .quantity = EcsLength,
                .base = EcsMeters,
                .prefix = EcsPico });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsPicoMeters,
                .kind = EcsF32
            });

            EcsNanoMeters = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "NanoMeters",
                .quantity = EcsLength,
                .base = EcsMeters,
                .prefix = EcsNano });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsNanoMeters,
                .kind = EcsF32
            });

            EcsMicroMeters = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "MicroMeters",
                .quantity = EcsLength,
                .base = EcsMeters,
                .prefix = EcsMicro });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMicroMeters,
                .kind = EcsF32
            });

            EcsMilliMeters = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "MilliMeters",
                .quantity = EcsLength,
                .base = EcsMeters,
                .prefix = EcsMilli });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMilliMeters,
                .kind = EcsF32
            });

            EcsCentiMeters = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "CentiMeters",
                .quantity = EcsLength,
                .base = EcsMeters,
                .prefix = EcsCenti });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsCentiMeters,
                .kind = EcsF32
            });

            EcsKiloMeters = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "KiloMeters",
                .quantity = EcsLength,
                .base = EcsMeters,
                .prefix = EcsKilo });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsKiloMeters,
                .kind = EcsF32
            });
            
        EcsMiles = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Miles",
            .quantity = EcsLength,
            .symbol = "mi"
        });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsMiles,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Pressure units */

    EcsPressure = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Pressure" });
    prev_scope = ecs_set_scope(world, EcsPressure);
        EcsPascal = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Pascal",
            .quantity = EcsPressure,
            .symbol = "Pa" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsPascal,
            .kind = EcsF32
        });
        EcsBar = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Bar",
            .quantity = EcsPressure,
            .symbol = "bar" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsBar,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Speed units */

    EcsSpeed = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Speed" });
    prev_scope = ecs_set_scope(world, EcsSpeed);
        EcsMetersPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "MetersPerSecond",
            .quantity = EcsSpeed,
            .base = EcsMeters,
            .over = EcsSeconds });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsMetersPerSecond,
            .kind = EcsF32
        });
        EcsKiloMetersPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "KiloMetersPerSecond",
            .quantity = EcsSpeed,
            .base = EcsKiloMeters,
            .over = EcsSeconds });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsKiloMetersPerSecond,
            .kind = EcsF32
        });
        EcsKiloMetersPerHour = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "KiloMetersPerHour",
            .quantity = EcsSpeed,
            .base = EcsKiloMeters,
            .over = EcsHours });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsKiloMetersPerHour,
            .kind = EcsF32
        });
        EcsMilesPerHour = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "MilesPerHour",
            .quantity = EcsSpeed,
            .base = EcsMiles,
            .over = EcsHours });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsMilesPerHour,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);
    
    /* Acceleration */

    EcsAcceleration = ecs_unit_init(world, &(ecs_unit_desc_t){ 
        .entity.name = "Acceleration",
        .base = EcsMetersPerSecond,
        .over = EcsSeconds });
    ecs_quantity_init(world, &(ecs_entity_desc_t){
        .entity = EcsAcceleration
    });
    ecs_primitive_init(world, &(ecs_primitive_desc_t){
        .entity.entity = EcsAcceleration,
        .kind = EcsF32
    });

    /* Temperature units */

    EcsTemperature = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Temperature" });
    prev_scope = ecs_set_scope(world, EcsTemperature);
        EcsKelvin = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Kelvin",
            .quantity = EcsTemperature,
            .symbol = "K" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsKelvin,
            .kind = EcsF32
        });
        EcsCelsius = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Celsius",
            .quantity = EcsTemperature,
            .symbol = "°C" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsCelsius,
            .kind = EcsF32
        });
        EcsFahrenheit = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Fahrenheit",
            .quantity = EcsTemperature,
            .symbol = "F" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsFahrenheit,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Data units */

    EcsData = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Data" });
    prev_scope = ecs_set_scope(world, EcsData);

        EcsBits = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Bits",
            .quantity = EcsData,
            .symbol = "bit" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsBits,
            .kind = EcsU64
        });

            EcsKiloBits = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "KiloBits",
                .quantity = EcsData,
                .base = EcsBits,
                .prefix = EcsKilo });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsKiloBits,
                .kind = EcsU64
            });

            EcsMegaBits = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "MegaBits",
                .quantity = EcsData,
                .base = EcsBits,
                .prefix = EcsMega });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMegaBits,
                .kind = EcsU64
            });

            EcsGigaBits = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "GigaBits",
                .quantity = EcsData,
                .base = EcsBits,
                .prefix = EcsGiga });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsGigaBits,
                .kind = EcsU64
            });

        EcsBytes = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Bytes",
            .quantity = EcsData,
            .symbol = "B",
            .base = EcsBits,
            .translation = { .factor = 8, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsBytes,
            .kind = EcsU64
        });

            EcsKiloBytes = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "KiloBytes",
                .quantity = EcsData,
                .base = EcsBytes,
                .prefix = EcsKilo });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsKiloBytes,
                .kind = EcsU64
            });

            EcsMegaBytes = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "MegaBytes",
                .quantity = EcsData,
                .base = EcsBytes,
                .prefix = EcsMega });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMegaBytes,
                .kind = EcsU64
            });

            EcsGigaBytes = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "GigaBytes",
                .quantity = EcsData,
                .base = EcsBytes,
                .prefix = EcsGiga });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsGigaBytes,
                .kind = EcsU64
            });

            EcsKibiBytes = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "KibiBytes",
                .quantity = EcsData,
                .base = EcsBytes,
                .prefix = EcsKibi });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsKibiBytes,
                .kind = EcsU64
            });

            EcsMebiBytes = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "MebiBytes",
                .quantity = EcsData,
                .base = EcsBytes,
                .prefix = EcsMebi });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMebiBytes,
                .kind = EcsU64
            });

            EcsGibiBytes = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "GibiBytes",
                .quantity = EcsData,
                .base = EcsBytes,
                .prefix = EcsGibi });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsGibiBytes,
                .kind = EcsU64
            });

    ecs_set_scope(world, prev_scope);

    /* DataRate units */

    EcsDataRate = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "DataRate" });
    prev_scope = ecs_set_scope(world, EcsDataRate);

        EcsBitsPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "BitsPerSecond",
            .quantity = EcsDataRate,
            .base = EcsBits,
            .over = EcsSeconds });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsBitsPerSecond,
            .kind = EcsU64
        });

            EcsKiloBitsPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "KiloBitsPerSecond",
                .quantity = EcsDataRate,
                .base = EcsKiloBits,
                .over = EcsSeconds
            });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsKiloBitsPerSecond,
                .kind = EcsU64
            });

            EcsMegaBitsPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "MegaBitsPerSecond",
                .quantity = EcsDataRate,
                .base = EcsMegaBits,
                .over = EcsSeconds
            });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMegaBitsPerSecond,
                .kind = EcsU64
            });

            EcsGigaBitsPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "GigaBitsPerSecond",
                .quantity = EcsDataRate,
                .base = EcsGigaBits,
                .over = EcsSeconds
            });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsGigaBitsPerSecond,
                .kind = EcsU64
            });

        EcsBytesPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "BytesPerSecond",
            .quantity = EcsDataRate,
            .base = EcsBytes,
            .over = EcsSeconds });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsBytesPerSecond,
            .kind = EcsU64
        });

            EcsKiloBytesPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "KiloBytesPerSecond",
                .quantity = EcsDataRate,
                .base = EcsKiloBytes,
                .over = EcsSeconds
            });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsKiloBytesPerSecond,
                .kind = EcsU64
            });

            EcsMegaBytesPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "MegaBytesPerSecond",
                .quantity = EcsDataRate,
                .base = EcsMegaBytes,
                .over = EcsSeconds
            });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsMegaBytesPerSecond,
                .kind = EcsU64
            });

            EcsGigaBytesPerSecond = ecs_unit_init(world, &(ecs_unit_desc_t){ 
                .entity.name = "GigaBytesPerSecond",
                .quantity = EcsDataRate,
                .base = EcsGigaBytes,
                .over = EcsSeconds
            });
            ecs_primitive_init(world, &(ecs_primitive_desc_t){
                .entity.entity = EcsGigaBytesPerSecond,
                .kind = EcsU64
            });

        ecs_set_scope(world, prev_scope);

    /* Percentage */

    EcsPercentage = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Percentage" });
    ecs_unit_init(world, &(ecs_unit_desc_t){ 
        .entity.entity = EcsPercentage,
        .symbol = "%"
    });
    ecs_primitive_init(world, &(ecs_primitive_desc_t){
        .entity.entity = EcsPercentage,
        .kind = EcsF32
    });

    /* Angles */

    EcsAngle = ecs_quantity_init(world, &(ecs_entity_desc_t){ 
        .name = "Angle" });
    prev_scope = ecs_set_scope(world, EcsAngle);
        EcsRadians = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Radians",
            .quantity = EcsAngle,
            .symbol = "rad" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsRadians,
            .kind = EcsF32
        });

        EcsDegrees = ecs_unit_init(world, &(ecs_unit_desc_t){ 
            .entity.name = "Degrees",
            .quantity = EcsAngle,
            .symbol = "°" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity.entity = EcsDegrees,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* DeciBel */

    EcsBel = ecs_unit_init(world, &(ecs_unit_desc_t){ 
        .entity.name = "Bel",
        .symbol = "B" });
    ecs_primitive_init(world, &(ecs_primitive_desc_t){
        .entity.entity = EcsBel,
        .kind = EcsF32
    });
    EcsDeciBel = ecs_unit_init(world, &(ecs_unit_desc_t){ 
        .entity.name = "DeciBel",
        .prefix = EcsDeci,
        .base = EcsBel });
    ecs_primitive_init(world, &(ecs_primitive_desc_t){
        .entity.entity = EcsDeciBel,
        .kind = EcsF32
    });

    /* Documentation */
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);

    ecs_doc_set_brief(world, EcsDuration, 
        "Time amount (e.g. \"20 seconds\", \"2 hours\")");
    ecs_doc_set_brief(world, EcsSeconds, "Time amount in seconds");
    ecs_doc_set_brief(world, EcsMinutes, "60 seconds");
    ecs_doc_set_brief(world, EcsHours, "60 minutes");
    ecs_doc_set_brief(world, EcsDays, "24 hours");

    ecs_doc_set_brief(world, EcsTime,
        "Time passed since an epoch (e.g. \"5pm\", \"March 3rd 2022\")");
    ecs_doc_set_brief(world, EcsDate,
        "Seconds passed since January 1st 1970");

    ecs_doc_set_brief(world, EcsMass, "Units of mass (e.g. \"5 kilograms\")");

    ecs_doc_set_brief(world, EcsElectricCurrent,
        "Units of electrical current (e.g. \"2 ampere\")");

    ecs_doc_set_brief(world, EcsAmount,
        "Units of amount of substance (e.g. \"2 mole\")");

    ecs_doc_set_brief(world, EcsLuminousIntensity,
        "Units of luminous intensity (e.g. \"1 candela\")");

    ecs_doc_set_brief(world, EcsForce, "Units of force (e.g. \"10 newton\")");

    ecs_doc_set_brief(world, EcsLength,
        "Units of length (e.g. \"5 meters\", \"20 miles\")");

    ecs_doc_set_brief(world, EcsPressure, 
        "Units of pressure (e.g. \"1 bar\", \"1000 pascal\")");

    ecs_doc_set_brief(world, EcsSpeed,
        "Units of movement (e.g. \"5 meters/second\")");

    ecs_doc_set_brief(world, EcsAcceleration,
        "Unit of speed increase (e.g. \"5 meters/second/second\")");

    ecs_doc_set_brief(world, EcsTemperature,
        "Units of temperature (e.g. \"5 degrees Celsius\")");

    ecs_doc_set_brief(world, EcsData,
        "Units of information (e.g. \"8 bits\", \"100 megabytes\")");

    ecs_doc_set_brief(world, EcsDataRate,
        "Units of data transmission (e.g. \"100 megabits/second\")");

    ecs_doc_set_brief(world, EcsAngle,
        "Units of rotation (e.g. \"1.2 radians\", \"180 degrees\")");

#endif
}

#endif
