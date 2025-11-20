// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DeveloperSettings.h"

#include "FlecsExplorerURLSettings.h"

#include "FlecsEditorPerProjectDeveloperSettings.generated.h"

UCLASS(MinimalAPI, config=EditorPerProjectUserSettings, PerObjectConfig, DisplayName="Flecs Per-Project Editor Settings")
class UFlecsEditorPerProjectDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	/**
	 * @brief Override URL used to connect to the Flecs Explorer for this project. Leave empty to use the global editor setting.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Explorer")
	TOptional<FFlecsEditorExplorerURL> FlecsExplorerURLOverride;

}; // class UFlecsEditorPerProjectDeveloperSettings
