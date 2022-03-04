
#include "../private_api.h"

#ifdef FLECS_UNITS

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
