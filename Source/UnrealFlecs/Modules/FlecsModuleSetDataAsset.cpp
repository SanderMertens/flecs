// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsModuleSetDataAsset.h"
#include "FlecsModuleInterface.h"
#include "SolidMacros/Macros.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsModuleSetDataAsset)

void UFlecsModuleSetDataAsset::ImportModules(const TSolidNonNullPtr<UFlecsWorld> InWorld)
{
	for (UObject* Module : Modules)
	{
		if UNLIKELY_IF(!ensureAlways(IsValid(Module)))
		{
			continue;
		}

		solid_check(Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()));

		InWorld->ImportModule(Module);
	}
}
