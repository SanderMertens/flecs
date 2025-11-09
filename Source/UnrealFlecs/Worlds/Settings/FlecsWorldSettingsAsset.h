// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"
#include "Misc/DataValidation.h"

#include "FlecsWorldInfoSettings.h"

#include "FlecsWorldSettingsAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsWorldSettingsAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFlecsWorldSettingsAsset();
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	FFlecsWorldSettingsInfo WorldSettings;

	virtual void PostLoad() override;

#if WITH_EDITOR

	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	EDataValidationResult CheckForDuplicateModules(FDataValidationContext& Context, TArrayView<TObjectPtr<UObject>> ImportedModules) const;
	EDataValidationResult CheckForHardDependencies(FDataValidationContext& Context, TArrayView<TObjectPtr<UObject>> ImportedModules) const;

#endif // WITH_EDITOR

	virtual FPrimaryAssetId GetPrimaryAssetId() const override final;

}; // class UFlecsWorldSettingsAsset
