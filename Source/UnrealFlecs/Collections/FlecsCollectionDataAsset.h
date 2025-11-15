// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"

#include "Entities/FlecsEntityRecord.h"
#include "FlecsCollectionTypes.h"
#include "FlecsCollectionDefinition.h"

#include "FlecsCollectionDataAsset.generated.h"

// @TODO: add versioning

UCLASS(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsCollectionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFlecsCollectionDataAsset();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override final;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TArray<FFlecsCollectionInstancedReference> Collections;

	UPROPERTY(EditAnywhere, Category = "Collections")
	FFlecsEntityRecord Record;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TMap<int32, FFlecsSubEntityCollectionReferences> SubEntityCollections;

	NO_DISCARD FFlecsCollectionDefinition MakeCollectionDefinition() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	
	NO_DISCARD EDataValidationResult ValidateCollections(FDataValidationContext& Context) const;
#endif // WITH_EDITOR

}; // class UFlecsCollectionDataAsset
