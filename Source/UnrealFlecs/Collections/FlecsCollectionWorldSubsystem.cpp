// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionWorldSubsystem.h"

#include "FlecsCollectionDataAsset.h"
#include "Logs/FlecsCategories.h"

#include "FlecsCollectionTypes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionWorldSubsystem)

void UFlecsCollectionWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const TSolidNotNull<UFlecsWorld*> FlecsWorld = GetFlecsWorld();

	FlecsWorld->RegisterComponentType<FFlecsCollectionPrefabTag>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionReferenceComponent>();

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetAddedHandle = AssetRegistry.OnAssetAdded().AddUObject(this, &UFlecsCollectionWorldSubsystem::OnAssetAdded);
	AssetRemovedHandle = AssetRegistry.OnAssetRemoved().AddUObject(this, &UFlecsCollectionWorldSubsystem::OnAssetRemoved);
	
	TArray<FAssetData> Found;
	
	FARFilter Filter;
	Filter.ClassPaths.Add(UFlecsCollectionDataAsset::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	AssetRegistry.GetAssets(Filter, Found);
	
	for (const FAssetData& Data : Found)
	{
		OnAssetAdded(Data);
	}
	
	UE_LOGFMT(LogFlecsWorld, Verbose, "UCollectionsModule registered");
}

void UFlecsCollectionWorldSubsystem::Deinitialize()
{
	const FAssetRegistryModule* AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>("AssetRegistry");
	
	if LIKELY_IF(AssetRegistryModule)
	{
		IAssetRegistry& AssetRegistry = AssetRegistryModule->Get();
		AssetRegistry.OnAssetAdded().Remove(AssetAddedHandle);
		AssetRegistry.OnAssetRemoved().Remove(AssetRemovedHandle);
	}
	
	AssetToPrefab.Empty();
	IdToPrefab.Empty();

	Super::Deinitialize();
}

void UFlecsCollectionWorldSubsystem::OnAssetAdded(const FAssetData& Data)
{
	if (Data.AssetClassPath != UFlecsCollectionDataAsset::StaticClass()->GetClassPathName())
	{
		return;
	}
	
	const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset = CastChecked<UFlecsCollectionDataAsset>(Data.GetAsset());

	GetOrBuildPrefab(Asset);

	UE_LOGFMT(LogFlecsWorld, Verbose, "Registered collection asset: {0}", *Asset->GetName());
}

void UFlecsCollectionWorldSubsystem::OnAssetRemoved(const FAssetData& Data)
{
	if (Data.AssetClassPath != UFlecsCollectionDataAsset::StaticClass()->GetClassPathName())
	{
		return;
	}

	const FPrimaryAssetId Id = FPrimaryAssetId("FlecsCollectionDataAsset", Data.AssetName);
	
	if (const FFlecsEntityHandle* Prefab = IdToPrefab.Find(Id))
	{
		if LIKELY_IF(Prefab->IsValid())
		{
			GetFlecsWorld()->DestroyPrefab(*Prefab);
		}
		
		IdToPrefab.Remove(Id);
	}

	for (auto It = AssetToPrefab.CreateIterator(); It; ++It)
	{
		if UNLIKELY_IF(!It.Key().ResolveObjectPtr() || !It.Value().IsValid())
		{
			It.RemoveCurrent();
		}
	}
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

	UE_LOGFMT(LogFlecsEntity, Verbose, "Building prefab for collection asset: {0}", *Asset->GetName());

	// Prevent circular dependencies
	TSet<const UFlecsCollectionDataAsset*> BuildStack;
	const FFlecsEntityHandle Prefab = BuildPrefabFromRecord(Asset, BuildStack);

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
			"Circular dependency detected when building prefab for collection asset: {0}", *Asset->GetName());
		return FFlecsEntityHandle();
	}

	BuildStack.Add(Asset);

	const TSolidNotNull<UFlecsWorld*> FlecsWorld = GetFlecsWorld();
	const FString PrefabName = GetCollectionName(Asset);

	const FFlecsEntityHandle RootPrefab = FlecsWorld->CreatePrefabWithRecord(Asset->Record, PrefabName);
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
				"Failed to build referenced collection prefab for asset: {0} in collection asset: {1}",
				*Collection->GetName(), *Asset->GetName());
			continue;
		}

		RootPrefab.SetIsA(ReferencedPrefab);
	}
}

void UFlecsCollectionWorldSubsystem::ResolveChildCollections(const FFlecsEntityHandle& NodePrefab,
	TSet<const UFlecsCollectionDataAsset*>& BuildStack)
{
	const TSolidNotNull<UFlecsWorld*> FlecsWorld = GetFlecsWorld();

	FlecsWorld->World.query_builder<FFlecsCollectionReferenceComponent*>()
		.with(flecs::ChildOf, NodePrefab)
		.each([&](flecs::entity ChildEntity, FFlecsCollectionReferenceComponent* ReferenceComponent)
		{
			const FFlecsEntityHandle ChildHandle(FlecsWorld, ChildEntity);

			if (ReferenceComponent)
			{
				if LIKELY_IF(ReferenceComponent->Collection)
				{
					const FFlecsEntityHandle OtherPrefab = GetOrBuildPrefab(ReferenceComponent->Collection);
					if UNLIKELY_IF(!OtherPrefab.IsValid())
					{
						// @TODO: Maybe BuildPrefabFromRecord here, hence why we have the BuildStack?
						UE_LOGFMT(LogFlecsEntity, Error,
							"Failed to build referenced collection prefab for asset: {0} in collection entity: {1}",
							*ReferenceComponent->Collection->GetName(), *ChildHandle.GetName());
					}
					else
					{
						ChildHandle.SetIsA(OtherPrefab);
					}
				}
				else
				{
					UE_LOGFMT(LogFlecsEntity, Warning,
						"Collection reference component on entity {0} has no collection set, skipping",
						*ChildHandle.GetName());
				}

				ChildHandle.Remove<FFlecsCollectionReferenceComponent>();
			}

			ResolveChildCollections(ChildHandle, BuildStack);
		});
}

FString UFlecsCollectionWorldSubsystem::GetCollectionName(const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset)
{
	return FString::Printf(TEXT("Collection_%s"), *Asset->GetName());
}
