// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsComponentPropertiesAsset.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsComponentPropertiesAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Properties")
	TObjectPtr<UScriptStruct> ComponentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Properties")
	TArray<FFlecsEntityHandle> ComponentProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Properties")
	TArray<FInstancedStruct> ComponentPropertyStructs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Properties", AdvancedDisplay)
	bool bResetExistingProperties = false;

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override final
	{
		return FPrimaryAssetId("FlecsComponentProperties", GetFName());
	}

	virtual void PostLoad() override;

	virtual void BeginDestroy() override;

	#if WITH_EDITOR
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	#endif

}; // class UFlecsComponentPropertiesAsset
