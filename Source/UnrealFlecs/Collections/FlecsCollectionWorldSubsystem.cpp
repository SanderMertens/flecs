// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionWorldSubsystem.h"

#include "Logs/FlecsCategories.h"

#include "FlecsCollectionTypes.h"
#include "FlecsCollectionDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionWorldSubsystem)

void UFlecsCollectionWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const TSolidNotNull<UFlecsWorld*> FlecsWorld = GetFlecsWorld();

	FlecsWorld->RegisterComponentType<FFlecsCollectionPrefabTag>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionReferenceComponent>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionSlotTag>();

	CollectionScopeEntity = FlecsWorld->CreateEntity("CollectionScope")
		.Add(flecs::Module);
	
	UE_LOGFMT(LogFlecsWorld, Verbose, "UCollectionsModule registered");
}

void UFlecsCollectionWorldSubsystem::Deinitialize()
{
	AssetToPrefab.Empty();
	IdToPrefab.Empty();

	if (MAYBE_UNUSED UFlecsWorld* FlecsWorld = GetFlecsWorld())
	{
		if LIKELY_IF(CollectionScopeEntity.IsValid())
		{
			CollectionScopeEntity.Destroy();
		}
	}
		
	Super::Deinitialize();
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionAsset(const UFlecsCollectionDataAsset* Asset)
{
	solid_check(Asset);
	return GetOrBuildPrefab(Asset);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabByAsset(const UFlecsCollectionDataAsset* Asset) const
{
	solid_check(Asset);
	
	if (const FFlecsEntityHandle* Prefab = AssetToPrefab.Find(Asset))
	{
		return *Prefab;
	}

	return FFlecsEntityHandle();
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetOrBuildPrefab(const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset)
{
	// Check cache as we may have already built this prefab due to another asset referencing it
	if (const FFlecsEntityHandle* CachedPrefabEntity = AssetToPrefab.Find(Asset))
	{
		if LIKELY_IF(CachedPrefabEntity->IsValid())
		{
			return *CachedPrefabEntity;
		}
	}

	UE_LOGFMT(LogFlecsEntity, Verbose, "Building prefab for collection asset: {AssetName}",
		Asset->GetName());

	// Prevent circular dependencies
	TSet<const UFlecsCollectionDataAsset*> BuildStack;

	const FFlecsEntityHandle Prefab = BuildPrefabFromRecord(Asset, BuildStack)
		.SetParent(CollectionScopeEntity);

	AssetToPrefab.FindOrAdd(Asset) = Prefab;
	IdToPrefab.FindOrAdd(Asset->GetPrimaryAssetId()) = Prefab;
	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::BuildPrefabFromRecord(
	const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset, OUT TSet<const UFlecsCollectionDataAsset*>& BuildStack)
{
	if UNLIKELY_IF(BuildStack.Contains(Asset))
	{
		UE_LOGFMT(LogFlecsEntity, Error,
			"Circular dependency detected when building prefab for collection asset: {AssetName}", Asset->GetName());
		return FFlecsEntityHandle();
	}

	BuildStack.Add(Asset);

	const TSolidNotNull<UFlecsWorld*> FlecsWorld = GetFlecsWorld();

	const FFlecsEntityHandle RootPrefab = FlecsWorld->CreatePrefabWithRecord(Asset->Record, Asset->GetName());
	
	ResolveRootCollections(RootPrefab, Asset, BuildStack);
	ResolveChildCollections(RootPrefab, BuildStack);

	BuildStack.Remove(Asset);
	return RootPrefab;
}

void UFlecsCollectionWorldSubsystem::ResolveRootCollections(const FFlecsEntityHandle& RootPrefab,
	const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset, OUT TSet<const UFlecsCollectionDataAsset*>& BuildStack)
{
	for (const TSolidNotNull<const UFlecsCollectionDataAsset*> Collection : Asset->Collections)
	{
		const FFlecsEntityHandle ReferencedPrefab = GetOrBuildPrefab(Collection);
		
		if UNLIKELY_IF(!ReferencedPrefab.IsValid())
		{
			// @TODO: Maybe BuildPrefabFromRecord here, hence why we have the BuildStack?
			UE_LOGFMT(LogFlecsEntity, Error,
				"Failed to build referenced collection prefab for asset: {CollectionName} in collection asset: {AssetName}",
				Collection->GetName(), Asset->GetName());
			continue;
		}

		RootPrefab.SetIsA(ReferencedPrefab);
	}
}

void UFlecsCollectionWorldSubsystem::ResolveChildCollections(const FFlecsEntityHandle& NodePrefab,
	TSet<const UFlecsCollectionDataAsset*>& BuildStack)
{
	const TSolidNotNull<UFlecsWorld*> FlecsWorld = GetFlecsWorld();

	FlecsWorld->Defer([this, FlecsWorld, &NodePrefab, &BuildStack]()
	{
		FlecsWorld->World.query_builder<FFlecsCollectionReferenceComponent*>() // 0
			.with(flecs::ChildOf, NodePrefab) // 1
			.with<FFlecsCollectionSlotTag>().optional() // 2
			.each([&](flecs::iter Iter, size_t Index, FFlecsCollectionReferenceComponent* ReferenceComponent)
			{
				const FFlecsEntityHandle ChildHandle = Iter.entity(Index);

				if (ReferenceComponent)
				{
					if LIKELY_IF(IsValid(ReferenceComponent->Collection))
					{
						FFlecsEntityHandle OtherPrefab = GetPrefabByAsset(ReferenceComponent->Collection);
						if (!OtherPrefab.IsValid())
						{
							OtherPrefab = BuildPrefabFromRecord(ReferenceComponent->Collection, BuildStack)
								.SetParent(CollectionScopeEntity);
							
							if (!OtherPrefab.IsValid())
							{
								UE_LOGFMT(LogFlecsEntity, Error,
									"Failed to build referenced collection prefab for asset: {CollectionName} in entity {ChildEntity}",
									ReferenceComponent->Collection->GetName(), ChildHandle.GetName());
								return;
							}

							AssetToPrefab.FindOrAdd(ReferenceComponent->Collection) = OtherPrefab;
							IdToPrefab.FindOrAdd(ReferenceComponent->Collection->GetPrimaryAssetId()) = OtherPrefab;
						}

						ChildHandle.SetIsA(OtherPrefab);
					}
					else
					{
						UE_LOGFMT(LogFlecsEntity, Error,
							"Collection reference component on entity {ChildEntity} has no collection set, skipping",
							ChildHandle.GetName());
					}

					ChildHandle.Remove<FFlecsCollectionReferenceComponent>();
				}

				if (Iter.is_set(2))
				{
					ChildHandle.MarkSlot();
					
					ChildHandle.Remove<FFlecsCollectionSlotTag>();
				}

				ResolveChildCollections(ChildHandle, BuildStack);
			});
	});
}
