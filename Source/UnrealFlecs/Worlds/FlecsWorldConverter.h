// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorldSubsystem.h"
#include "SolidMacros/Macros.h"

class UFlecsWorld;

namespace Unreal::Flecs
{
	NO_DISCARD FORCEINLINE TSolidNotNull<UFlecsWorld*> ToFlecsWorld(const flecs::world& InWorld)
	{
		solid_check(InWorld.get_ctx() != nullptr);

		const TSolidNotNull<UFlecsWorld*> FlecsWorld = static_cast<UFlecsWorldSubsystem*>(InWorld.get_ctx())->GetDefaultWorld();
		solid_checkf(IsValid(FlecsWorld), TEXT("FlecsWorld is not valid."));
		
		return FlecsWorld;
	}
	
} // namespace Unreal::Flecs