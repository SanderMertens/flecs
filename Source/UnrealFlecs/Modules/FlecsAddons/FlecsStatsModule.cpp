// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsStatsModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsStatsModule)

void UFlecsStatsModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	#if WITH_EDITOR
	
	StatsEntity = InWorld->World.import<flecs::stats>();
	
	#endif // WITH_EDITOR
}

void UFlecsStatsModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	#if WITH_EDITOR

	if UNLIKELY_IF(!StatsEntity.IsValid())
	{
		return;
	}
	
	StatsEntity.Disable();
	
	#endif // WITH_EDITOR
}
