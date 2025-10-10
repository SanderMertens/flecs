// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FFlecsUObjectTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsUObjectTag
{
	GENERATED_BODY()
}; // struct FFlecsUObjectTag

REGISTER_FLECS_COMPONENT(FFlecsUObjectTag,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Target);
	});


