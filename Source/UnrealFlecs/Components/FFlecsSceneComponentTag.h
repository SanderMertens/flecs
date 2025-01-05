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

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsSceneComponentTag, flecs::PairIsTag);