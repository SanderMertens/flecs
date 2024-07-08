// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FlecsNetworkDeveloperSettings.generated.h"

UCLASS(BlueprintType, Config = Flecs, DefaultConfig, Category = "Flecs",
	meta = (DisplayName = "Flecs Network Settings"))
class UNREALFLECS_API UFlecsNetworkDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	

}; // class UFlecsNetworkDeveloperSettings
