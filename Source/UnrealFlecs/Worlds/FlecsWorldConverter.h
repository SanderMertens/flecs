// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"
#include "Types/SolidNotNull.h"

class UFlecsWorld;

namespace Unreal::Flecs
{
	/**
	 * @brief Converts a flecs::world to its corresponding UFlecsWorld
	 * @param InWorld The flecs::world to convert
	 * @return The corresponding UFlecsWorld
	 */
	UNREALFLECS_API NO_DISCARD TSolidNotNull<UFlecsWorld*> ToUnrealFlecsWorld(const flecs::world& InWorld);
	
} // namespace Unreal::Flecs