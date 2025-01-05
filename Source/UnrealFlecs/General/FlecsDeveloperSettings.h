// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FlecsDeveloperSettings.generated.h"

UCLASS(BlueprintType, Config = Game, DefaultConfig, Category = "Flecs", meta = (DisplayName = "Flecs World Settings"))
class UNREALFLECS_API UFlecsDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Flecs", meta = (ConsoleVariable = "Flecs.UseFlecs"))
	bool bEnableFlecs = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs", meta = (ConsoleVariable = "Flecs.UseTaskThreads"))
	bool bUseTaskThreads = true;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs",
		meta = (EditCondition = "bUseTaskThreads", ConsoleVariable = "Flecs.TaskThreadCount"))
	int32 TaskThreadCount = 4;

}; // class UFlecsDeveloperSettings
