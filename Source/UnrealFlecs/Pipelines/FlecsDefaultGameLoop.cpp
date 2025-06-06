// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsDefaultGameLoop.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsDefaultGameLoop)

void UFlecsDefaultGameLoop::InitializeGameLoop(TSolidNotNull<UFlecsWorld*> InWorld)
{
}

bool UFlecsDefaultGameLoop::Progress(const double DeltaTime, const TSolidNotNull<UFlecsWorld*> InWorld)
{
	return InWorld->Progress(DeltaTime);
}
