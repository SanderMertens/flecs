// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Entities/FlecsEntityRecord.h"

#include "Tests/FlecsTestTypes.h"

/*
 * Layout of the Tests:
 * A. Entity Record Application Tests
 * B. Entity Record Prefab Tests
 * C. Entity Record Fragment Tests
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
		FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
	}

	AFTER_EACH()
	{
		FlecsWorld = nullptr;
		Fixture.Reset();
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

	TEST_METHOD(A2_ApplyRecord_AddsTagComponent_And_GameplayTag)
	{
		FFlecsEntityRecord Record;
		Record.AddComponent<FFlecsTestStruct_Tag>();
		Record.AddComponent(FFlecsTestNativeGameplayTags::Get().TestTag2);

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		Record.ApplyRecordToEntity(FlecsWorld, Entity);

		ASSERT_THAT(IsTrue(Entity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(Entity.Has(FFlecsTestStruct_Tag::StaticStruct())));
		ASSERT_THAT(IsTrue(Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2)));
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

	TEST_METHOD(A8_ApplyRecord_MultipleTimes_AddScriptEnum)
	{
		FFlecsEntityRecord Record;
		const FSolidEnumSelector EnumValue = FSolidEnumSelector::Make<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One);
		Record.AddComponent(EnumValue);

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(Entity.IsValid()));
		
		Record.ApplyRecordToEntity(FlecsWorld, Entity);

		ASSERT_THAT(IsTrue(Entity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(Entity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsTrue(Entity.Has(StaticEnum<EFlecsTestEnum_UENUM>(),
			static_cast<int64>(EFlecsTestEnum_UENUM::One))));
	}

	TEST_METHOD(A9_ApplyRecord_WithSubEntities_AddsComponents)
	{
		FFlecsEntityRecord SubRecord;
		SubRecord.AddComponent<FFlecsTestStruct_Tag>();

		FFlecsEntityRecord Record;
		Record.AddSubEntity(SubRecord);

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(Entity.IsValid()));

		Record.ApplyRecordToEntity(FlecsWorld, Entity);
		// Should be in the sub-entity
		ASSERT_THAT(IsFalse(Entity.Has<FFlecsTestStruct_Tag>()));

		bool bFoundChild = false;
		Entity.IterateChildren([&](const FFlecsEntityHandle& ChildEntity)
		{
			if (ChildEntity.Has<FFlecsTestStruct_Tag>())
			{
				bFoundChild = true;
			}
		});

		ASSERT_THAT(IsTrue(bFoundChild));
	}

	TEST_METHOD(B1_CreatePrefabWithRecord_ApplyPrefabToEntity_AddsTagComponent)
	{
		FFlecsEntityRecord Record;
		Record.AddComponent<FFlecsTestStruct_Tag>();

		const FFlecsEntityHandle PrefabEntity = FlecsWorld->CreatePrefabWithRecord(Record);
		ASSERT_THAT(IsTrue(PrefabEntity.IsValid()));
		ASSERT_THAT(IsTrue(PrefabEntity.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPrefab(PrefabEntity);
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.IsA(PrefabEntity)));
	}

	TEST_METHOD(B2_CreatePrefabWithRecord_ApplyPrefabToEntity_AddsScriptStructComponent)
	{
		FFlecsEntityRecord Record;
		Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 321 });

		const FFlecsEntityHandle PrefabEntity = FlecsWorld->CreatePrefabWithRecord(Record);
		ASSERT_THAT(IsTrue(PrefabEntity.IsValid()));
		ASSERT_THAT(IsTrue(PrefabEntity.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPrefab(PrefabEntity);
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 321));
	}

	TEST_METHOD(B3_CreatePrefabWithRecord_ApplyPrefabToEntity_AddsPairComponents)
	{
		FFlecsEntityRecord Record;
		
		FFlecsRecordPair Pair;
		Pair.First = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent>();
		Pair.Second = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent_Second>();
		Pair.PairValueType = EFlecsValuePairType::None;
		Record.AddComponent(MoveTemp(Pair));

		const FFlecsEntityHandle PrefabEntity = FlecsWorld->CreatePrefabWithRecord(Record);
		ASSERT_THAT(IsTrue(PrefabEntity.IsValid()));
		ASSERT_THAT(IsTrue(PrefabEntity.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPrefab(PrefabEntity);
		ASSERT_THAT(IsTrue(TestEntity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));
	}

	TEST_METHOD(B4_CreatePrefabWithRecord_ApplyPrefabToEntity_MultipleComponents)
	{
		FFlecsEntityRecord Record;
		Record.AddComponent<FFlecsTestStruct_Tag>();
		Record.AddComponent<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 654 });
		
		FFlecsRecordPair Pair;
		Pair.First = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent>();
		Pair.Second = FFlecsRecordPairSlot::Make<FUSTRUCTPairTestComponent_Second>();
		Pair.PairValueType = EFlecsValuePairType::None;
		Record.AddComponent(MoveTemp(Pair));

		const FFlecsEntityHandle PrefabEntity = FlecsWorld->CreatePrefabWithRecord(Record);
		ASSERT_THAT(IsTrue(PrefabEntity.IsValid()));
		ASSERT_THAT(IsTrue(PrefabEntity.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPrefab(PrefabEntity);

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		
		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 654));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));
	}

	TEST_METHOD(B5_CreatePrefabWithRecord_ApplyPrefabToEntity_AddsScriptEnum)
	{
		FFlecsEntityRecord Record;
		const FSolidEnumSelector EnumValue = FSolidEnumSelector::Make<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Two);
		Record.AddComponent(EnumValue);

		const FFlecsEntityHandle PrefabEntity = FlecsWorld->CreatePrefabWithRecord(Record);
		ASSERT_THAT(IsTrue(PrefabEntity.IsValid()));
		ASSERT_THAT(IsTrue(PrefabEntity.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPrefab(PrefabEntity);

		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Two)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(),
			static_cast<int64>(EFlecsTestEnum_UENUM::Two))));
	}

	TEST_METHOD(B6_CreatePrefabWithRecord_WithSubEntities_ApplyPrefabToEntity_AddsComponentsToSubEntities)
	{
		FFlecsEntityRecord SubRecord;
		SubRecord.AddComponent<FFlecsTestStruct_Tag>();

		FFlecsEntityRecord Record;
		Record.AddSubEntity(SubRecord);

		const FFlecsEntityHandle PrefabEntity = FlecsWorld->CreatePrefabWithRecord(Record);
		ASSERT_THAT(IsTrue(PrefabEntity.IsValid()));
		ASSERT_THAT(IsTrue(PrefabEntity.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPrefab(PrefabEntity);

		// Should be in the sub-entity
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag>()));

		bool bFoundChild = false;
		TestEntity.IterateChildren([&](const FFlecsEntityHandle& ChildEntity)
		{
			if (ChildEntity.Has<FFlecsTestStruct_Tag>())
			{
				bFoundChild = true;
			}
		});

		ASSERT_THAT(IsTrue(bFoundChild));
	}

	TEST_METHOD(C1_CreateEntityWithRecord_WithNamedEntityRecordFragment)
	{
		FFlecsEntityRecord Record;
		Record.AddFragment<FFlecsNamedEntityRecordFragment>("TestEntityWithRecordFragment");
		Record.AddComponent<FFlecsTestStruct_Tag>();

		const FFlecsEntityHandle Entity = FlecsWorld->CreateEntityWithRecord(Record);
		ASSERT_THAT(IsTrue(Entity.IsValid()));
		ASSERT_THAT(IsTrue(Entity.HasName()));
		ASSERT_THAT(AreEqual(TEXT("TestEntityWithRecordFragment"), Entity.GetName()));
		ASSERT_THAT(IsTrue(Entity.Has<FFlecsTestStruct_Tag>()));
	}
	
}; // End of B1_FlecsEntityRecordTests

#endif // WITH_AUTOMATION_TESTS
