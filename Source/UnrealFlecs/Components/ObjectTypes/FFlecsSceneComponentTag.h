// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FFlecsSceneComponentTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsSceneComponentTag : public FFlecsActorComponentTag
{
	GENERATED_BODY()
}; // struct FFlecsSceneComponentTag

REGISTER_FLECS_COMPONENT(FFlecsSceneComponentTag,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Target);
	});