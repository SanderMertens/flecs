// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"
#include "Worlds/FlecsWorld.h"

/**
 * Layout of the tests:
 * A. Tag Entity Tests
 * B. Add/Remove GameplayTag API Tests
 */
TEST_CLASS_WITH_FLAGS_AND_TAGS(A6_UnrealFlecsGameplayTagTests,
							   "UnrealFlecs.A6_GameplayTags",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
							   | EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Entity][Tag][GameplayTag]")
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

	TEST_METHOD(A1_GameplayTagEntity_GetTagEntityAndTagFromEntity)
	{
		const FGameplayTag TestTag = FFlecsTestNativeGameplayTags::Get().TestTag1;
		ASSERT_THAT(IsTrue(TestTag.IsValid()));

		const FFlecsEntityHandle TagEntity = FlecsWorld->GetTagEntity(TestTag);
		ASSERT_THAT(IsTrue(TagEntity.IsValid()));
		ASSERT_THAT(IsTrue(TagEntity.Has<FGameplayTag>()));

		const FGameplayTag RetrievedTag = TagEntity.Get<FGameplayTag>();
		ASSERT_THAT(IsTrue(RetrievedTag == TestTag));
	}

	TEST_METHOD(B1_GameplayTagAddRemove_OneLevelTag)
	{
		const FGameplayTag TestTag = FFlecsTestNativeGameplayTags::Get().TestTag1;
		ASSERT_THAT(IsTrue(TestTag.IsValid()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		ASSERT_THAT(IsFalse(TestEntity.Has(TestTag)));

		TestEntity.Add(TestTag);
		ASSERT_THAT(IsTrue(TestEntity.Has(TestTag)));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2)));

		TestEntity.Remove(TestTag);
		ASSERT_THAT(IsFalse(TestEntity.Has(TestTag)));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2)));
	}

	TEST_METHOD(B2_GameplayTagAddRemove_TwoLevelTag)
	{
		const FGameplayTag TestTag = FFlecsTestNativeGameplayTags::Get().TestTag2;
		ASSERT_THAT(IsTrue(TestTag.IsValid()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.Add(TestTag);
		ASSERT_THAT(IsTrue(TestEntity.Has(TestTag)));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1)));

		TestEntity.Remove(TestTag);
		ASSERT_THAT(IsFalse(TestEntity.Has(TestTag)));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1)));
	}

	TEST_METHOD(B3_GameplayTagAddRemove_MultipleTags)
	{
		const FGameplayTag TestTag1 = FFlecsTestNativeGameplayTags::Get().TestTag1;
		const FGameplayTag TestTag2 = FFlecsTestNativeGameplayTags::Get().TestTag2;
		ASSERT_THAT(IsTrue(TestTag1.IsValid()));
		ASSERT_THAT(IsTrue(TestTag2.IsValid()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.Add(TestTag1);
		TestEntity.Add(TestTag2);
		ASSERT_THAT(IsTrue(TestEntity.Has(TestTag1)));
		ASSERT_THAT(IsTrue(TestEntity.Has(TestTag2)));

		TestEntity.Remove(TestTag1);
		ASSERT_THAT(IsFalse(TestEntity.Has(TestTag1)));
		ASSERT_THAT(IsTrue(TestEntity.Has(TestTag2)));

		TestEntity.Remove(TestTag2);
		ASSERT_THAT(IsFalse(TestEntity.Has(TestTag1)));
		ASSERT_THAT(IsFalse(TestEntity.Has(TestTag2)));
	}

	TEST_METHOD(B3_GameplayTagAddRemove_Pair_TagRel_ValueTarget)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
		
		const FGameplayTag TestTag1 = FFlecsTestNativeGameplayTags::Get().TestTag1;
		const FGameplayTag TestTag2 = FFlecsTestNativeGameplayTags::Get().TestTag2;
		ASSERT_THAT(IsTrue(TestTag1.IsValid()));
		ASSERT_THAT(IsTrue(TestTag2.IsValid()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.SetPairSecond<FFlecsTestStruct_Value>(TestTag1, FFlecsTestStruct_Value{ 55 });
		ASSERT_THAT(IsTrue(TestEntity.HasPairSecond<FFlecsTestStruct_Value>(TestTag1)));
		
		const FFlecsTestStruct_Value& RetrievedValue = TestEntity.GetPairSecond<FFlecsTestStruct_Value>(TestTag1);
		ASSERT_THAT(IsTrue(RetrievedValue.Value == 55));

		TestEntity.RemovePairSecond<FFlecsTestStruct_Value>(TestTag1);
		ASSERT_THAT(IsFalse(TestEntity.HasPairSecond<FFlecsTestStruct_Value>(TestTag1)));
	}

	TEST_METHOD(B4_GameplayTagAddRemove_Pair_TagRel_TagTarget)
	{
		const FGameplayTag TestTag1 = FFlecsTestNativeGameplayTags::Get().TestTag1;
		const FGameplayTag TestTag2 = FFlecsTestNativeGameplayTags::Get().TestTag2;
		ASSERT_THAT(IsTrue(TestTag1.IsValid()));
		ASSERT_THAT(IsTrue(TestTag2.IsValid()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPair(TestTag1, TestTag2);
		ASSERT_THAT(IsTrue(TestEntity.HasPair(TestTag1, TestTag2)));

		TestEntity.RemovePair(TestTag1, TestTag2);
		ASSERT_THAT(IsFalse(TestEntity.HasPair(TestTag1, TestTag2)));
	}

	TEST_METHOD(B5_GameplayTagAddRemove_Pair_TagRel_EntityTarget)
	{
		const FGameplayTag TestTag1 = FFlecsTestNativeGameplayTags::Get().TestTag1;
		ASSERT_THAT(IsTrue(TestTag1.IsValid()));

		const FFlecsEntityHandle TargetEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TargetEntity.IsValid()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddPair(TestTag1, TargetEntity);
		ASSERT_THAT(IsTrue(TestEntity.HasPair(TestTag1, TargetEntity)));

		TestEntity.RemovePair(TestTag1, TargetEntity);
		ASSERT_THAT(IsFalse(TestEntity.HasPair(TestTag1, TargetEntity)));
	}

	TEST_METHOD(B6_GameplayTagAddRemove_Pair_ValueRel_TagTarget)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
		
		const FGameplayTag TestTag2 = FFlecsTestNativeGameplayTags::Get().TestTag2;
		ASSERT_THAT(IsTrue(TestTag2.IsValid()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.SetPair<FFlecsTestStruct_Value>(TestTag2, FFlecsTestStruct_Value{ 42 });
		ASSERT_THAT(IsTrue(TestEntity.HasPair<FFlecsTestStruct_Value>(TestTag2)));
		
		const FFlecsTestStruct_Value& RetrievedValue = TestEntity.GetPairFirst<FFlecsTestStruct_Value>(TestTag2);
		ASSERT_THAT(IsTrue(RetrievedValue.Value == 42));

		TestEntity.RemovePair<FFlecsTestStruct_Value>(TestTag2);
		ASSERT_THAT(IsFalse(TestEntity.HasPair<FFlecsTestStruct_Value>(TestTag2)));
	}
	
}; // End of A6_UnrealFlecsGameplayTagTests

#endif // #if WITH_AUTOMATION_TESTS

