
#include "../private_api.h"

#ifdef FLECS_UNITS

ECS_DECLARE(EcsDuration);
    ECS_DECLARE(EcsSeconds);

ECS_DECLARE(EcsTime);
    ECS_DECLARE(EcsDate);

ECS_DECLARE(EcsPercent);

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

    EcsDuration = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Duration" });

        EcsSeconds = ecs_unit_init(world, &(ecs_unit_desc_t) {
            .entity.name = "Seconds",
            .quantity = EcsDuration,
            .symbol = "s" });

    EcsTime = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Time" });

        EcsDate = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Date",
            .quantity = EcsTime });

    EcsDistance = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Distance" });

        EcsMeters = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Meters",
            .quantity = EcsDistance,
            .symbol = "m" });

        EcsMiles = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Miles",
            .quantity = EcsDistance,
            .symbol = "mi"
        });

    EcsData = ecs_quantity_init(world, &(ecs_entity_desc_t) { 
        .name = "Data" });

        EcsDataByte = ecs_unit_init(world, &(ecs_unit_desc_t) { 
            .entity.name = "Byte",
            .quantity = EcsData,
            .symbol = "B" });
}

#endif
