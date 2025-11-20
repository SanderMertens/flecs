// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/EngineBaseTypes.h"
#include "Engine/DeveloperSettings.h"

#include "SolidMacros/Macros.h"

#include "FlecsExplorerURLSettings.h"

#include "FlecsEditorDeveloperSettings.generated.h"

UCLASS(config=Editor, DefaultConfig, DisplayName="Flecs General Editor Settings")
class UNREALFLECS_API UFlecsEditorDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFlecsEditorDeveloperSettings(const FObjectInitializer& ObjectInitializer);

	// @TODO: Test this

	/**
	 * @brief URL used to connect to the Flecs Explorer.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Explorer")
	FFlecsEditorExplorerURL FlecsExplorerURL;

	NO_DISCARD FFlecsEditorExplorerURL GetFlecsExplorerURL() const;

#if WITH_EDITOR

	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

#endif // WITH_EDITOR
	

}; // class UFlecsEditorDeveloperSettings
