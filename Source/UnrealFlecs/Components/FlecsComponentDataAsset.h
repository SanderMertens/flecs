// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsComponentDataAsset.generated.h"

UENUM(BlueprintType)
enum class EFlecsComponentFlags : uint8
{
	None = 0,
	
}; // enum class EFlecsComponentFlags

UCLASS(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsComponentDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	TObjectPtr<UScriptStruct> ComponentDataStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	TSet<EFlecsComponentFlags> Flags;
	
}; // class UFlecsComponentDataAsset
