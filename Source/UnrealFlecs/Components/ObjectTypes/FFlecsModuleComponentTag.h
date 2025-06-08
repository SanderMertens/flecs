// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FFlecsModuleComponentTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsModuleComponentTag : public FFlecsUObjectTag
{
	GENERATED_BODY()
}; // struct FFlecsModuleComponentTag

REGISTER_FLECS_COMPONENT(FFlecsModuleComponentTag,
	[](flecs::world InWorld, FFlecsComponentHandle& InComponent)
	{
		InComponent.Add(flecs::Target);
	});
