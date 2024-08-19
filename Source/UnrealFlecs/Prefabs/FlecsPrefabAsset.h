// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Entities/FlecsEntityRecord.h"
#include "General/FlecsPrimaryDataAsset.h"
#include "FlecsPrefabAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsPrefabAsset : public UFlecsPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override final
	{
		return FPrimaryAssetId("FlecsPrefab", *GetName());
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FFlecsEntityRecord EntityRecord;

	FFlecsEntityHandle EntityHandle;

	FORCEINLINE virtual void OnEntityCreated(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World) override final;

	FORCEINLINE virtual void OnEntityDestroyed(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World) override final;


}; // class UFlecsPrefabAsset
