// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Tests/FlecsTestTypes.h"
#include "Worlds/FlecsWorld.h"

BEGIN_DEFINE_SPEC(FEntityRecordTestsSpec,
                  "Flecs.Entity.Record",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEntityRecordTestsSpec);

void FEntityRecordTestsSpec::Define()
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

	Describe("Test Creating and Applying Entity Records", [this]()
	{
		It("Should create a new entity record", [this]()
		{
			FFlecsEntityRecord Record;
			
			Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 1 });

			TestTrue("Record should have the component", !Record.Components.IsEmpty());
		});

		It("Should create an entity with a record", [this]()
		{
			FFlecsEntityRecord Record;
			
			Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 1 });

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntityWithRecord(Record);
			TestTrue("Entity should be valid", TestEntity.IsValid());
			
			TestTrue("Entity should have the component from the Record",
				TestEntity.Has<FFlecsTestStruct_Value>());
			
			TestEqual("Entity should have the correct value from the Record",
				TestEntity.Get<FFlecsTestStruct_Value>().Value, 1);
		});

		It("Should apply a record to an entity", [this]()
		{
			FFlecsEntityRecord Record;
			
			Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 1 });

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestTrue("Entity should be valid", TestEntity.IsValid());

			Record.ApplyRecordToEntity(TestEntity);
			TestTrue("Entity should have the Component from the record",
				TestEntity.Has<FFlecsTestStruct_Value>());
			TestEqual("Entity have the correct value from the record",
				TestEntity.Get<FFlecsTestStruct_Value>().Value, 1);
		});

		It("Should apply a record with a sub-entity to an entity", [this]()
		{
			FFlecsEntityRecord Record;
			
			Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 1 });

			const FFlecsEntityHandle SubEntity = Fixture.FlecsWorld->CreateEntity();
			TestTrue("Sub-entity should be valid", SubEntity.IsValid());

			const int32 Index = Record.AddSubEntity(FFlecsRecordSubEntity{ .Name = TEXT("SubEntity") });
			FFlecsRecordSubEntity& SubEntityRecord = Record.GetSubEntity(Index);
			
			FFlecsComponentTypeInfo NewComponent;
			NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
			NewComponent.ScriptStruct = FInstancedStruct::Make<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 1 });
			SubEntityRecord.Components.Add(NewComponent);

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();

			Record.ApplyRecordToEntity(TestEntity);
			TestTrue("Entity should have the Component from the record using the CPP API",
				TestEntity.Has<FFlecsTestStruct_Value>());
			TestEqual("Entity should have the correct record value",
				TestEntity.Get<FFlecsTestStruct_Value>().Value, 1);
			
			TestTrue("Entity should have the sub-entity from the record",
				TestEntity.Lookup(TEXT("SubEntity")).IsValid());
			TestTrue("Sub-entity should have the Component from the record",
				TestEntity.Lookup(TEXT("SubEntity")).Has<FFlecsTestStruct_Value>());
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
