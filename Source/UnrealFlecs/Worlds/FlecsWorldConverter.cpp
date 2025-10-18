// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorldConverter.h"

#include "FlecsWorldSubsystem.h"
#include "FlecsWorld.h"
#include "UnrealFlecsWorldTag.h"

TSolidNotNull<UFlecsWorld*> Unreal::Flecs::ToUnrealFlecsWorld(const flecs::world& InWorld)
{
	solid_checkf(InWorld, TEXT("Passed in flecs::world is not valid."));
	solid_checkf(InWorld.has<FUnrealFlecsWorldTag>(), TEXT("Passed in flecs::world is not an Unreal Flecs World."));
	
	solid_check(InWorld.get_ctx() != nullptr);

	const TSolidNotNull<UFlecsWorld*> FlecsWorld = static_cast<UFlecsWorldSubsystem*>(InWorld.get_ctx())->GetDefaultWorldChecked();
	solid_checkf(IsValid(FlecsWorld), TEXT("FlecsWorld is not valid."));
		
	return FlecsWorld;
}
