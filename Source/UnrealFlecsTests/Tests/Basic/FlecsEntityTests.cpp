// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"

/*
 * Layout of Tests:
 * A. General Entity Operation Tests
 * B. Entity Handle/View API Tests
 * C. Entity Hierarchy Tests
 * D. Default Entity Tests
 */
TEST_CLASS_WITH_FLAGS(A10_UnrealFlecsEntityTests,
							   "UnrealFlecs.A10_Entities",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
								| EAutomationTestFlags::CriticalPriority)
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
		Fixture.Reset();
	}

	TEST_METHOD(A1_SpawnEmptyEntity)
	{
		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity();
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));
	}

	TEST_METHOD(A2_SpawnNamedEntity)
	{
		static const FString EntityName = TEXT("MyTestEntity");

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity(EntityName);
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));
		ASSERT_THAT(IsTrue(TestEntity.HasName()));
		ASSERT_THAT(AreEqual(EntityName, TestEntity.GetName()));
	}

	TEST_METHOD(A3_SpawnEntityWithParent_SetParent_API)
	{
		const FFlecsEntityHandle ParentEntity = FlecsWorld->CreateEntity("ParentEntity");
		const FFlecsEntityHandle ChildEntity = FlecsWorld->CreateEntity("ChildEntity")
			.SetParent(ParentEntity);

		ASSERT_THAT(IsTrue(ChildEntity.IsValid()));
		ASSERT_THAT(IsTrue(ChildEntity.HasParent()));
		ASSERT_THAT(AreEqual(ParentEntity, ChildEntity.GetParent<FFlecsEntityHandle>()));
	}
	
	TEST_METHOD(C1_SpawnEntityWithChildrenInOrder_SetChildOrder_C_API)
	{
		const FFlecsEntityHandle ParentEntity = FlecsWorld->CreateEntity("ParentEntity")
			.Add(flecs::OrderedChildren);

		const FFlecsEntityHandle ChildEntityA = FlecsWorld->CreateEntity("ChildEntityA").SetParent(ParentEntity);
		const FFlecsEntityHandle ChildEntityB = FlecsWorld->CreateEntity("ChildEntityB").SetParent(ParentEntity);
		const FFlecsEntityHandle ChildEntityC = FlecsWorld->CreateEntity("ChildEntityC").SetParent(ParentEntity);

		{
			TArray<FFlecsEntityHandle> ChildrenArray;
			ParentEntity.IterateChildren([&ChildrenArray](const FFlecsEntityHandle& InChildEntity)
			{
				ChildrenArray.Add(InChildEntity);
			});

			ASSERT_THAT(IsTrue(ChildrenArray.Num() == 3));
			ASSERT_THAT(AreEqual(ChildrenArray[0], ChildEntityA));
			ASSERT_THAT(AreEqual(ChildrenArray[1], ChildEntityB));
			ASSERT_THAT(AreEqual(ChildrenArray[2], ChildEntityC));
		}

		FFlecsId NewChildrenEntityOrder[] = { ChildEntityC.GetFlecsId(), ChildEntityA.GetFlecsId(), ChildEntityB.GetFlecsId() };
		ParentEntity.SetChildOrder(NewChildrenEntityOrder, 3);

		{
			TArray<FFlecsEntityHandle> ChildrenArray;
			ParentEntity.IterateChildren([&ChildrenArray](const FFlecsEntityHandle& InChildEntity)
			{
				ChildrenArray.Add(InChildEntity);
			});

			ASSERT_THAT(IsTrue(ChildrenArray.Num() == 3));
			ASSERT_THAT(AreEqual(ChildrenArray[0], ChildEntityC));
			ASSERT_THAT(AreEqual(ChildrenArray[1], ChildEntityA));
			ASSERT_THAT(AreEqual(ChildrenArray[2], ChildEntityB));
		}
		
	}

	TEST_METHOD(C2_SpawnEntityWithChildrenInOrder_SetChildOrder_TArrayView_API)
	{
		const FFlecsEntityHandle ParentEntity = FlecsWorld->CreateEntity("ParentEntity")
			.Add(flecs::OrderedChildren);

		const FFlecsEntityHandle ChildEntityA = FlecsWorld->CreateEntity("ChildEntityA").SetParent(ParentEntity);
		const FFlecsEntityHandle ChildEntityB = FlecsWorld->CreateEntity("ChildEntityB").SetParent(ParentEntity);
		const FFlecsEntityHandle ChildEntityC = FlecsWorld->CreateEntity("ChildEntityC").SetParent(ParentEntity);

		{
			TArray<FFlecsEntityHandle> ChildrenArray;
			ParentEntity.IterateChildren([&ChildrenArray](const FFlecsEntityHandle& InChildEntity)
			{
				ChildrenArray.Add(InChildEntity);
			});

			ASSERT_THAT(IsTrue(ChildrenArray.Num() == 3));
			ASSERT_THAT(AreEqual(ChildrenArray[0], ChildEntityA));
			ASSERT_THAT(AreEqual(ChildrenArray[1], ChildEntityB));
			ASSERT_THAT(AreEqual(ChildrenArray[2], ChildEntityC));
		}

		TArray<FFlecsId> NewChildrenEntityOrder = TArray<FFlecsId>{ ChildEntityC.GetFlecsId(), ChildEntityA.GetFlecsId(), ChildEntityB.GetFlecsId() };
		ParentEntity.SetChildOrder(NewChildrenEntityOrder);

		{
			TArray<FFlecsEntityHandle> ChildrenArray;
			ParentEntity.IterateChildren([&ChildrenArray](const FFlecsEntityHandle& InChildEntity)
			{
				ChildrenArray.Add(InChildEntity);
			});

			ASSERT_THAT(IsTrue(ChildrenArray.Num() == 3));
			ASSERT_THAT(AreEqual(ChildrenArray[0], ChildEntityC));
			ASSERT_THAT(AreEqual(ChildrenArray[1], ChildEntityA));
			ASSERT_THAT(AreEqual(ChildrenArray[2], ChildEntityB));
		}
	}

	TEST_METHOD(D1_DefaultEntityAPI_CheckExists)
	{
		ASSERT_THAT(IsTrue(FlecsWorld->IsAlive(TestEntityOption)));

		const FFlecsEntityHandle TestDefaultEntity = FlecsWorld->GetAlive(TestEntityOption);
		ASSERT_THAT(IsTrue(TestDefaultEntity.IsValid()));
	}

	TEST_METHOD(D2_DefaultEntityAPI_GetDefaultEntityWithTrait)
	{
		ASSERT_THAT(IsTrue(FlecsWorld->IsAlive(TestEntityOption2WithTrait)));
		
		const FFlecsEntityHandle TestDefaultEntity = FlecsWorld->GetAlive(TestEntityOption2WithTrait);
		ASSERT_THAT(IsTrue(TestDefaultEntity.IsValid()));
		ASSERT_THAT(IsTrue(TestDefaultEntity.Has(flecs::Trait)));
	}

	TEST_METHOD(D3_DefaultEntityAPI_GetDefaultEntityInNamespace)
	{
		ASSERT_THAT(IsTrue(FlecsWorld->IsAlive(Unreal::Flecs::test::internal::TestEntityOption3InNamespace)));
		
		const FFlecsEntityHandle TestDefaultEntity = FlecsWorld->GetAlive(Unreal::Flecs::test::internal::TestEntityOption3InNamespace);
		ASSERT_THAT(IsTrue(TestDefaultEntity.IsValid()));
	}
	
}; // End of A10_UnrealFlecsComponentRegistrationTests


#endif // #if WITH_AUTOMATION_TESTS
