// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Properties/FlecsComponentProperties.h"

#include "FlecsGameLoopTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsGameLoopTag
{
	GENERATED_BODY()
}; // struct FFlecsGameLoopTag

REGISTER_FLECS_COMPONENT(FFlecsGameLoopTag);

