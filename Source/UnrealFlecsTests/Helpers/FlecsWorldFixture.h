// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "SolidMacros/Macros.h"
#include "Worlds/FlecsWorldSubsystem.h"

class FFlecsWorldFixture
{
public:
	
	FORCEINLINE explicit FFlecsWorldFixture(
		const FFlecsWorldSettings& FlecsWorldSettings = FFlecsWorldSettings(TEXT("TestFlecsWorld"))
		, const FURL& URL = FURL())
	{
		if UNLIKELY_IF(GEngine == nullptr)
		{
			return;
		}

		static uint32 WorldCounter = 0;
		const FString WorldName = FString::Printf(TEXT("WorldFixture_%d"), WorldCounter++);
 
		if (UWorld* World = UWorld::CreateWorld(EWorldType::Game, true, *WorldName, GetTransientPackage()))
		{
			FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
			WorldContext.SetCurrentWorld(World);
 
			World->InitializeActorsForPlay(URL);
			
			if (IsValid(World->GetWorldSettings()))
			{
				World->GetWorldSettings()->NotifyBeginPlay();
				World->GetWorldSettings()->NotifyMatchStarted();
			}
			
			World->BeginPlay();
			
			FlecsWorld = World->GetSubsystem<UFlecsWorldSubsystem>()->CreateWorld(TEXT("TestFlecsWorld"), FlecsWorldSettings);
 
			WeakWorld = World;
		}
	}

	FORCEINLINE ~FFlecsWorldFixture()
	{
		if (WeakWorld.IsValid() && GEngine != nullptr)
		{
			WeakWorld->BeginTearingDown();
			
			for (TActorIterator<AActor> It = TActorIterator<AActor>(WeakWorld.Get()); It; ++It)
			{
				It->Destroy();
			}
 
			GEngine->DestroyWorldContext(WeakWorld.Get());
			WeakWorld->DestroyWorld(false);
		}
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
	UFlecsWorld* FlecsWorld;
	TWeakObjectPtr<UWorld> WeakWorld;
	
}; // class FFlecsWorldFixture
