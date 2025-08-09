// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsScopedDeferWindow.h"
#include "FlecsWorld.h"

FFlecsScopedDeferWindow::FFlecsScopedDeferWindow(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld)
	: FlecsWorld(InFlecsWorld)
{
	ensureMsgf(FlecsWorld->BeginDefer(),
		TEXT("Failed to begin defer window in FlecsWorld"));
}

FFlecsScopedDeferWindow::~FFlecsScopedDeferWindow()
{
	if LIKELY_IF(FlecsWorld.IsValid())
	{
		ensureMsgf(FlecsWorld->EndDefer(),
			TEXT("Failed to end defer window in FlecsWorld"));
	}
}
