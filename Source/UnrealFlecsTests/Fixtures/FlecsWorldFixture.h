// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/Object.h"
#include "Worlds/FlecsWorldSubsystem.h"

#define FLECS_LOAD_OBJECT(AssetType, AssetPath) \
	StaticLoadObject(AssetType::StaticClass(), nullptr, AssetPath, nullptr, LOAD_None, nullptr)

UNLOG_CATEGORY(LogFlecsTests);

class FFlecsTestFixture
{
public:
	TWeakObjectPtr<UWorld> TestWorld = nullptr;
	TWeakObjectPtr<UFlecsWorldSubsystem> WorldSubsystem = nullptr;
	TWeakObjectPtr<UFlecsWorld> FlecsWorld = nullptr;
	bool bOldAssertNoFlecsWorld = false;

	void SetUp(const TArray<UObject*>& InModules = {})
	{
		UFlecsDeveloperSettings* DeveloperSettings = GetMutableDefault<UFlecsDeveloperSettings>();
		bOldAssertNoFlecsWorld = DeveloperSettings->bAssertNoFlecsWorld;
		
		/* Disable assert on no Flecs world as we have no way to spawn the
		 * AInfo Actor before the world is created since we are not in the editor
		 * */
		DeveloperSettings->bAssertNoFlecsWorld = false;
		
		TestWorld = UWorld::CreateWorld(EWorldType::Game, false, TEXT("FlecsTestWorld"));
		check(TestWorld.IsValid());

		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext.SetCurrentWorld(TestWorld.Get());

		TestWorld->InitializeActorsForPlay(FURL());
		TestWorld->BeginPlay();
	
		WorldSubsystem = TestWorld->GetSubsystem<UFlecsWorldSubsystem>();
		check(WorldSubsystem.IsValid());
        
		// Create world settings
		FFlecsWorldSettingsInfo WorldSettings;
		WorldSettings.WorldName = TEXT("TestWorld");
		WorldSettings.Modules = InModules;
		
		FlecsWorld = WorldSubsystem->CreateWorld(TEXT("TestWorld"), WorldSettings);
	}

	void TearDown()
	{
		UFlecsDeveloperSettings* DeveloperSettings = GetMutableDefault<UFlecsDeveloperSettings>();
		DeveloperSettings->bAssertNoFlecsWorld = bOldAssertNoFlecsWorld;
		
		if (TestWorld.IsValid())
		{
			TestWorld->EndPlay(EEndPlayReason::Quit);
			TestWorld->DestroyWorld(false);
			GEngine->DestroyWorldContext(TestWorld.Get());
		}

		TestWorld.Reset();
		WorldSubsystem.Reset();
		FlecsWorld.Reset();
	}
	
}; // class FFlecsTestFixture

#define FLECS_FIXTURE_LIFECYCLE(FixtureName) \
	BeforeEach([this]() \
	{ \
		FixtureName.SetUp(); \
	}); \
	AfterEach([this]() \
	{ \
		FixtureName.TearDown(); \
	})
