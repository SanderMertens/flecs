// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorldSettings.h"
#include "Engine/DeveloperSettings.h"
#include "FlecsWorldDeveloperSettings.generated.h"

UCLASS(BlueprintType, Config = Game, DefaultConfig, meta = (DisplayName = "Flecs World Developer Settings"))
class UNREALFLECS_API UFlecsWorldDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFlecsWorldDeveloperSettings();

	virtual void PostInitProperties() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	TSet<FFlecsWorldSettings> Worlds;
	
}; // class UFlecsWorldDeveloperSettings
