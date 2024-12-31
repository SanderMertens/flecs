// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FlecsDeveloperSettings.generated.h"

UCLASS(BlueprintType, Config = Game, DefaultConfig, Category = "Flecs",
	meta = (DisplayName = "Flecs World Settings"))
class UNREALFLECS_API UFlecsDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	bool bEnableFlecs = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	bool bUseTaskThreads = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs", meta = (EditCondition = "bUseTaskThreads"))
	int32 TaskThreadCount = 4;

}; // class UFlecsDeveloperSettings
