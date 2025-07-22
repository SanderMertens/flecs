// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsScopedDeferWindow.h"
#include "FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsScopedDeferWindow)

FFlecsScopedDeferWindow::FFlecsScopedDeferWindow(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld)
	: FlecsWorld(InFlecsWorld)
{
	ensureMsgf(FlecsWorld->BeginDefer(),
		TEXT("Failed to begin defer window in FlecsWorld"));
}

FFlecsScopedDeferWindow::~FFlecsScopedDeferWindow()
{
	if LIKELY_IF(IsValid(FlecsWorld))
	{
		ensureMsgf(FlecsWorld->EndDefer(),
			TEXT("Failed to end defer window in FlecsWorld"));
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("FlecsWorld is invalid in FFlecsScopedDeferWindow destructor"));
	}
}
