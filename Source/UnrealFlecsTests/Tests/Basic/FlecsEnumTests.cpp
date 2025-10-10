// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"
#include "Worlds/FlecsWorld.h"

/** Layout of the tests:
 * A. Enum Register Add/Remove/Replace API Tests
 * B. Sparse Enum Register Add/Remove/Replace API Tests
 */
TEST_CLASS_WITH_FLAGS_AND_TAGS(A5_UnrealFlecsEnumRegistrationTests,
                               "UnrealFlecs.A5_Enums.Registration",
                               EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
                               | EAutomationTestFlags::CriticalPriority,
                               "[Flecs][Component][Pair][Enum][CPP-API][StaticEnum-API]")
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

	TEST_METHOD_WITH_TAGS(A1_RegisterStaticEnumAPI_AddRemoveReplaceStaticEnumAPI,
								   "[Flecs][Component][Enum][StaticEnum-API]")
	{
		const FFlecsComponentHandle TestEnumEntity = FlecsWorld->RegisterComponentType(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(TestEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(TestEnumEntity.IsEnum()));
		
		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		// Add the enum value using StaticEnum API
		TestEntity.Add(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<uint64>(EFlecsTestEnum_UENUM::One));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));

		const EFlecsTestEnum_UENUM CPPEnumValue = TestEntity.GetEnumValue<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(AreEqual(CPPEnumValue, EFlecsTestEnum_UENUM::One));

		const EFlecsTestEnum_UENUM StaticEnumValue = static_cast<EFlecsTestEnum_UENUM>(
			TestEntity.GetEnumValue(StaticEnum<EFlecsTestEnum_UENUM>()));
		ASSERT_THAT(AreEqual(StaticEnumValue, EFlecsTestEnum_UENUM::One));

		// Replace the enum value with a different one
		TestEntity.Add(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<uint64>(EFlecsTestEnum_UENUM::Three));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Three)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::Three))));
		
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
		
		// Remove the enum value using StaticEnum API
		TestEntity.Remove(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsFalse(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
	}

	TEST_METHOD_WITH_TAGS(A2_RegisterCPPAPI_AddRemoveReplaceCPPAPI,
								   "[Flecs][Component][Enum][CPP-API]")
	{
		const FFlecsComponentHandle TestEnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(TestEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(TestEnumEntity.IsEnum()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.Add<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One);
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));

		const EFlecsTestEnum_UENUM CPPEnumValue = TestEntity.GetEnumValue<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(AreEqual(CPPEnumValue, EFlecsTestEnum_UENUM::One));
		const EFlecsTestEnum_UENUM StaticEnumValue = static_cast<EFlecsTestEnum_UENUM>(
			TestEntity.GetEnumValue(StaticEnum<EFlecsTestEnum_UENUM>()));
		ASSERT_THAT(AreEqual(StaticEnumValue, EFlecsTestEnum_UENUM::One));

		TestEntity.Add<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Three);
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Three)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::Three))));
		
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
		
		TestEntity.Remove<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsFalse(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
	}

	TEST_METHOD_WITH_TAGS(A3_RegisterStaticEnumAPI_Add_CPPAPI_Remove_StaticEnumAPI_Replace_StaticEnumAPI,
			"[Flecs][Component][Enum][StaticEnum-API]")
	{ 
		const FFlecsComponentHandle TestEnumEntity = FlecsWorld->RegisterComponentType(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(TestEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(TestEnumEntity.IsEnum()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.Add<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One);
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));

		const EFlecsTestEnum_UENUM CPPEnumValue = TestEntity.GetEnumValue<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(AreEqual(CPPEnumValue, EFlecsTestEnum_UENUM::One));
		const EFlecsTestEnum_UENUM StaticEnumValue = static_cast<EFlecsTestEnum_UENUM>(
			TestEntity.GetEnumValue(StaticEnum<EFlecsTestEnum_UENUM>()));
		ASSERT_THAT(AreEqual(StaticEnumValue, EFlecsTestEnum_UENUM::One));

		TestEntity.Add(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<uint64>(EFlecsTestEnum_UENUM::Three));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Three)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::Three))));
		
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
		
		TestEntity.Remove(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsFalse(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
	}

	TEST_METHOD_WITH_TAGS(A4_RegisterCPPAPI_AddRemoveStaticEnumAPI_Replace_StaticCPPAPI,
			"[Flecs][Component][Enum][CPP-API]")
	{
		const FFlecsComponentHandle TestEnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(TestEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(TestEnumEntity.IsEnum()));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.Add(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<uint64>(EFlecsTestEnum_UENUM::One));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));

		const EFlecsTestEnum_UENUM CPPEnumValue = TestEntity.GetEnumValue<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(AreEqual(CPPEnumValue, EFlecsTestEnum_UENUM::One));
		const EFlecsTestEnum_UENUM StaticEnumValue = static_cast<EFlecsTestEnum_UENUM>(
			TestEntity.GetEnumValue(StaticEnum<EFlecsTestEnum_UENUM>()));
		ASSERT_THAT(AreEqual(StaticEnumValue, EFlecsTestEnum_UENUM::One));

		TestEntity.Add<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Three);
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsTrue(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsTrue(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::Three)));
		ASSERT_THAT(IsTrue(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::Three))));
		
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));

		TestEntity.Remove<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsFalse(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
	}

}; // End of A5_UnrealFlecsEnumRegistrationTests

TEST_CLASS_WITH_FLAGS_AND_TAGS(A6_UnrealFlecsEnumTests,
							   "UnrealFlecs.A5.Enums",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
							   | EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Component][Pair][Enum][CPP-API][StaticEnum-API][Entity-API]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsComponentHandle EnumComponentEntity;
	inline static FFlecsEntityHandle SparseEnumEntity;

	BEFORE_EACH()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();

		EnumComponentEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		SparseEnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_SparseUENUM>();
	}

	AFTER_EACH()
	{
		Fixture.Reset();
		FlecsWorld = nullptr;
	}

	

}; // End of A6_UnrealFlecsBasicEnumCPPTests

#endif // #if WITH_AUTOMATION_TESTS
