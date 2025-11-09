// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"

#include "Entities/FlecsEntityHandle.h"

#include "FlecsPrimaryDataAsset.generated.h"

// Unused for now
UCLASS(Abstract, BlueprintType)
class UNREALFLECS_API UFlecsPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	bool bEnabledOnStartup = true;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override final
	{
		return FPrimaryAssetId("FlecsPrimaryDataAsset", GetFName());
	}

	virtual void OnEntityCreated(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
		PURE_VIRTUAL(UFlecsPrimaryDataAsset::OnEntityCreated, );
	virtual void OnEntityDestroyed(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
		PURE_VIRTUAL(UFlecsPrimaryDataAsset::OnEntityDestroyed, );

	NO_DISCARD bool ShouldSpawn() const;

}; // class UFlecsPrimaryDataAsset
