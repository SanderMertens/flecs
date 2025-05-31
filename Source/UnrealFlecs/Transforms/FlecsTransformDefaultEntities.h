// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"

// Gets the closest Entity with FGlobalTransform
UNREALFLECS_API DECLARE_DEFAULT_ENTITY(FlecsLocalTrait);
UNREALFLECS_API DECLARE_DEFAULT_ENTITY(FlecsGlobalTrait);

// Doesn't have own Transform, but is relative to another Entity
UNREALFLECS_API DECLARE_DEFAULT_ENTITY(FlecsRelativeTrait);
