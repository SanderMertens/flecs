// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FFlecsUObjectTag.h"
#include "Properties/FlecsComponentProperties.h"
#include "FFlecsModuleObject.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsModuleObjectTarget : public FFlecsUObjectTag
{
	GENERATED_BODY()
}; // struct FFlecsModuleObject

REGISTER_FLECS_COMPONENT(FFlecsModuleObjectTarget,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Target);
	});
