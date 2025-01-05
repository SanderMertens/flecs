// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsActorTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsActorTag
{
	GENERATED_BODY()
}; // struct FFlecsActorTag

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsActorTag, flecs::PairIsTag)
