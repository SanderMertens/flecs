// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#if WITH_AUTOMATION_TESTS

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/Object.h"
#include "Worlds/FlecsWorldSubsystem.h"

class UNREALFLECSTESTS_API FFlecsTestFixture
{
public:
	TUniquePtr<FTestWorldWrapper> TestWorldWrapper;
	
	TWeakObjectPtr<UWorld> TestWorld;
	TStrongObjectPtr<UFlecsWorldSubsystem> WorldSubsystem = nullptr;
	TStrongObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	void SetUp(TScriptInterface<IFlecsGameLoopInterface> InGameLoopInterface = nullptr,
	           const TArray<UObject*>& InModules = {})
	{
		TestWorldWrapper = MakeUnique<FTestWorldWrapper>();
		TestWorldWrapper->CreateTestWorld(EWorldType::Game);

		TestWorld = TestWorldWrapper->GetTestWorld();

		WorldSubsystem = TStrongObjectPtr(TestWorld->GetSubsystem<UFlecsWorldSubsystem>());
		check(WorldSubsystem.IsValid());

		if (!InGameLoopInterface)
		{
			InGameLoopInterface = NewObject<UFlecsDefaultGameLoop>(WorldSubsystem.Get());
		}

		// Create world settings
		FFlecsWorldSettingsInfo WorldSettings;
		WorldSettings.WorldName = TEXT("TestWorld");
		WorldSettings.GameLoop = InGameLoopInterface.GetObject();
		WorldSettings.Modules = InModules;

		FlecsWorld = TStrongObjectPtr(WorldSubsystem->CreateWorld(TEXT("TestWorld"), WorldSettings));

		TestWorldWrapper->BeginPlayInTestWorld();
	}

	void TickWorld(const float DeltaTime = 0.01f) const
	{
		TestWorldWrapper->TickTestWorld(DeltaTime);
	}

	void TearDown()
	{
		if (FlecsWorld.IsValid())
		{
			FlecsWorld.Reset();
		}

		if (WorldSubsystem.IsValid())
		{
			WorldSubsystem.Reset();
		}

		TestWorldWrapper.Reset();
	}

	NO_DISCARD FORCEINLINE UFlecsWorld* GetFlecsWorld() const
	{
		return FlecsWorld.Get();
	}
	
}; // class FFlecsTestFixture

struct UNREALFLECSTESTS_API FFlecsTestFixtureRAII
{
	mutable FFlecsTestFixture Fixture;

	FFlecsTestFixtureRAII(const TScriptInterface<IFlecsGameLoopInterface> InGameLoopInterface = nullptr,
			   const TArray<UObject*>& InModules = {})
	{
		Fixture.SetUp(InGameLoopInterface, InModules);
	}

	~FFlecsTestFixtureRAII()
	{
		Fixture.TearDown();
	}

	FORCEINLINE FFlecsTestFixture* operator->() const
	{
		return &Fixture;
	}
	
}; // struct FFlecsTestFixtureRAII

#define FLECS_FIXTURE_LIFECYCLE(FixtureName) \
	BeforeEach([this]() \
	{ \
		FixtureName.SetUp(); \
	}); \
	AfterEach([this]() \
	{ \
		FixtureName.TearDown(); \
	}) \

#define FLECS_FIXTURE_LIFECYCLE_LATENT(FixtureName) \
	LatentBeforeEach([this](const FDoneDelegate& Done) \
	{ \
		FixtureName.SetUp(); \
		Done.Execute(); \
	}); \
	LatentAfterEach([this](const FDoneDelegate& Done) \
	{ \
		FixtureName.TearDown(); \
		Done.Execute(); \
	});

#endif // #if WITH_AUTOMATION_TESTS
