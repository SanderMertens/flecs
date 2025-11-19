// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/WorldSettings.h"

#include "FlecsWorldSettings.generated.h"

class UFlecsWorldSettingsAsset;

UCLASS()
class UNREALFLECS_API AFlecsWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs", meta = (ExposeOnSpawn = true))
	bool bUseFlecsWorld = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs",
		meta = (ExposeOnSpawn = true, EditCondition = "bUseFlecsWorld"))
	TObjectPtr<UFlecsWorldSettingsAsset> DefaultWorld;

}; // class AFlecsWorldSettings
