// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsPrefabAsset.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsPrefabAsset)

void UFlecsPrefabAsset::OnEntityCreated(MAYBE_UNUSED const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
{
	PrefabEntityHandle = World->CreatePrefabWithRecord(EntityRecord, PrefabName);
}

void UFlecsPrefabAsset::OnEntityDestroyed(MAYBE_UNUSED const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
{
	World->DestroyPrefab(PrefabEntityHandle);
}
