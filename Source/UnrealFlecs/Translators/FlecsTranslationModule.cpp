// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTranslationModule.h"
#include "FlecsTranslationSettingTraits.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTranslationModule)

void UFlecsTranslationModule::InitializeModule(const TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	InWorld->RegisterComponentType<FFlecsTranslationPropertyTrait>();
	
	InWorld->RegisterComponentType<FFlecsToUnreal>();
	InWorld->RegisterComponentType<FUnrealToFlecs>();
	InWorld->RegisterComponentType<FFlecsToBoth>();
}

void UFlecsTranslationModule::DeinitializeModule(const TSolidNotNull<UFlecsWorld*> InWorld)
{
	
}
