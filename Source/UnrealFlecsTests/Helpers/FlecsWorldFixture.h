// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "SolidMacros/Macros.h"
#include "Worlds/FlecsWorldSubsystem.h"

class FFlecsWorldFixture
{
public:
	static constexpr bool bInformEngineOfWorld = false;

	FORCEINLINE_DEBUGGABLE explicit FFlecsWorldFixture(
		const FFlecsWorldSettings& FlecsWorldSettings = FFlecsWorldSettings(TEXT("TestFlecsWorld"))
		, const FURL& URL = FURL())
	{
		if UNLIKELY_IF(GEngine)
		{
			return;
		}

		FlecsWorld = nullptr;

		static uint32 WorldCounter = 0;
		const FString WorldName = FString::Printf(TEXT("WorldFixture_%d"), ++WorldCounter);
 
		if (UWorld* World = UWorld::CreateWorld(EWorldType::Game, bInformEngineOfWorld, *WorldName, GetTransientPackage()))
		{
			FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
			WorldContext.SetCurrentWorld(World);
 
			World->InitializeActorsForPlay(URL);
			
			if LIKELY_IF(IsValid(World->GetWorldSettings()))
			{
				World->GetWorldSettings()->NotifyBeginPlay();
				World->GetWorldSettings()->NotifyMatchStarted();
			}
			
			World->BeginPlay();
			
			FlecsWorld = World->GetSubsystem<UFlecsWorldSubsystem>()->CreateWorld(TEXT("TestFlecsWorld"), FlecsWorldSettings);
			solid_checkf(IsValid(FlecsWorld), TEXT("Failed to create Flecs world"));
 
			WeakWorld = World;
		}
	}

	FORCEINLINE ~FFlecsWorldFixture()
	{
		if (WeakWorld.IsValid())
		{
			WeakWorld->BeginTearingDown();
			
			for (TActorIterator<AActor> It = TActorIterator<AActor>(WeakWorld.Get()); It; ++It)
			{
				It->Destroy();
			}

			if (GEngine)
			{
				GEngine->DestroyWorldContext(WeakWorld.Get());
			}
			
			WeakWorld->DestroyWorld(bInformEngineOfWorld);
		}

		delete FlecsWorld;
		FlecsWorld = nullptr;
	}

	FORCEINLINE NO_DISCARD UWorld* GetWorld() const
	{
		return WeakWorld.Get();
	}

	FORCEINLINE NO_DISCARD UFlecsWorld* GetFlecsWorld() const
	{
		return FlecsWorld;
	}

private:
	UFlecsWorld* FlecsWorld = nullptr;
	TWeakObjectPtr<UWorld> WeakWorld;
	
}; // class FFlecsWorldFixture
