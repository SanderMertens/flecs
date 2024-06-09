// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsTickDeveloperSettings.generated.h"

USTRUCT(BlueprintType)
struct FFlecsTickerSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Ticker")
	FString TickerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Ticker")
	bool bTickableWhenPaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Ticker")
	bool bStartWithTickEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Ticker")
	bool bHasTickRate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Ticker", meta = (EditCondition = "bHasTickRate",
		ClampMin = "0.0", UIMin = "0.0", Units = "Hz"))
	float TickerRate = 60.0f;
	
}; // struct FFlecsTickerSettings

UCLASS(BlueprintType, Config = Game, DefaultConfig)
class UNREALFLECS_API UFlecsTickDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
}; // class UFlecsTickDeveloperSettings
