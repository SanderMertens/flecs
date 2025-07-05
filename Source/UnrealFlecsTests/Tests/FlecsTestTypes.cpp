// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTestTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTestTypes)

DEFINE_DEFAULT_ENTITY(TestEntityOption, 6000 + FLECS_HI_COMPONENT_ID, [](flecs::entity InEntity)
{
});

DEFINE_DEFAULT_ENTITY(TestEntityOption2WithTrait,
	6001 + FLECS_HI_COMPONENT_ID, [](flecs::entity InEntity)
{
	InEntity.add(flecs::Trait);
});

FFlecsTestNativeGameplayTags FFlecsTestNativeGameplayTags::StaticInstance;