// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"

/*
 * Layout of the tests:
 * A. Add/Remove/Set Component in Deferred Context Tests
 */
TEST_CLASS_WITH_FLAGS(A9_DeferWorldTests, "UnrealFlecs.A9.World.Defer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
	| EAutomationTestFlags::CriticalPriority)
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsEntityHandle TestEntity;
	
	inline static FFlecsComponentHandle TestComponent;

	BEFORE_ALL()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();

		TestEntity = FlecsWorld->CreateEntity("TestEntity");
		TestComponent = FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
	}

	AFTER_ALL()
	{
		FlecsWorld = nullptr;
		Fixture.Reset();
	}

	TEST_METHOD(A1_AddRemoveComponentDeferred_Add_CPPAPI_Remove_CPPAPI)
	{
		FlecsWorld->Defer([&]()
		{
			TestEntity.Add<FFlecsTestStruct_Value>();
			// 
			ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should not have the component yet!"));
		});

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should have the component after the deferred context is applied!"));

		FlecsWorld->Defer([&]()
		{
			TestEntity.Remove<FFlecsTestStruct_Value>();
			
			ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should have the component before the deferred context is applied!"));
		});

		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should not have the component after the deferred context is applied!"));
	}

	TEST_METHOD(A2_AddRemoveComponentDeferred_Add_StaticStructAPI_Remove_StaticStructAPI)
	{
		FlecsWorld->Defer([&]()
		{
			TestEntity.Add(FFlecsTestStruct_Value::StaticStruct());
			// 
			ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should not have the component yet!"));
		});

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should have the component after the deferred context is applied!"));

		FlecsWorld->Defer([&]()
		{
			TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
			
			ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should have the component before the deferred context is applied!"));
		});

		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should not have the component after the deferred context is applied!"));
	}
	
};

#endif // WITH_AUTOMATION_TESTS
