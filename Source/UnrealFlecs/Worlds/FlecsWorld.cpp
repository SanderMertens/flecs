// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorld.h"
#include "FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorld)

// static bool GFlecs_bRegisterMemberTypeStructs = false;
// FAutoConsoleVariable CVarFlecsRegisterMemberTypeStructs(
// 	TEXT("Flecs.RegisterMemberTypeStructs"),
// 	GFlecs_bRegisterMemberTypeStructs,
// 	TEXT("Register the member type structs as components if not previously registered."),
// 	ECVF_Default);

UFlecsWorldSubsystem* UFlecsWorld::GetContext() const
{
	return static_cast<UFlecsWorldSubsystem*>(World.get_ctx());
}
