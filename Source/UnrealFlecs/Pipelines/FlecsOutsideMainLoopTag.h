// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Properties/FlecsComponentProperties.h"

#include "FlecsOutsideMainLoopTag.generated.h"

USTRUCT()
struct UNREALFLECS_API FFlecsOutsideMainLoopTag
{
	GENERATED_BODY()
}; // struct FFlecsOutsideMainLoopTag

REGISTER_FLECS_COMPONENT(FFlecsOutsideMainLoopTag);
