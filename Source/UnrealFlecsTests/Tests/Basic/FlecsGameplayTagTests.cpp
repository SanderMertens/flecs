// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"
#include "Worlds/FlecsWorld.h"

/**
 * Layout of the tests:
 * A. Add/Remove GameplayTag API Tests
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

	TEST_METHOD(A1_GameplayTagAddRemove_OneLevelTag)
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

	TEST_METHOD(A2_GameplayTagAddRemove_TwoLevelTag)
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

	TEST_METHOD(A3_GameplayTagAddRemove_MultipleTags)
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
	
}; // End of A6_UnrealFlecsGameplayTagTests

#endif // #if WITH_AUTOMATION_TESTS

