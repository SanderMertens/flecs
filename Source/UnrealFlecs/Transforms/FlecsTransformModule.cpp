// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTransformModule.h"
#include "FlecsTransformDefaultEntities.h"
#include "Translators/FlecsTranslationModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformModule)

UFlecsTransformModule::UFlecsTransformModule()
{
}

void UFlecsTransformModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	const flecs::entity_t FlecsTransformSystemKind = flecs::PostUpdate;

	
}

void UFlecsTransformModule::DeinitializeModule(UFlecsWorld* InWorld)
{
}
