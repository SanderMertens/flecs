
#include "../private_api.h"

#ifdef FLECS_UNITS

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
    ECS_DECLARE(EcsSeconds);

ECS_DECLARE(EcsTime);
    ECS_DECLARE(EcsDate);

ECS_DECLARE(EcsPercentage);

ECS_DECLARE(EcsLength);
    ECS_DECLARE(EcsMeters);
    ECS_DECLARE(EcsMiles);

ECS_DECLARE(EcsInformation);
    ECS_DECLARE(EcsBits);
    ECS_DECLARE(EcsBytes);

void FlecsUnitsImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsUnits);

    ecs_set_name_prefix(world, "Ecs");

    /* Initialize unit prefixes */

    EcsPico = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Pico",
        .translation = { .factor = 10, .power = -12 }
    });
    EcsNano = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Nano",
        .translation = { .factor = 10, .power = -9 }
    });
    EcsMicro = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Micro",
        .translation = { .factor = 10, .power = -6 }
    });
    EcsMilli = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Milli",
        .translation = { .factor = 10, .power = -3 }
    });
    EcsCenti = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Centi",
        .translation = { .factor = 10, .power = -2 }
    });
    EcsDeci = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Deci",
        .translation = { .factor = 10, .power = -1 }
    });
    EcsDeca = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Deca",
        .translation = { .factor = 10, .power = 1 }
    });
    EcsHecto = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Hecto",
        .translation = { .factor = 10, .power = 2 }
    });
    EcsKilo = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Kilo",
        .translation = { .factor = 10, .power = 3 }
    });
    EcsMega = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Mega",
        .translation = { .factor = 10, .power = 6 }
    });
    EcsGiga = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Giga",
        .translation = { .factor = 10, .power = 9 }
    });
    EcsTera = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Tera",
        .translation = { .factor = 10, .power = 12 }
    });
    EcsPeta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Peta",
        .translation = { .factor = 10, .power = 15 }
    });
    EcsZetta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Zetta",
        .translation = { .factor = 10, .power = 18 }
    });
    EcsYotta = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Yotta",
        .translation = { .factor = 10, .power = 21 }
    });

    EcsKibi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Kibi",
        .symbol = "Ki",
        .translation = { .factor = 2, .power = 1 }
    });
    EcsMebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Mebi",
        .symbol = "Me",
        .translation = { .factor = 2, .power = 1 }
    });
    EcsGibi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Gibi",
        .symbol = "Gi",
        .translation = { .factor = 2, .power = 1 }
    });
    EcsTebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Tebi",
        .symbol = "Te",
        .translation = { .factor = 2, .power = 1 }
    });
    EcsPebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Pebi",
        .symbol = "Pe",
        .translation = { .factor = 2, .power = 1 }
    });
    EcsExbi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Exbi",
        .symbol = "Ex",
        .translation = { .factor = 2, .power = 1 }
    });
    EcsZebi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Zebi",
        .symbol = "Ze",
        .translation = { .factor = 2, .power = 1 }
    });
    EcsYobi = ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) {
        .entity.name = "Yobi",
        .symbol = "Yo",
        .translation = { .factor = 2, .power = 1 }
    });

    /* Initialize units with their default types */

    EcsDuration = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Duration" });

        EcsSeconds = ecs_unit_init(world, &(ecs_unit_desc_t) {
            .entity.name = "Seconds",
            .quantity = EcsDuration,
            .symbol = "s" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsSeconds,
            .kind = EcsF32
        });

    EcsTime = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Time" });

        EcsDate = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Date",
            .quantity = EcsTime });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsDate,
            .kind = EcsU32
        });

    EcsLength = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Length" });

        EcsMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Meters",
            .quantity = EcsLength,
            .symbol = "m" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsMeters,
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

    EcsInformation = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Information" });
        EcsBits = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Bits",
            .quantity = EcsInformation,
            .symbol = "bit" });
        EcsBytes = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Bytes",
            .quantity = EcsInformation,
            .symbol = "B" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsBits,
            .kind = EcsI32
        });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsBytes,
            .kind = EcsI32
        });

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
