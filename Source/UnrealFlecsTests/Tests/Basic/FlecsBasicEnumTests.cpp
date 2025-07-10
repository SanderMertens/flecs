#pragma once

#if WITH_AUTOMATION_TESTS

#include "CQTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Misc/AutomationTest.h"

TEST_CLASS_WITH_FLAGS_AND_TAGS(A5_UnrealFlecsBasicEnumTests,
							   "UnrealFlecs.Basic.Enums",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter
							   | EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Basic][Enum]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsEntityHandle EnumComponentEntity;
	

}; // End of A5_UnrealFlecsBasicEnumTests

#endif // #if WITH_AUTOMATION_TESTS
