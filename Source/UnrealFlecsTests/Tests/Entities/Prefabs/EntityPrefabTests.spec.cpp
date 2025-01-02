
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "EntityRecordTestStructs.h"

BEGIN_DEFINE_SPEC(FEntityPrefabTestsSpec,
                  "Flecs.Entity.Prefab",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEntityPrefabTestsSpec);

void FEntityPrefabTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Create and Apply Prefabs", [this]()
	{
		It("Should create a new entity prefab", [this]()
		{
			FFlecsEntityRecord PrefabRecord;

			PrefabRecord.AddComponent<FTestStruct_EntityRecord>(FTestStruct_EntityRecord{ 1 });

			FFlecsEntityHandle PrefabEntity = Fixture.FlecsWorld->CreatePrefabWithRecord(PrefabRecord);
			TestTrue("Prefab should be valid", PrefabEntity.IsValid());
			TestTrue("Prefab should have FFlecsEntityRecord component",
				PrefabEntity.Has<FFlecsEntityRecord>());
			TestTrue("Prefab should have the record", PrefabEntity.Has<FTestStruct_EntityRecord>());
			TestTrue("Prefab should have the correct record",
				PrefabEntity.Get<FTestStruct_EntityRecord>().IntegerValue == 1);
		});

		It("Should apply a record to a prefab", [this]()
		{
			FFlecsEntityRecord PrefabRecord;

			PrefabRecord.AddComponent<FTestStruct_EntityRecord>(FTestStruct_EntityRecord{ 1 });

			FFlecsEntityHandle Prefab = Fixture.FlecsWorld->CreatePrefabWithRecord(PrefabRecord);
			TestTrue("Prefab should be valid", Prefab.IsValid());

			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntityWithPrefab(Prefab);
			TestTrue("Entity should be valid", TestEntity.IsValid());
			TestTrue("Entity should have the record", TestEntity.Has<FTestStruct_EntityRecord>());
			TestTrue("Entity should have the correct record",
				TestEntity.Get<FTestStruct_EntityRecord>().IntegerValue == 1);
			TestFalse("Entity should not have the FFlecsEntityRecord component as its set to DontInherit",
				TestEntity.Has<FFlecsEntityRecord>());
		});
		
	});
}

#endif // WITH_AUTOMATION_TESTS
