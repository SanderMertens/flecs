#if WITH_AUTOMATION_TESTS

#include "FlecsTestTypes.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FlecsBasicTest,
                                 "UnrealFlecs.Basic",
                                 EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter |
                                 EAutomationTestFlags::CriticalPriority)

bool FlecsBasicTest::RunTest(const FString& Parameters)
{
	const FFlecsTestFixtureRAII Fixture = FFlecsTestFixtureRAII();
	
	// World Tests
	{
		// Should be able to create a flecs::world
		{
			UTEST_TRUE_EXPR(IsValid(Fixture->GetFlecsWorld()));
		}

		// Should be able to convert flecs::world to UFlecsWorld using ToFlecsWorld Function
		{
			UTEST_TRUE("Should be able to convert flecs::world to UFlecsWorld using ToFlecsWorld Function",
				IsValid(Unreal::Flecs::ToFlecsWorld(Fixture->GetFlecsWorld()->World)));
		}
		
	} // End of World Tests

	// Entity Basic Tests
	{
		static constexpr FFlecsId TestEntityId = FLECS_HI_COMPONENT_ID + 5012;

		const FFlecsEntityHandle TestEntity = Fixture->GetFlecsWorld()->CreateEntity();
		UTEST_VALID_EXPR(TestEntity);

		// Entity with specific ID creation
		{
			const FFlecsEntityHandle TestEntityWithId = Fixture->GetFlecsWorld()->CreateEntityWithId(TestEntityId);
			UTEST_VALID_EXPR(TestEntityWithId);
			UTEST_EQUAL_EXPR(TestEntityWithId.GetFlecsId(), TestEntityId);
			
			UTEST_TRUE_EXPR(Fixture->GetFlecsWorld()->IsAlive(TestEntityWithId));

			const FFlecsEntityHandle FindEntityWithId = Fixture->GetFlecsWorld()->GetEntity(TestEntityId);
			UTEST_VALID_EXPR(FindEntityWithId);
			UTEST_EQUAL_EXPR(FindEntityWithId, TestEntityWithId);
		}

		// Named Entity Creation
		{
			const FFlecsEntityHandle TestEntityWithName = Fixture->GetFlecsWorld()->CreateEntity(TEXT("TestEntityWithName"));
			UTEST_VALID_EXPR(TestEntityWithName);
			UTEST_TRUE_EXPR(TestEntityWithName.HasName());
			UTEST_EQUAL_EXPR(TestEntityWithName.GetName(), TEXT("TestEntityWithName"));

			const FFlecsEntityHandle FindEntityWithName = Fixture->GetFlecsWorld()->LookupEntity(TEXT("TestEntityWithName"));
			UTEST_VALID_EXPR(FindEntityWithName);
			UTEST_EQUAL_EXPR(FindEntityWithName, TestEntityWithName);
		}

		// Add/Remove Tag for TestEntity
		{
			const FFlecsEntityHandle TagEntity = Fixture->GetFlecsWorld()->CreateEntity();

			TestEntity.Add(TagEntity);
			UTEST_TRUE_EXPR(TestEntity.Has(TagEntity));

			TestEntity.Remove(TagEntity);
			UTEST_FALSE_EXPR(TestEntity.Has(TagEntity));
		}

		// Add/Remove USTRUCT Component for TestEntity registered with CPP API
		{
			const FFlecsComponentHandle TestComponent = Fixture->GetFlecsWorld()->RegisterComponentType<FFlecsTestStruct_Value>();
			UTEST_VALID_EXPR(TestComponent);
			UTEST_TRUE_EXPR(TestComponent.IsComponent());
			UTEST_FALSE_EXPR(TestComponent.IsTag());

			// Entity API
			{
				TestEntity.Add(TestComponent);
				UTEST_TRUE_EXPR(TestEntity.Has(TestComponent));
				UTEST_TRUE_EXPR(TestEntity.Has<FFlecsTestStruct_Value>());
				UTEST_TRUE_EXPR(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));

				TestEntity.Remove(TestComponent);
				UTEST_FALSE_EXPR(TestEntity.Has(TestComponent));
				UTEST_FALSE_EXPR(TestEntity.Has<FFlecsTestStruct_Value>());
				UTEST_FALSE_EXPR(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));
			}

			// CPP API
			{
				TestEntity.Add<FFlecsTestStruct_Value>();
				UTEST_TRUE_EXPR(TestEntity.Has<FFlecsTestStruct_Value>());
				UTEST_TRUE_EXPR(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));
				UTEST_TRUE_EXPR(TestEntity.Has(TestComponent));

				TestEntity.Remove<FFlecsTestStruct_Value>();
				UTEST_FALSE_EXPR(TestEntity.Has<FFlecsTestStruct_Value>());
				UTEST_FALSE_EXPR(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));
				UTEST_FALSE_EXPR(TestEntity.Has(TestComponent));
			}

			// Script Struct API
			{
				TestEntity.Add(FFlecsTestStruct_Value::StaticStruct());
				UTEST_TRUE_EXPR(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));
				UTEST_TRUE_EXPR(TestEntity.Has<FFlecsTestStruct_Value>());
				UTEST_TRUE_EXPR(TestEntity.Has(TestComponent));

				TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
				UTEST_FALSE_EXPR(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));
				UTEST_FALSE_EXPR(TestEntity.Has<FFlecsTestStruct_Value>());
				UTEST_FALSE_EXPR(TestEntity.Has(TestComponent));
			}
		}
		
	} // End of Entity and Component Basic Tests

	// World Ticking Tests
	{
		// Tick World 3 times
		{
			uint32 TickCount = 0;
			Fixture->GetFlecsWorld()->CreateSystemWithBuilder("TestSystem")
					.run([&TickCount](flecs::iter& Iter)
					{
						++TickCount;
					});

			Fixture->TickWorld();
			Fixture->TickWorld();
			Fixture->TickWorld();

			UTEST_EQUAL_EXPR(TickCount, 3);
		}
	}
	
	return true;
}

#endif // #if WITH_AUTOMATION_TESTS