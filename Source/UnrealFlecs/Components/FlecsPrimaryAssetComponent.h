// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsPrimaryAssetComponent.generated.h"

USTRUCT()
struct FFlecsPrimaryAssetComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsPrimaryAssetComponent() = default;
	FORCEINLINE FFlecsPrimaryAssetComponent(const FPrimaryAssetId& InPrimaryAssetId) : PrimaryAssetId(InPrimaryAssetId) {}

	FORCEINLINE NO_DISCARD FPrimaryAssetId GetPrimaryAssetId() const
	{
		return PrimaryAssetId;
	}

	FORCEINLINE void SetPrimaryAssetId(const FPrimaryAssetId& InPrimaryAssetId)
	{
		PrimaryAssetId = InPrimaryAssetId;
	}

	FORCEINLINE NO_DISCARD bool IsValid() const
	{
		return PrimaryAssetId.IsValid();
	}

	UPROPERTY()
	FPrimaryAssetId PrimaryAssetId;
	
}; // struct FFlecsPrimaryAssetComponent
