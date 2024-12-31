// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorldInfoSettings.h"
#include "Engine/DataAsset.h"
#include "FlecsWorldSettingsAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsWorldSettingsAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFlecsWorldSettingsAsset()
	{
		WorldSettings.WorldName = TEXT("DefaultFlecsWorld");
	}
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FFlecsWorldSettingsInfo WorldSettings;

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override final
	{
		return FPrimaryAssetId("FlecsWorld", GetFName());
	}
	
}; // class UFlecsWorldSettingsAsset
