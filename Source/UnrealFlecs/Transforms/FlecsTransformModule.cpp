// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTransformModule.h"
#include "FlecsTransformComponents.h"
#include "FlecsTransformDefaultEntities.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformModule)

UFlecsTransformModule::UFlecsTransformModule()
{
}

void UFlecsTransformModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	const FFlecsId FlecsTransformSystemKind = flecs::PreUpdate;
}

void UFlecsTransformModule::DeinitializeModule(UFlecsWorld* InWorld)
{
}
