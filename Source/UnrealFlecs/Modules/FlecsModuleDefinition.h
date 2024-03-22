// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsModuleDefinition.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UNREALFLECS_API UFlecsModuleDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Module")
	TArray<TObjectPtr<UScriptStruct>> Structs;

	virtual FPrimaryAssetId GetPrimaryAssetId() const final override
	{
		return FPrimaryAssetId("FlecsModule", GetFName());
	}
	
}; // class UFlecsModuleDefinition
