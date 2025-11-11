// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"

/*
 * Layout of the Tests:
 * A. Entity Record Application Tests
 * B. Entity Record Prefab Tests
 * C. Entity Record Sub-Entity Tests
 */
TEST_CLASS_WITH_FLAGS(B1_FlecsEntityRecordTests, "UnrealFlecs.B1_EntityRecords",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
			| EAutomationTestFlags::CriticalPriority)
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	BEFORE_EACH()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();
		
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent>();
		FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent_Second>();
		FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent_Data>();
	}

	AFTER_EACH()
	{
		FlecsWorld = nullptr;
		Fixture.Reset();
	}

	static TInstancedStruct<FFlecsEntityRecord> MakeRecordInstance(const FFlecsEntityRecord& In)
	{
		return TInstancedStruct<FFlecsEntityRecord>::Make(In);
	}

	TEST_METHOD(A1_ApplyRecord_AddsScriptStructComponent)
	{
		FFlecsEntityRecord Record;
		Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 123 });

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		Record.ApplyRecordToEntity(FlecsWorld, Entity);

		ASSERT_THAT(IsTrue(Entity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(Entity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [Value] = Entity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 123));

		const FFlecsTestStruct_Value* ValueByUStruct
			= static_cast<const FFlecsTestStruct_Value*>(Entity.TryGet(FFlecsTestStruct_Value::StaticStruct()));
		ASSERT_THAT(IsTrue(ValueByUStruct != nullptr));
		ASSERT_THAT(IsTrue(ValueByUStruct->Value == 123));
	}

	TEST_METHOD(A2_ApplyRecord_AddsTagComponent)
	{
		FFlecsEntityRecord Record;
		Record.AddComponent<FFlecsTestStruct_Tag>();

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		Record.ApplyRecordToEntity(FlecsWorld, Entity);

		ASSERT_THAT(IsTrue(Entity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(Entity.Has(FFlecsTestStruct_Tag::StaticStruct())));
	}

	TEST_METHOD(A3_ApplyRecord_AddsPairComponents_Tags)
	{
		FFlecsEntityRecord Record;
		
		FFlecsRecordPair Pair;
		Pair.First = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent>();
		Pair.Second = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent_Second>();
		Pair.PairValueType = EFlecsValuePairType::None;
		Record.AddComponent(MoveTemp(Pair));

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(Entity.IsValid()));
		
		Record.ApplyRecordToEntity(FlecsWorld, Entity);
		ASSERT_THAT(IsTrue(Entity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));
		ASSERT_THAT(IsTrue(Entity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(), FUSTRUCTPairTestComponent_Second::StaticStruct())));
		
		ASSERT_THAT(IsFalse(Entity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Data>()));
	}

	TEST_METHOD(A4_ApplyRecord_AddsPairComponents_Data_First)
	{
		FFlecsEntityRecord Record;
		
		FFlecsRecordPair Pair;
		Pair.First = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent_Data>(FUSTRUCTPairTestComponent_Data{ .Value = 123 });
		Pair.Second = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent>();
		Pair.PairValueType = EFlecsValuePairType::First;
		Record.AddComponent(MoveTemp(Pair));

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(Entity.IsValid()));

		Record.ApplyRecordToEntity(FlecsWorld, Entity);
		ASSERT_THAT(IsTrue(Entity.HasPair<FUSTRUCTPairTestComponent_Data, FUSTRUCTPairTestComponent>()));
		ASSERT_THAT(IsFalse(Entity.HasPair<FUSTRUCTPairTestComponent_Second, FUSTRUCTPairTestComponent>()));
		ASSERT_THAT(IsFalse(Entity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Data>()));

		const FUSTRUCTPairTestComponent_Data& Data
			= Entity.GetPairFirst<FUSTRUCTPairTestComponent_Data, FUSTRUCTPairTestComponent>();

		ASSERT_THAT(IsTrue(Data.Value == 123));
	}

	TEST_METHOD(A5_ApplyRecord_AddsPairComponents_Data_Second)
	{
		FFlecsEntityRecord Record;
		
		FFlecsRecordPair Pair;
		Pair.First = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent>();
		Pair.Second = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent_Data>(FUSTRUCTPairTestComponent_Data{ .Value = 456 });
		Pair.PairValueType = EFlecsValuePairType::Second;
		Record.AddComponent(MoveTemp(Pair));

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(Entity.IsValid()));
		
		Record.ApplyRecordToEntity(FlecsWorld, Entity);
		ASSERT_THAT(IsTrue(Entity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Data>()));
		ASSERT_THAT(IsFalse(Entity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));

		const FUSTRUCTPairTestComponent_Data& Data
			= Entity.GetPairSecond<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Data>();
		
		ASSERT_THAT(IsTrue(Data.Value == 456));
	}

	TEST_METHOD(A6_ApplyRecord_NoComponents_DoesNothing)
	{
		FFlecsEntityRecord Record;

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(Entity.IsValid()));
		
		Record.ApplyRecordToEntity(FlecsWorld, Entity);

		ASSERT_THAT(IsFalse(Entity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsFalse(Entity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(Entity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));
	}

	TEST_METHOD(A7_ApplyRecord_AddsMultipleComponents)
	{
		FFlecsEntityRecord Record;
		Record.AddComponent<FFlecsTestStruct_Tag>();
		Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 789 });
		
		FFlecsRecordPair Pair;
		Pair.First = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent>();
		Pair.Second = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent_Second>();
		Pair.PairValueType = EFlecsValuePairType::None;
		Record.AddComponent(MoveTemp(Pair));

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(Entity.IsValid()));
		
		Record.ApplyRecordToEntity(FlecsWorld, Entity);

		ASSERT_THAT(IsTrue(Entity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(Entity.Has<FFlecsTestStruct_Value>()));
		const auto& [Value] = Entity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 789));
		ASSERT_THAT(IsTrue(Entity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));
	}

	
	
}; // End of B1_FlecsEntityRecordTests

#endif // WITH_AUTOMATION_TESTS
