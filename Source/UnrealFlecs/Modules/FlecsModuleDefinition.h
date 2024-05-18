// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsModuleDefinition.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UNREALFLECS_API UFlecsModuleDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	virtual FPrimaryAssetId GetPrimaryAssetId() const final override
	{
		return FPrimaryAssetId("FlecsModule", GetFName());
	}
	
}; // class UFlecsModuleDefinition
