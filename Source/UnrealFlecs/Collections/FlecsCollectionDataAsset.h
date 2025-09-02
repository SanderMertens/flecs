// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"
#include "Entities/FlecsEntityRecord.h"

#include "FlecsCollectionDataAsset.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsCollectionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFlecsCollectionDataAsset();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	TArray<TObjectPtr<UFlecsCollectionDataAsset>> Collections;

	UPROPERTY(EditAnywhere, Category="Collections")
	FFlecsEntityRecord Record;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	
	NO_DISCARD EDataValidationResult ValidateCollections(FDataValidationContext& Context) const;
#endif // WITH_EDITOR

}; // class UFlecsCollectionDataAsset
