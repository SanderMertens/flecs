// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FlecsDefaultEntitiesDeveloperSettings.generated.h"

UCLASS(BlueprintType, Config = Engine, DefaultConfig, meta = (DisplayName = "Flecs Default Entities Developer Settings"))
class UNREALFLECS_API UFlecsDefaultEntitiesDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Flecs | Default Entities")
	TArray<FName> DefaultEntities;

}; // class UFlecsDefaultEntitiesDeveloperSettings
