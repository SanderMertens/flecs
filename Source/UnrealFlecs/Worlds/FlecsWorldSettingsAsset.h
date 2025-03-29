// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorldInfoSettings.h"
#include "Engine/DataAsset.h"
#include "Misc/DataValidation.h"
#include "FlecsWorldSettingsAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNREALFLECS_API UFlecsWorldSettingsAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFlecsWorldSettingsAsset()
	{
		WorldSettings.WorldName = "DefaultFlecsWorld";
	}
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FFlecsWorldSettingsInfo WorldSettings;

	#if WITH_EDITOR

	//virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) override;

	#endif // WITH_EDITOR

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override final
	{
		return FPrimaryAssetId("FlecsWorld", GetFName());
	}
	
}; // class UFlecsWorldSettingsAsset
