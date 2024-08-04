// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Entities/FlecsEntityRecord.h"
#include "FlecsPrefabAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsPrefabAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override final
	{
		return FPrimaryAssetId("FlecsPrefab", *GetName());
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FFlecsEntityRecord EntityRecord;


}; // class UFlecsPrefabAsset
