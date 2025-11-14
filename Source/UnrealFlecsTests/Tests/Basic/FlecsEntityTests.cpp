// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Tests/FlecsTestTypes.h"

/*
 * Layout of Tests:
 * A. General Entity Operation Tests
 * B. Entity Handle/View API Tests
 * C. Entity Hierarchy Tests
 */
TEST_CLASS_WITH_FLAGS(A10_UnrealFlecsComponentRegistrationTests,
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
	
	TEST_METHOD(C1_SpawnEntityWithChildrenInOrder_SetChildOrder_C_API)
	{
		const FFlecsEntityHandle ParentEntity = FlecsWorld->CreateEntity("ParentEntity");

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
		const FFlecsEntityHandle ParentEntity = FlecsWorld->CreateEntity("ParentEntity");

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

		TArray<FFlecsId> NewChildrenEntityOrder = { ChildEntityC.GetFlecsId(), ChildEntityA.GetFlecsId(), ChildEntityB.GetFlecsId() };
		ParentEntity.SetChildOrder(TArrayView<FFlecsId>(NewChildrenEntityOrder));

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
	
}; // End of A10_UnrealFlecsComponentRegistrationTests


#endif // #if WITH_AUTOMATION_TESTS
