// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FFlecsUObjectTag.h"
#include "Entities/FlecsComponentHandle.h"
#include "Properties/FlecsComponentProperties.h"
#include "FFlecsActorComponentTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsActorComponentTag : public FFlecsUObjectTag
{
	GENERATED_BODY()
}; // struct FFlecsActorComponentTag

REGISTER_FLECS_COMPONENT(FFlecsActorComponentTag,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Target);
	});
