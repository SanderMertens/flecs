// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#if WITH_AUTOMATION_TESTS

#include "CQTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Misc/AutomationTest.h"
#include "Tests/FlecsTestTypes.h"

/** Layout of the tests:
 * A. Enum Add/Remove/Replace Tests
 */
TEST_CLASS_WITH_FLAGS_AND_TAGS(A5_UnrealFlecsEnumRegistrationTests,
                               "UnrealFlecs.A5.Enums.Registration",
                               EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter
                               | EAutomationTestFlags::CriticalPriority,
                               "[Flecs][Enum][CPP-API]")
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

	TEST_METHOD_WITH_TAGS(A1_RegisterStaticEnum_AddRemoveStaticEnumAPI,
								   "[Flecs][Component][Enum][StaticEnum-API]")
	{
		const FFlecsComponentHandle TestEnumEntity = FlecsWorld->RegisterComponentType(StaticEnum<EFlecsTestEnum_UENUM>());
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

		TestEntity.Remove(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsFalse(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
	}

	TEST_METHOD_WITH_TAGS(A2_RegisterCPPAPI_AddRemoveCPPAPI,
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

		TestEntity.Remove<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>())));
		ASSERT_THAT(IsFalse(TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard)));
		ASSERT_THAT(IsFalse(TestEntity.Has<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One)));
		ASSERT_THAT(IsFalse(TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One))));
	}

}; // End of A5_UnrealFlecsEnumRegistrationTests

TEST_CLASS_WITH_FLAGS_AND_TAGS(A6_UnrealFlecsEnumTests,
							   "UnrealFlecs.A5.Enums",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter
							   | EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Enum][CPP-API]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsComponentHandle EnumComponentEntity;

	BEFORE_ALL()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();
	}

	AFTER_ALL()
	{
		Fixture.Reset();
		FlecsWorld = nullptr;
	}

}; // End of A6_UnrealFlecsBasicEnumCPPTests

#endif // #if WITH_AUTOMATION_TESTS
