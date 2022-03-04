
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

ECS_DECLARE(EcsPercentage);

ECS_DECLARE(EcsLength);
    ECS_DECLARE(EcsMeters);
        ECS_DECLARE(EcsPicoMeters);
        ECS_DECLARE(EcsNanoMeters);
        ECS_DECLARE(EcsMicroMeters);
        ECS_DECLARE(EcsMilliMeters);
        ECS_DECLARE(EcsCentiMeters);
        ECS_DECLARE(EcsKiloMeters);
    ECS_DECLARE(EcsMiles);

ECS_DECLARE(EcsInformation);
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
        ECS_DECLARE(EcsMibiBytes);

void FlecsUnitsImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsUnits);

    ecs_set_name_prefix(world, "Ecs");

    EcsUnitPrefixes = ecs_entity_init(world, &(ecs_entity_desc_t) {
        .name = "prefixes",
        .add = { EcsModule }
    });

    /* Initialize unit prefixes */

    ecs_entity_t prev_scope = ecs_set_scope(world, EcsUnitPrefixes);

    EcsYocto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Yocto",
        .symbol = "y",
        .translation = { .factor = 10, .power = -24 }
    });
    EcsZepto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Zepto",
        .symbol = "z",
        .translation = { .factor = 10, .power = -21 }
    });
    EcsAtto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Atto",
        .symbol = "a",
        .translation = { .factor = 10, .power = -18 }
    });
    EcsFemto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Femto",
        .symbol = "a",
        .translation = { .factor = 10, .power = -15 }
    });
    EcsPico = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Pico",
        .symbol = "p",
        .translation = { .factor = 10, .power = -12 }
    });
    EcsNano = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Nano",
        .symbol = "n",
        .translation = { .factor = 10, .power = -9 }
    });
    EcsMicro = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Micro",
        .symbol = "μ",
        .translation = { .factor = 10, .power = -6 }
    });
    EcsMilli = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Milli",
        .symbol = "m",
        .translation = { .factor = 10, .power = -3 }
    });
    EcsCenti = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Centi",
        .symbol = "c",
        .translation = { .factor = 10, .power = -2 }
    });
    EcsDeci = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Deci",
        .symbol = "d",
        .translation = { .factor = 10, .power = -1 }
    });
    EcsDeca = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Deca",
        .symbol = "da",
        .translation = { .factor = 10, .power = 1 }
    });
    EcsHecto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Hecto",
        .symbol = "h",
        .translation = { .factor = 10, .power = 2 }
    });
    EcsKilo = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Kilo",
        .symbol = "k",
        .translation = { .factor = 10, .power = 3 }
    });
    EcsMega = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Mega",
        .symbol = "M",
        .translation = { .factor = 10, .power = 6 }
    });
    EcsGiga = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Giga",
        .symbol = "G",
        .translation = { .factor = 10, .power = 9 }
    });
    EcsTera = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Tera",
        .symbol = "T",
        .translation = { .factor = 10, .power = 12 }
    });
    EcsPeta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Peta",
        .symbol = "P",
        .translation = { .factor = 10, .power = 15 }
    });
    EcsExa = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Exa",
        .symbol = "E",
        .translation = { .factor = 10, .power = 18 }
    });
    EcsZetta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Zetta",
        .symbol = "Z",
        .translation = { .factor = 10, .power = 21 }
    });
    EcsYotta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Yotta",
        .symbol = "Y",
        .translation = { .factor = 10, .power = 24 }
    });

    EcsKibi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Kibi",
        .symbol = "Ki",
        .translation = { .factor = 1024, .power = 1 }
    });
    EcsMebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Mebi",
        .symbol = "Mi",
        .translation = { .factor = 1024, .power = 2 }
    });
    EcsGibi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Gibi",
        .symbol = "Gi",
        .translation = { .factor = 1024, .power = 3 }
    });
    EcsTebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Tebi",
        .symbol = "Ti",
        .translation = { .factor = 1024, .power = 4 }
    });
    EcsPebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Pebi",
        .symbol = "Pi",
        .translation = { .factor = 1024, .power = 5 }
    });
    EcsExbi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Exbi",
        .symbol = "Ei",
        .translation = { .factor = 1024, .power = 6 }
    });
    EcsZebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Zebi",
        .symbol = "Zi",
        .translation = { .factor = 1024, .power = 7 }
    });
    EcsYobi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Yobi",
        .symbol = "Yi",
        .translation = { .factor = 1024, .power = 8 }
    });

    ecs_set_scope(world, prev_scope);

    /* Duration units */

    EcsDuration = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Duration" });
    prev_scope = ecs_set_scope(world, EcsDuration);

        EcsSeconds = ecs_unit_init(world, &(ecs_unit_desc_t) {
            .entity.name = "Seconds",
            .quantity = EcsDuration,
            .symbol = "s" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsSeconds,
            .kind = EcsF32
        });
            EcsPicoSeconds = ecs_unit_init(world, &(ecs_unit_desc_t) {
                .entity.name = "PicoSeconds",
                .quantity = EcsDuration,
                .derived = EcsSeconds,
                .prefix = EcsPico });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsPicoSeconds,
                .kind = EcsF32
            });


            EcsNanoSeconds = ecs_unit_init(world, &(ecs_unit_desc_t) {
                .entity.name = "NanoSeconds",
                .quantity = EcsDuration,
                .derived = EcsSeconds,
                .prefix = EcsNano });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsNanoSeconds,
                .kind = EcsF32
            });

            EcsMicroSeconds = ecs_unit_init(world, &(ecs_unit_desc_t) {
                .entity.name = "MicroSeconds",
                .quantity = EcsDuration,
                .derived = EcsSeconds,
                .prefix = EcsMicro });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsMicroSeconds,
                .kind = EcsF32
            });

            EcsMilliSeconds = ecs_unit_init(world, &(ecs_unit_desc_t) {
                .entity.name = "MilliSeconds",
                .quantity = EcsDuration,
                .derived = EcsSeconds,
                .prefix = EcsMilli });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsMilliSeconds,
                .kind = EcsF32
            });

        EcsMinutes = ecs_unit_init(world, &(ecs_unit_desc_t) {
            .entity.name = "Minutes",
            .quantity = EcsDuration,
            .derived = EcsSeconds,
            .symbol = "min",
            .translation = { .factor = 60, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsMinutes,
            .kind = EcsU32
        });

        EcsHours = ecs_unit_init(world, &(ecs_unit_desc_t) {
            .entity.name = "Hours",
            .quantity = EcsDuration,
            .derived = EcsMinutes,
            .symbol = "h",
            .translation = { .factor = 60, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsHours,
            .kind = EcsU32
        });

        EcsDays = ecs_unit_init(world, &(ecs_unit_desc_t) {
            .entity.name = "Days",
            .quantity = EcsDuration,
            .derived = EcsHours,
            .symbol = "d",
            .translation = { .factor = 24, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsDays,
            .kind = EcsU32
        });
    ecs_set_scope(world, prev_scope);

    /* Time units */

    EcsTime = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Time" });
    prev_scope = ecs_set_scope(world, EcsTime);

        EcsDate = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Date",
            .quantity = EcsTime });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsDate,
            .kind = EcsU32
        });
    ecs_set_scope(world, prev_scope);

    EcsLength = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Length" });
    prev_scope = ecs_set_scope(world, EcsLength);
        EcsMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Meters",
            .quantity = EcsLength,
            .symbol = "m" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsMeters,
            .kind = EcsF32
        });

            EcsPicoMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "PicoMeters",
                .quantity = EcsLength,
                .derived = EcsMeters,
                .prefix = EcsPico });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsPicoMeters,
                .kind = EcsF32
            });

            EcsNanoMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "NanoMeters",
                .quantity = EcsLength,
                .derived = EcsMeters,
                .prefix = EcsNano });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsNanoMeters,
                .kind = EcsF32
            });

            EcsMicroMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "MicroMeters",
                .quantity = EcsLength,
                .derived = EcsMeters,
                .prefix = EcsMicro });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsMicroMeters,
                .kind = EcsF32
            });

            EcsMilliMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "MilliMeters",
                .quantity = EcsLength,
                .derived = EcsMeters,
                .prefix = EcsMilli });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsMilliMeters,
                .kind = EcsF32
            });

            EcsCentiMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "CentiMeters",
                .quantity = EcsLength,
                .derived = EcsMeters,
                .prefix = EcsCenti });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsCentiMeters,
                .kind = EcsF32
            });

            EcsKiloMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "KiloMeters",
                .quantity = EcsLength,
                .derived = EcsMeters,
                .prefix = EcsKilo });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsKiloMeters,
                .kind = EcsF32
            });
            
        EcsMiles = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Miles",
            .quantity = EcsLength,
            .symbol = "mi"
        });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsMiles,
            .kind = EcsF32
        });
    ecs_set_scope(world, prev_scope);

    /* Information units */

    EcsInformation = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Information" });
    prev_scope = ecs_set_scope(world, EcsInformation);

        EcsBits = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Bits",
            .quantity = EcsInformation,
            .symbol = "bit" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsBits,
            .kind = EcsU64
        });

            EcsKiloBits = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "KiloBits",
                .quantity = EcsInformation,
                .derived = EcsBits,
                .prefix = EcsKilo });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsKiloBits,
                .kind = EcsU64
            });

            EcsMegaBits = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "MegaBits",
                .quantity = EcsInformation,
                .derived = EcsBits,
                .prefix = EcsMega });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsMegaBits,
                .kind = EcsU64
            });

            EcsGigaBits = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "GigaBits",
                .quantity = EcsInformation,
                .derived = EcsBits,
                .prefix = EcsGiga });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsGigaBits,
                .kind = EcsU64
            });

        EcsBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Bytes",
            .quantity = EcsInformation,
            .symbol = "B",
            .derived = EcsBits,
            .translation = { .factor = 8, .power = 1 } });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsBytes,
            .kind = EcsU64
        });

            EcsKiloBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "KiloBytes",
                .quantity = EcsInformation,
                .derived = EcsBytes,
                .prefix = EcsKilo });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsKiloBytes,
                .kind = EcsU64
            });

            EcsMegaBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "MegaBytes",
                .quantity = EcsInformation,
                .derived = EcsBytes,
                .prefix = EcsMega });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsMegaBytes,
                .kind = EcsU64
            });

            EcsGigaBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "GigaBytes",
                .quantity = EcsInformation,
                .derived = EcsBytes,
                .prefix = EcsGiga });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsGigaBytes,
                .kind = EcsU64
            });

            EcsKibiBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "KibiBytes",
                .quantity = EcsInformation,
                .derived = EcsBytes,
                .prefix = EcsKibi });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsKibiBytes,
                .kind = EcsU64
            });

            EcsMibiBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "MebiBytes",
                .quantity = EcsInformation,
                .derived = EcsBytes,
                .prefix = EcsMebi });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsMibiBytes,
                .kind = EcsU64
            });

            EcsGibiBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
                .entity.name = "GibiBytes",
                .quantity = EcsInformation,
                .derived = EcsBytes,
                .prefix = EcsGibi });
            ecs_primitive_init(world, &(ecs_primitive_desc_t) {
                .entity.entity = EcsGibiBytes,
                .kind = EcsU64
            });

    ecs_set_scope(world, prev_scope);

    EcsPercentage = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Percentage" });
    ecs_unit_init(world, &(ecs_unit_desc_t) { 
        .entity.entity = EcsPercentage,
        .symbol = "%"
    });
    ecs_primitive_init(world, &(ecs_primitive_desc_t) {
        .entity.entity = EcsPercentage,
        .kind = EcsF32
    });

    /* Documentation */
