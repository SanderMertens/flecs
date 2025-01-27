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

REGISTER_USTRUCT_FLECS_COMPONENT(FFlecsUObjectTag, [](flecs::world InWorld, flecs::untyped_component InComponent)
	{
		InComponent.add(flecs::Target);
	});


