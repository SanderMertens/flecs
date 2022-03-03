
#include "../private_api.h"

#ifdef FLECS_UNITS

ECS_DECLARE(EcsDuration);
    ECS_DECLARE(EcsSeconds);

ECS_DECLARE(EcsTime);
    ECS_DECLARE(EcsDate);

ECS_DECLARE(EcsPercentage);

ECS_DECLARE(EcsDistance);
    ECS_DECLARE(EcsMeters);
    ECS_DECLARE(EcsMiles);

ECS_DECLARE(EcsData);
    ECS_DECLARE(EcsDataByte);

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

    EcsDistance = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Distance" });

        EcsMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Meters",
            .quantity = EcsDistance,
            .symbol = "m" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsMeters,
            .kind = EcsF32
        });

        EcsMiles = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Miles",
            .quantity = EcsDistance,
            .symbol = "mi"
        });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsMiles,
            .kind = EcsF32
        });

    EcsData = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Data" });

        EcsDataByte = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Byte",
            .quantity = EcsData,
            .symbol = "B" });
        ecs_primitive_init(world, &(ecs_primitive_desc_t) {
            .entity.entity = EcsDataByte,
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
}

#endif
