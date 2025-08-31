// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Tests/FlecsTestTypes.h"
#include "Worlds/FlecsWorld.h"

BEGIN_DEFINE_SPEC(FEntityPrefabTestsSpec,
                  "Flecs.Entity.Prefab",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEntityPrefabTestsSpec);

void FEntityPrefabTestsSpec::Define()
{
	BeforeEach([this]()
	{
		Fixture.SetUp();

		Fixture.FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
	});
	
	AfterEach([this]()
	{
		Fixture.TearDown();
	});
	
	Describe("Create and Apply Prefabs", [this]()
	{
		It("Should create and apply a record to a prefab", [this]()
		{
			FFlecsEntityRecord PrefabRecord;

			PrefabRecord.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 1 });

			const FFlecsEntityHandle Prefab = Fixture.FlecsWorld->CreatePrefabWithRecord(PrefabRecord);
			TestTrue("Prefab should be valid", Prefab.IsValid());

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntityWithPrefab(Prefab);
			TestTrue("Entity should be valid", TestEntity.IsValid());
			TestTrue("Entity should have the component from the Prefab",
				TestEntity.Has<FFlecsTestStruct_Value>());
			
			TestTrue("Entity should have the correct value from the Prefab",
				TestEntity.Get<FFlecsTestStruct_Value>().Value == 1);
			TestFalse("Entity should not have the FFlecsEntityRecord component as its set to DontInherit",
				TestEntity.Has<FFlecsEntityRecord>());
		});
		
	});
}

#endif // WITH_AUTOMATION_TESTS
