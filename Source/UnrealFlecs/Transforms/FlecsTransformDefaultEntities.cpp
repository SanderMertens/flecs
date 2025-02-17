// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTransformDefaultEntities.h"

DEFINE_DEFAULT_ENTITY(FlecsLocalTrait, 5002 + FLECS_HI_COMPONENT_ID, [](flecs::entity& InEntity)
{
});

DEFINE_DEFAULT_ENTITY(FlecsGlobalTrait, 5003 + FLECS_HI_COMPONENT_ID, [](flecs::entity& InEntity)
{
});

DEFINE_DEFAULT_ENTITY(FlecsRelativeTrait, 5004 + FLECS_HI_COMPONENT_ID, [](flecs::entity& InEntity)
{
});