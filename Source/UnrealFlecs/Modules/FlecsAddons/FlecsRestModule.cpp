// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsRestModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsRestModule)

void UFlecsRestModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	uint16 ClientPieInstanceOffset = 0;
	
	const UWorld* UnrealWorld = InWorld->GetWorld();
	solid_check(IsValid(UnrealWorld));
	
	if (UnrealWorld && UnrealWorld->GetNetMode() == NM_Client)
	{
		ClientPieInstanceOffset = UE::GetPlayInEditorID();
	}
	
	const uint16 RestPort = ECS_REST_DEFAULT_PORT + ClientPieInstanceOffset;

	InWorld->SetSingleton<flecs::Rest>(flecs::Rest{ .port = RestPort});
	RestEntity = InWorld->GetSingletonEntity<flecs::Rest>();

	if (bImportStats)
	{
		StatsEntity = InWorld->ImportFlecsModule<flecs::stats>();
	}
}

void UFlecsRestModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	if (StatsEntity.IsValid())
	{
		StatsEntity.Disable();
	}
	
	if UNLIKELY_IF(!RestEntity.IsValid())
	{
		return;
	}
	
	RestEntity.Disable();
}
