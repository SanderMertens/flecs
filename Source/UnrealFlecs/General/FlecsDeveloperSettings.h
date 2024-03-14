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
	bool bEnableFlecs : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	bool bAutoCreateWorld : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	bool bAutoDestroyWorld : 1 = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	bool bAutoTickWorld : 1 = true;

}; // class UFlecsDeveloperSettings
