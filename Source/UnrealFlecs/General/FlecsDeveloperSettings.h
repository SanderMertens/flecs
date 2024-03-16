// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FlecsDeveloperSettings.generated.h"

UCLASS(BlueprintType, Config = Game, DefaultConfig, meta = (DisplayName = "Flecs Developer Settings"))
class UNREALFLECS_API UFlecsDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	uint32 bEnableFlecs : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	uint32 bAutoCreateWorld : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	uint32 bAutoDestroyWorld : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	uint32 bAutoTickWorld : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	uint32 bDefaultAutoMerge : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	uint32 bAutoImportExplorer : 1 = true;

}; // class UFlecsDeveloperSettings
