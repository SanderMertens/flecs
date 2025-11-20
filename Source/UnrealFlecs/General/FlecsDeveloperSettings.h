// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DeveloperSettings.h"

#include "FlecsDeveloperSettings.generated.h"

UCLASS(MinimalAPI, BlueprintType, Config = Game, DefaultConfig, Category = "Flecs",
	meta = (DisplayName = "Flecs World Settings"))
class UFlecsDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief Enable Unreal Flecs Plugin.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Flecs", meta = (ConsoleVariable = "Flecs.UseFlecs"))
	bool bEnableFlecs = true;

	/**
	 * @brief Enable task threads for Flecs.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Flecs", meta = (ConsoleVariable = "Flecs.UseTaskThreads"))
	bool bUseTaskThreads = false;

	/**
	 * @brief Number of threads to use for Flecs task processing.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Flecs",
		meta = (EditCondition = "bUseTaskThreads", ConsoleVariable = "Flecs.TaskThreadCount"))
	int32 TaskThreadCount = 4;

	/**
	 * @brief Delete empty Flecs tables during Unreal Garbage Collection.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Flecs")
	bool bDeleteEmptyTablesOnGC = false;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs",
		meta = (EditCondition = "bDeleteEmptyTablesOnGC", EditConditionHides, ClampMin = "0", UIMin = "0", ForceUnits = "s"))
	double TimeBudget = 0.01;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs",
		meta = (EditCondition = "bDeleteEmptyTablesOnGC", EditConditionHides, ClampMin = "0", UIMin = "0", ClampMax = "65535", UIMax = "65535"))
	uint32 ClearGeneration = 0;

	UPROPERTY(EditAnywhere, Config, Category = "Flecs",
		meta = (EditCondition = "bDeleteEmptyTablesOnGC", EditConditionHides, ClampMin = "0", UIMin = "0", ClampMax = "65535", UIMax = "65535"))
	uint32 DeleteGeneration = 10;

	UNREALFLECS_API virtual void PostInitProperties() override;

#if WITH_EDITOR

	UNREALFLECS_API virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif // WITH_EDITOR

}; // class UFlecsDeveloperSettings
