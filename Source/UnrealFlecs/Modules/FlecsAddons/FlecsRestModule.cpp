// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsRestModule.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsRestModule)

void UFlecsRestModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	InWorld->SetSingleton<flecs::Rest>(flecs::Rest { static_cast<uint16>(Port),
			const_cast<ANSICHAR*>(StringCast<ANSICHAR>(*IPAddress).Get()) });
	RestEntity = InWorld->GetSingletonEntity<flecs::Rest>();
}

void UFlecsRestModule::DeinitializeModule(UFlecsWorld* InWorld)
{
	if UNLIKELY_IF(!RestEntity.IsValid())
	{
		return;
	}
	
	RestEntity.Disable();
}
