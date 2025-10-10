// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"

#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldConverter.h"
#include "Worlds/UnrealFlecsWorldTag.h"

/**
 * Layout of the tests:
 * A. OS API Tests
 * B. World Tests
 * C. Entity Tests
 */
TEST_CLASS_WITH_FLAGS_AND_TAGS(A1_FlecsWorldTests, "UnrealFlecs.A1_World",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
	| EAutomationTestFlags::CriticalPriority,
	"[Flecs][OS-API][World][Entity]")
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
		Fixture.Reset();
		FlecsWorld = nullptr;
	}

	TEST_METHOD(A1_AllocateMemoryOSAPI)
	{
		static constexpr uint32 MemorySize = 16;
		
		void* Memory = ecs_os_malloc(MemorySize);
		ASSERT_THAT(IsNotNull(Memory));
		
		ecs_os_free(Memory);
		Memory = nullptr;
	}

	TEST_METHOD(A2_ReAllocateMemoryOSAPI)
	{
		static constexpr uint32 MemorySize = 16;
		
		void* Memory = ecs_os_malloc(MemorySize);
		ASSERT_THAT(IsNotNull(Memory));
		
		void* ReallocatedMemory = ecs_os_realloc(Memory, MemorySize * 2);
		ASSERT_THAT(IsNotNull(ReallocatedMemory));
		
		ecs_os_free(ReallocatedMemory);
	}

	TEST_METHOD(A3_CallocOSAPI)
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

	TEST_METHOD(A4_GetTimeNowOSAPI)
	{
		const uint32_t Time = ecs_os_now();
		ASSERT_THAT(IsTrue(Time > 0));
	}

	TEST_METHOD(A5_SleepNanoSecondsOSAPI)
	{
		static FTimespan SleepTime = FTimespan::FromMilliseconds(10);
		
		const uint32 StartTime = ecs_os_now();
		ecs_os_sleep(SleepTime.GetSeconds(), 0);
		const uint32 EndTime = ecs_os_now();
		
		ASSERT_THAT(IsTrue(EndTime > StartTime));
	}
	
	TEST_METHOD(B1_CanCreateWorld)
	{
		ASSERT_THAT(IsTrue(IsValid(FlecsWorld)));
	}

	TEST_METHOD(B2_CanGetWorldEntity)
	{
		const FFlecsEntityHandle WorldEntity = FlecsWorld->GetWorldEntity();
		ASSERT_THAT(IsTrue(WorldEntity.IsValid()));
		
		ASSERT_THAT(AreEqual(FString("World"), WorldEntity.GetName()));
	}

	TEST_METHOD(B3_CanConvertFlecsWorldToUFlecsWorld)
	{
		const TSolidNotNull<UFlecsWorld*> ConvertedWorld = Unreal::Flecs::ToFlecsWorld(FlecsWorld->World);
		ASSERT_THAT(IsTrue(IsValid(ConvertedWorld)));
		
		ASSERT_THAT(IsTrue(FlecsWorld == ConvertedWorld));
	}

	TEST_METHOD(B4_IsUnrealFlecsWorld)
	{
		ASSERT_THAT(IsTrue(FlecsWorld->HasSingleton<FUnrealFlecsWorldTag>()));

		flecs::world non_unreal_world;
		non_unreal_world.component<FUnrealFlecsWorldTag>();
		ASSERT_THAT(IsFalse(non_unreal_world.has<FUnrealFlecsWorldTag>()));
	}

	TEST_METHOD(C1_CanCreateEntity)
	{
		FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));
	}

	TEST_METHOD(C2_CanCreateEntityWithID)
	{
		static constexpr FFlecsId TestId = FLECS_HI_COMPONENT_ID + 10012;
		
		const FFlecsEntityHandle EntityWithId = FlecsWorld->CreateEntityWithId(TestId);
		ASSERT_THAT(AreEqual(TestId, EntityWithId.GetFlecsId()));
		
		ASSERT_THAT(IsTrue(FlecsWorld->IsAlive(EntityWithId)));
		
		ASSERT_THAT(AreEqual(EntityWithId, FlecsWorld->GetEntity(TestId)));
	}

	TEST_METHOD(C3_CanAddRemoveTag)
	{
		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		const FFlecsEntityHandle Tag = FlecsWorld->CreateEntity();
		
		TestEntity.Add(Tag);
		ASSERT_THAT(IsTrue(TestEntity.Has(Tag)));
		
		TestEntity.Remove(Tag);
		ASSERT_THAT(IsFalse(TestEntity.Has(Tag)));
	}

	TEST_METHOD(C4_CanCreateNamedEntity)
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

	TEST_METHOD(C5_CanSetThenClearEntityName)
	{
		static FString NewEntityName = TEXT("MyRenamedTestEntity");

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		
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
	
}; // End of A1_UnrealFlecsBasicTests*/

#endif // #if WITH_AUTOMATION_TESTS