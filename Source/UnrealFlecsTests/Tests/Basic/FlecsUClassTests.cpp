// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "CQTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Misc/AutomationTest.h"
#include "Tests/FlecsTestTypes.h"
#include "Tests/Types/FlecsClassTestTypes.h"
#include "Worlds/FlecsWorld.h"

/*
 * Layout of the tests:
 * A. Class Registration as types
 */
TEST_CLASS_WITH_FLAGS(A7_FlecsUClassTests, "UnrealFlecs.A7_UClass",
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
	}

	AFTER_ALL()
	{
		Fixture.Reset();
	}

	TEST_METHOD(A1_ShouldRegisterUClassAsType_CPPAPI)
	{
		const FFlecsEntityHandle CPPTypedEntity = FlecsWorld->ObtainTypedEntity<UFlecsUObjectComponentTestObject>();
		ASSERT_THAT(IsTrue(CPPTypedEntity.IsValid()));

		const FFlecsEntityHandle ScriptClassTypedEntity = FlecsWorld->ObtainTypedEntity(UFlecsUObjectComponentTestObject::StaticClass());
		ASSERT_THAT(IsTrue(ScriptClassTypedEntity.IsValid()));

		ASSERT_THAT(IsTrue(CPPTypedEntity == ScriptClassTypedEntity,
			TEXT("TypedEntity should be equal to ScriptClassTypedEntity after creation")));
	}

	TEST_METHOD(A2_ShouldRegisterUClassAsType_ScriptClassAPI)
	{
		const FFlecsEntityHandle ScriptClassTypedEntity = FlecsWorld->ObtainTypedEntity(UFlecsUObjectComponentTestObject::StaticClass());
		ASSERT_THAT(IsTrue(ScriptClassTypedEntity.IsValid()));

		const FFlecsEntityHandle CPPTypedEntity = FlecsWorld->ObtainTypedEntity<UFlecsUObjectComponentTestObject>();
		ASSERT_THAT(IsTrue(CPPTypedEntity.IsValid()));

		ASSERT_THAT(IsTrue(ScriptClassTypedEntity == CPPTypedEntity,
			TEXT("ScriptClassTypedEntity should be equal to CPPTypedEntity after creation")));
	}
	
}; // End of A7_FlecsUClassTests

#endif // WITH_AUTOMATION_TESTS
