// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "CoreMinimal.h"
#include "flecs.h"

class FFlecsLibraryWorldFixture
{
	TUniquePtr<flecs::world> World = nullptr;
	
public:
	void Setup()
	{
		World = MakeUnique<flecs::world>();
	}

	void TearDown()
	{
		World.Reset();
	}
	
}; // class FFlecsLibraryWorldFixture

#define FLECS_LIBRARY_FIXTURE_LIFECYCLE(FixtureName) \
	BeforeEach([this]() \
	{ \
		FixtureName.SetUp(); \
	}); \
	AfterEach([this]() \
	{ \
		FixtureName.TearDown(); \
	})

#endif // #if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)
