// Elie Wiese-Namir © 2025. All Rights Reserved.

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
