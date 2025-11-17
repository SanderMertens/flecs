
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "FlecsAbstractWorldSubsystemTestTypes.h"

#include "Tests/FlecsTestTypes.h"

/*
 * Layout of Tests:
 * A. Abstract Flecs World Subsystem Initialization Tests
 */
TEST_CLASS_WITH_FLAGS(B3_FlecsWorldSubsystems, "UnrealFlecs.B3_FlecsWorldSubsystems",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
			| EAutomationTestFlags::CriticalPriority)
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	BEFORE_EACH()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();
	}
	
	AFTER_EACH()
	{
		FlecsWorld = nullptr;
		Fixture.Reset();
	}

	TEST_METHOD(A1_AbstractFlecsWorldSubsystem_FlecsWorldInitialization)
	{
		const UTestFlecsWorldSubsystem_Initialization* WorldSubsystem
			= FlecsWorld->GetWorld()->GetSubsystem<UTestFlecsWorldSubsystem_Initialization>();
		ASSERT_THAT(IsTrue(IsValid(WorldSubsystem)));
		
		ASSERT_THAT(IsTrue(WorldSubsystem->bWasFlecsWorldInitialized));
		ASSERT_THAT(IsTrue(IsValid(WorldSubsystem->GetFlecsWorld())));

		ASSERT_THAT(IsTrue(WorldSubsystem->TimesChecked == 0));
		++WorldSubsystem->TimesChecked;
	}
	
	TEST_METHOD(A2_AbstractFlecsWorldSubsystem_FlecsWorldInitialization_Again)
	{
		const UTestFlecsWorldSubsystem_Initialization* WorldSubsystem
			= FlecsWorld->GetWorld()->GetSubsystem<UTestFlecsWorldSubsystem_Initialization>();
		ASSERT_THAT(IsTrue(IsValid(WorldSubsystem)));
		
		ASSERT_THAT(IsTrue(WorldSubsystem->bWasFlecsWorldInitialized));
		ASSERT_THAT(IsTrue(IsValid(WorldSubsystem->GetFlecsWorld())));

		ASSERT_THAT(IsTrue(WorldSubsystem->TimesChecked == 0));
		++WorldSubsystem->TimesChecked;
	}
	
}; // End of B3_FlecsWorldSubsystems

#endif // WITH_AUTOMATION_TESTS
