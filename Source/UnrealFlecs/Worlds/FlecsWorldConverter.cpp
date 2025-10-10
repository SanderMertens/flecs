// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorldConverter.h"

#include "FlecsWorldSubsystem.h"
#include "FlecsWorld.h"

TSolidNotNull<UFlecsWorld*> Unreal::Flecs::ToFlecsWorld(const flecs::world& InWorld)
{
	solid_check(InWorld.get_ctx() != nullptr);

	const TSolidNotNull<UFlecsWorld*> FlecsWorld = static_cast<UFlecsWorldSubsystem*>(InWorld.get_ctx())->GetDefaultWorldChecked();
	solid_checkf(IsValid(FlecsWorld), TEXT("FlecsWorld is not valid."));
		
	return FlecsWorld;
}
