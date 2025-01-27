// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FFlecsSceneComponentTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsSceneComponentTag
{
	GENERATED_BODY()
}; // struct FFlecsSceneComponentTag

REGISTER_USTRUCT_FLECS_COMPONENT(FFlecsSceneComponentTag, [](flecs::world InWorld, flecs::untyped_component InComponent)
	{
		InComponent.add(flecs::Target);
	});