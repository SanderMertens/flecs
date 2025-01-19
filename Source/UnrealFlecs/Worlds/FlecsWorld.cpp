// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorld.h"
#include "FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorld)

UFlecsWorldSubsystem* UFlecsWorld::GetContext() const
{
	return static_cast<UFlecsWorldSubsystem*>(World.get_ctx());
}
