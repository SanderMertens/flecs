// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "DefaultEntityDefinitions.h"

DEFINE_DEFAULT_ENTITY(TestEntityOption, 6000 + FLECS_HI_COMPONENT_ID, [](flecs::entity& InEntity)
{
});

DEFINE_DEFAULT_ENTITY(TestEntityOption2, 6001 + FLECS_HI_COMPONENT_ID, [](flecs::entity& InEntity)
{
	InEntity.add(flecs::Trait);
});