#ifdef FLECS_DOC
    ecs_doc_set_brief(world, EcsDuration, 
        "Time amount (e.g. \"20 seconds\", \"2 hours\")");
    ecs_doc_set_brief(world, EcsSeconds, "Time amount in seconds");
    ecs_doc_set_brief(world, EcsMinutes, "60 seconds");
    ecs_doc_set_brief(world, EcsHours, "60 minutes");
    ecs_doc_set_brief(world, EcsDays, "24 hours");

    ecs_doc_set_brief(world, EcsTime,
        "Seconds passed since epoch (e.g. \"5pm\", \"March 3rd 2022\")");
    ecs_doc_set_brief(world, EcsDate,
        "Seconds passed since January 1st 1970");

    ecs_doc_set_brief(world, EcsLength,
        "Unit of length (e.g. \"5 meters\", \"20 miles\")");
    ecs_doc_set_brief(world, EcsMeters, "Length in meters");
    ecs_doc_set_brief(world, EcsMiles, "Length in miles");

    ecs_doc_set_brief(world, EcsInformation,
        "Unit of information (e.g. \"8 bits\", \"100 megabytes\")");
    ecs_doc_set_brief(world, EcsBits, "Basic unit of information (0 or 1)");
    ecs_doc_set_brief(world, EcsBytes, "8 bits");
#endif
}

#endif
