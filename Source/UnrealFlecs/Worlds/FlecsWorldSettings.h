// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "GameFramework/Info.h"
#include "SolidMacros/Macros.h"
#include "FlecsWorldSettings.generated.h"

class UFlecsWorldSettingsAsset;

UCLASS(BlueprintType)
class UNREALFLECS_API AFlecsWorldSettings final : public AInfo
{
	GENERATED_BODY()

	friend class UFlecsWorldSubsystem;

	static FORCEINLINE void AssertFlecsWorldSettings(const UWorld* World)
	{
		solid_check(IsValid(World));
		solid_checkf(World->IsGameWorld(), TEXT("World %s is not a game world"), *World->GetName());

		TArray<AFlecsWorldSettings*> FoundSettings;
		
		for (TActorIterator<AFlecsWorldSettings> It(World); It; ++It)
		{
			FoundSettings.Add(*It);
		}

		checkf(FoundSettings.Num() == 1,
			TEXT("Expected exactly one Flecs world settings, but found %d"), FoundSettings.Num());
	}

	static FORCEINLINE NO_DISCARD bool HasValidFlecsWorldSettings(const UWorld* World)
	{
		solid_check(IsValid(World));
		solid_checkf(World->IsGameWorld(), TEXT("World %s is not a game world"), *World->GetName());

		TArray<AFlecsWorldSettings*> FoundSettings;
		
		for (TActorIterator<AFlecsWorldSettings> It(World); It; ++It)
		{
			FoundSettings.Add(*It);
		}

		return FoundSettings.Num() == 1;
	}

	static FORCEINLINE NO_DISCARD AFlecsWorldSettings* Get(const UObject* WorldContextObject)
	{
		solid_check(IsValid(WorldContextObject));
		
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		solid_check(IsValid(World));

		solid_checkf(World->IsGameWorld(), TEXT("World %s is not a game world"), *World->GetName());

		return *TActorIterator<AFlecsWorldSettings>(World);
	}

public:
	AFlecsWorldSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* Allowed to be null. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs", meta = (ExposeOnSpawn = true))
	TObjectPtr<UFlecsWorldSettingsAsset> DefaultWorld;

}; // class AFlecsWorldSettings
