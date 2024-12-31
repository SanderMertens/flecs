// Fill out your copyright notice in the Description page of Project Settings.

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
	TObjectPtr<UFlecsWorldSettingsAsset> DefaultWorld;

}; // class AFlecsWorldSettings
