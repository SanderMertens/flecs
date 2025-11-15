// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTestTypes.h"

#include "Types/SolidCppStructOps.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTestTypes)

DEFINE_SOLID_MOVEABLE_CPP_STRUCT(FUStructTestComponent_MovableUSTRUCT);
DEFINE_SOLID_MOVEABLE_CPP_STRUCT(FUStructTestComponent_LifecycleTracker);

DEFINE_DEFAULT_ENTITY(TestEntityOption, 6000 + FLECS_HI_COMPONENT_ID, [](flecs::entity InEntity)
{
});

namespace Unreal::Flecs::test::internal
{
	DEFINE_DEFAULT_ENTITY(TestEntityOption3InNamespace,
		6002 + FLECS_HI_COMPONENT_ID, [](flecs::entity InEntity)
	{
		
	});
	
} // namespace Unreal::Flecs::test::internal

DEFINE_DEFAULT_ENTITY(TestEntityOption2WithTrait,
	6001 + FLECS_HI_COMPONENT_ID, [](flecs::entity InEntity)
	{
		InEntity
			.add(flecs::Trait);
	});

DEFINE_DEFAULT_ENTITY(TestEntityOption4WithComponentValue,
	6003 + FLECS_HI_COMPONENT_ID, [](flecs::entity InEntity)
	{
		
	});

FFlecsTestNativeGameplayTags FFlecsTestNativeGameplayTags::StaticInstance;