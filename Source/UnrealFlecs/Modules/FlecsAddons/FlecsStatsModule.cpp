// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsStatsModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsStatsModule)

void UFlecsStatsModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	InModuleEntity.Destroy();
	ModuleEntity = InWorld->World.import<flecs::stats>();
}

void UFlecsStatsModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	ModuleEntity.Destroy();
}
