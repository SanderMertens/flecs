// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"

/**
 * Layout of the tests:
 * A. OS API Tests
 * B. World Tests
 * C. Entity Tests
 */
TEST_CLASS_WITH_FLAGS_AND_TAGS(A1_FlecsWorldTests, "UnrealFlecs.A1.World",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
	| EAutomationTestFlags::CriticalPriority,
	"[Flecs][OS-API][World][Entity]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;
	inline static FFlecsEntityHandle TestEntity;

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

	TEST_METHOD_WITH_TAGS(A1_AllocateMemoryOSAPI, "[Flecs][OS-API]")
	{
		static constexpr uint32 MemorySize = 16;
		
		void* Memory = ecs_os_malloc(MemorySize);
		ASSERT_THAT(IsNotNull(Memory));
		
		ecs_os_free(Memory);
		Memory = nullptr;
	}

	TEST_METHOD_WITH_TAGS(A2_ReAllocateMemoryOSAPI, "[Flecs][OS-API]")
	{
		static constexpr uint32 MemorySize = 16;
		
		void* Memory = ecs_os_malloc(MemorySize);
		ASSERT_THAT(IsNotNull(Memory));
		
		void* ReallocatedMemory = ecs_os_realloc(Memory, MemorySize * 2);
		ASSERT_THAT(IsNotNull(ReallocatedMemory));
		
		ecs_os_free(ReallocatedMemory);
	}

	TEST_METHOD_WITH_TAGS(A3_CallocOSAPI, "[Flecs][OS-API]")
	{
		static constexpr uint32 MemorySize = 16;
		
		void* Memory = ecs_os_calloc(MemorySize);
		ASSERT_THAT(IsNotNull(Memory));
		
		const TSolidNotNull<const uint8*> ByteMemory = static_cast<const uint8*>(Memory);
		
		for (uint32 Index = 0; Index < MemorySize; ++Index)
		{
			ASSERT_THAT(AreEqual(ByteMemory[Index], 0));
		}
		
		ecs_os_free(Memory);
	}

	TEST_METHOD_WITH_TAGS(A4_GetTimeNowOSAPI, "[Flecs][OS-API]")
	{
		const uint32_t Time = ecs_os_now();
		ASSERT_THAT(IsTrue(Time > 0));
	}

	TEST_METHOD_WITH_TAGS(A5_SleepNanoSecondsOSAPI, "[Flecs][OS-API]")
	{
		static FTimespan SleepTime = FTimespan::FromMilliseconds(10);
		
		const uint32 StartTime = ecs_os_now();
		ecs_os_sleep(SleepTime.GetSeconds(), 0);
		const uint32 EndTime = ecs_os_now();
		
		ASSERT_THAT(IsTrue(EndTime > StartTime));
	}
	
	TEST_METHOD_WITH_TAGS(B1_CanCreateWorld, "[Flecs][World]")
	{
		ASSERT_THAT(IsTrue(IsValid(FlecsWorld)));
	}

	TEST_METHOD_WITH_TAGS(B2_CanGetWorldEntity, "[Flecs][World]")
	{
		const FFlecsEntityHandle WorldEntity = FlecsWorld->GetWorldEntity();
		ASSERT_THAT(IsTrue(WorldEntity.IsValid()));
		
		ASSERT_THAT(AreEqual(FString("World"), WorldEntity.GetName()));
	}

	TEST_METHOD_WITH_TAGS(B3_CanConvertFlecsWorldToUFlecsWorld, "[Flecs][World]")
	{
		const TSolidNotNull<UFlecsWorld*> ConvertedWorld = Unreal::Flecs::ToFlecsWorld(FlecsWorld->World);
		ASSERT_THAT(IsTrue(IsValid(ConvertedWorld)));
		
		ASSERT_THAT(IsTrue(FlecsWorld == ConvertedWorld));
	}

	TEST_METHOD_WITH_TAGS(C1_CanCreateEntity, "[Flecs][World][Entity]")
	{
		TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));
	}

	TEST_METHOD_WITH_TAGS(C2_CanCreateEntityWithID, "[Flecs][Entity]")
	{
		static constexpr FFlecsId TestId = FLECS_HI_COMPONENT_ID + 10012;
		
		const FFlecsEntityHandle EntityWithId = FlecsWorld->CreateEntityWithId(TestId);
		ASSERT_THAT(AreEqual(TestId, EntityWithId.GetFlecsId()));
		
		ASSERT_THAT(IsTrue(FlecsWorld->IsAlive(EntityWithId)));
		
		ASSERT_THAT(AreEqual(EntityWithId, FlecsWorld->GetEntity(TestId)));
	}

	TEST_METHOD_WITH_TAGS(C3_CanAddRemoveTag, "[Flecs][Entity][Tag][Entity-API]")
	{
		const FFlecsEntityHandle Tag = FlecsWorld->CreateEntity();
		
		TestEntity.Add(Tag);
		ASSERT_THAT(IsTrue(TestEntity.Has(Tag)));
		
		TestEntity.Remove(Tag);
		ASSERT_THAT(IsFalse(TestEntity.Has(Tag)));
	}

	TEST_METHOD_WITH_TAGS(C4_CanCreateNamedEntity, "[Flecs][Entity][Identifier][Entity-Name][Entity-API]")
	{
		static FString EntityName = TEXT("MyTestEntity");
		
		const FFlecsEntityHandle NamedEntity = FlecsWorld->CreateEntity(EntityName);
		ASSERT_THAT(IsTrue(NamedEntity.HasName()));
		ASSERT_THAT(AreEqual(EntityName, NamedEntity.GetName()));

		ASSERT_THAT(AreEqual(
			NamedEntity,
			FlecsWorld->LookupEntity(EntityName)
		));
	}

	TEST_METHOD_WITH_TAGS(C5_CanSetThenClearEntityName, "[Flecs][Entity][Identifier][Entity-Name][Entity-API]")
	{
		static FString NewEntityName = TEXT("MyRenamedTestEntity");
		
		TestEntity.SetName(NewEntityName);
		ASSERT_THAT(AreEqual(NewEntityName, TestEntity.GetName()));
		
		ASSERT_THAT(AreEqual(
			TestEntity,
			FlecsWorld->LookupEntity(NewEntityName)
		));
		
		TestEntity.ClearName();
		ASSERT_THAT(IsFalse(TestEntity.HasName()));

		ASSERT_THAT(IsFalse(FlecsWorld->LookupEntity(NewEntityName).IsValid()));
	}
	
}; // End of A1_UnrealFlecsBasicTests

#endif // #if WITH_AUTOMATION_TESTS