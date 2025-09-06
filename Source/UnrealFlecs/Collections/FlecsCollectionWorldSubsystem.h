// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Worlds/FlecsAbstractWorldSubsystem.h"
#include "FlecsCollectionWorldSubsystem.generated.h"

class UFlecsCollectionDataAsset;

UCLASS()
class UNREALFLECS_API UFlecsCollectionWorldSubsystem final : public UFlecsAbstractWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Get (or build) the compiled prefab for a given collection asset */
	FFlecsEntityHandle GetOrBuildPrefab(const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset);
	
	NO_DISCARD FFlecsEntityHandle GetPrefabByAsset(
		const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FORCEINLINE FFlecsEntityHandle GetCollectionScope() const
	{
		return CollectionScopeEntity;
	}

private:
	UFUNCTION()
	void OnAssetAdded(const FAssetData& Data);

	UFUNCTION()
	void OnAssetRemoved(const FAssetData& Data);
	
	FDelegateHandle AssetAddedHandle;
	FDelegateHandle AssetRemovedHandle;

	UPROPERTY()
	FFlecsEntityHandle CollectionScopeEntity;
	
	TMap<TObjectKey<UFlecsCollectionDataAsset>, FFlecsEntityHandle> AssetToPrefab;
	TMap<FPrimaryAssetId, FFlecsEntityHandle> IdToPrefab;

	FFlecsEntityHandle BuildPrefabFromRecord(const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset,
		OUT TSet<const UFlecsCollectionDataAsset*>& BuildStack);

	void ResolveRootCollections(const FFlecsEntityHandle& RootPrefab,
		const TSolidNotNull<const UFlecsCollectionDataAsset*> Asset,
		OUT TSet<const UFlecsCollectionDataAsset*>& BuildStack);

	void ResolveChildCollections(const FFlecsEntityHandle& NodePrefab,
		OUT TSet<const UFlecsCollectionDataAsset*>& BuildStack);
	
}; // class UFlecsCollectionWorldSubsystem
