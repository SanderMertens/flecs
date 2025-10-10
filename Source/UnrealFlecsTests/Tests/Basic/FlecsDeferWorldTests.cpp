// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "StructUtils/StructView.h"
#include "Tests/FlecsTestTypes.h"
#include "Worlds/FlecsWorld.h"

/*
 * Layout of the tests:
 * A. Add/Remove/Set Component in Deferred Context Tests
 * B. Scoped Defer Window Tests
 */
TEST_CLASS_WITH_FLAGS(A9_DeferWorldTests, "UnrealFlecs.A9_World.Defer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
	| EAutomationTestFlags::CriticalPriority)
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsEntityHandle TestEntity;
	
	inline static FFlecsComponentHandle TestComponent;

	BEFORE_EACH()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();

		TestEntity = FlecsWorld->CreateEntity("TestEntity");
		TestComponent = FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
	}

	AFTER_EACH()
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

	TEST_METHOD(A3_AddRemoveComponentDeferred_Add_EntityAPI_Remove_EntityAPI)
	{
		FlecsWorld->Defer([&]()
		{
			TestEntity.Add(TestComponent);
			
			ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should not have the component yet!"));
		});

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should have the component after the deferred context is applied!"));

		FlecsWorld->Defer([&]()
		{
			TestEntity.Remove(TestComponent);
			
			ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should have the component before the deferred context is applied!"));
		});

		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should not have the component after the deferred context is applied!"));
	}

	TEST_METHOD(A4_SetComponentDeferred_Set_CPPAPI)
	{
		FlecsWorld->Defer([&]()
		{
			TestEntity.Set<FFlecsTestStruct_Value>({ 42 });
			
			ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should not have the component yet!"));
		});

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should have the component after the deferred context is applied!"));

		const FFlecsTestStruct_Value& ComponentValue = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(ComponentValue.Value == 42));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
	}

	TEST_METHOD(A5_SetComponentDeferred_Set_StaticStructAPI)
	{
		FlecsWorld->Defer([&]()
		{
			static constexpr FFlecsTestStruct_Value ComponentValue{ 42 };
			
			TestEntity.Set(FFlecsTestStruct_Value::StaticStruct(), &ComponentValue);
			
			ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should not have the component yet!"));
		});

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should have the component after the deferred context is applied!"));

		const FFlecsTestStruct_Value& ComponentValue = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(ComponentValue.Value == 42));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
	}

	TEST_METHOD(B1_ScopedDeferWindow_Add_CPPAPI_Remove_CPPAPI)
	{
		{
			FFlecsScopedDeferWindow DeferWindow(FlecsWorld);
			solid_checkf(DeferWindow.IsValid(), TEXT("DeferWindow is not valid!"));

			TestEntity.Add<FFlecsTestStruct_Value>();
			// 
			ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should not have the component yet!"));
		}

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should have the component after the deferred context is applied!"));

		{
			FFlecsScopedDeferWindow DeferWindow(FlecsWorld);
			solid_checkf(DeferWindow.IsValid(), TEXT("DeferWindow is not valid!"));

			TestEntity.Remove<FFlecsTestStruct_Value>();
			
			ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>(),
				"TestEntity should have the component before the deferred context is applied!"));
		}

		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>(),
			"TestEntity should not have the component after the deferred context is applied!"));
	}
	
}; // End of A9_DeferWorldTests

#endif // WITH_AUTOMATION_TESTS
