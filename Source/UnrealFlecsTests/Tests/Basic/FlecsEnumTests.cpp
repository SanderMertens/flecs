// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#if WITH_AUTOMATION_TESTS

#include "CQTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Misc/AutomationTest.h"

TEST_CLASS_WITH_FLAGS_AND_TAGS(A5_UnrealFlecsEnumTests,
							   "UnrealFlecs.A5.Enums",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter
							   | EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Enum]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsComponentHandle EnumComponentEntity;

	BEFORE_ALL()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();

		
	}

	AFTER_ALL()
	{
		Fixture.Reset();
		FlecsWorld = nullptr;
	}

}; // End of A5_UnrealFlecsBasicEnumTests

#endif // #if WITH_AUTOMATION_TESTS
