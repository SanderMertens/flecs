// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FFlecsModuleComponentTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsModuleComponentTag
{
	GENERATED_BODY()
}; // struct FFlecsModuleComponentTag

REGISTER_USTRUCT_FLECS_COMPONENT(FFlecsModuleComponentTag, [](flecs::world InWorld, flecs::untyped_component InComponent)
	{
		InComponent.add(flecs::Target);
	});
