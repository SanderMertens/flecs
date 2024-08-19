// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsPrefabAsset.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPrefabComponent)

void UFlecsPrefabAsset::OnEntityCreated(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
{
	EntityHandle = World->CreatePrefab(EntityRecord);
}

void UFlecsPrefabAsset::OnEntityDestroyed(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
{
	World->DestroyPrefab(EntityHandle);
}
