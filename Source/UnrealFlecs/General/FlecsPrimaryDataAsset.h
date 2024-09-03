// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsPrimaryDataAsset.generated.h"

UCLASS(Abstract, BlueprintType)
class UNREALFLECS_API UFlecsPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	bool bEnabledOnStartup = true;

	virtual void OnEntityCreated(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
		PURE_VIRTUAL(UFlecsPrimaryDataAsset::OnEntityCreated, );
	virtual void OnEntityDestroyed(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World)
		PURE_VIRTUAL(UFlecsPrimaryDataAsset::OnEntityDestroyed, );

	FORCEINLINE NO_DISCARD bool ShouldSpawn() const
	{
		return bEnabledOnStartup;
	}

}; // class UFlecsPrimaryDataAsset
