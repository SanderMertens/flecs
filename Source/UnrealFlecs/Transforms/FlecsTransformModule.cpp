// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTransformModule.h"
#include "FlecsTransformComponents.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformModule)

UFlecsTransformModule::UFlecsTransformModule()
{
}

void UFlecsTransformModule::InitializeModule(
	TSolidNonNullPtr<UFlecsWorld> InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	InWorld->RegisterComponentType<FFlecsLocationComponent>();
	InWorld->RegisterComponentType<FFlecsRotationComponent>();
	InWorld->RegisterComponentType<FFlecsScaleComponent>();
}

void UFlecsTransformModule::DeinitializeModule(TSolidNonNullPtr<UFlecsWorld> InWorld)
{
}
