// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FEntityBasicTestsSpec,
                  "Flecs.Entity.Basic",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEntityBasicTestsSpec);

void FEntityBasicTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Entity Creation and Deletion", [this]()
	{
		It("Should create and delete an entity", [this]()
		{
			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity();
			TestTrueExpr(EntityHandle.IsValid());
		});

		It("Should create and delete an entity with a name", [this]()
		{
			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			TestTrue("Entity should be valid", EntityHandle.IsValid());
			TestEqual("Entity name should be TestEntity",
				EntityHandle.GetName(), TEXT("TestEntity"));
			
			EntityHandle.Destroy();
			TestFalseExpr(EntityHandle.IsValid());
		});

		It("Should create an entity with a specific ID", [this]()
		{
			constexpr FFlecsId EntityId = 12345;
			
			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntityWithId(EntityId);
			TestTrue("Entity ID should be 12345", EntityHandle == EntityId);
		});

		It("Should create an entity and destroy it with a specific Id", [this]()
		{
			constexpr FFlecsId EntityId = 12345;
			
			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntityWithId(EntityId);
			TestTrue("Entity should be valid", EntityHandle.IsValid());
			TestTrue("Entity ID should be 12345", EntityHandle == EntityId);
			
			EntityHandle.Destroy();
			TestFalseExpr(EntityHandle.IsValid());
		});
	});

	Describe("Entity Hierarchies", [this]()
	{
		It("Should create a child entity", [this]()
		{
			const FFlecsEntityHandle ParentEntity = Fixture.FlecsWorld->CreateEntity();
			const FFlecsEntityHandle ChildEntity = Fixture.FlecsWorld->CreateEntity();
			ChildEntity.SetParent(ParentEntity);
			
			TestTrue("Child entity should be valid", ChildEntity.IsValid());
			TestTrue("Parent entity should be valid", ParentEntity.IsValid());
			TestTrueExpr(ChildEntity.HasParent());
			
			TestEqual("Child entity should have a parent",
				ChildEntity.GetParent(), ParentEntity);
		});

		It("Should create a child entity and then destroy the parent", [this]()
		{
			const FFlecsEntityHandle ParentEntity = Fixture.FlecsWorld->CreateEntity();
			const FFlecsEntityHandle ChildEntity = Fixture.FlecsWorld->CreateEntity();
			ChildEntity.SetParent(ParentEntity);
			
			TestTrue("Child entity should be valid", ChildEntity.IsValid());
			TestTrue("Parent entity should be valid", ParentEntity.IsValid());
			TestEqual("Child entity should have a parent", ChildEntity.GetParent(), ParentEntity);
			
			ParentEntity.Destroy();
			TestFalse("Parent entity should be invalid", ParentEntity.IsValid());
			TestFalse("Child entity should be invalid", ChildEntity.IsValid());
		});

		It("Should create a path of child entities", [this]()
		{
			const FFlecsEntityHandle ParentEntity = Fixture.FlecsWorld->CreateEntity("Parent");
			const FFlecsEntityHandle ChildEntity1 = Fixture.FlecsWorld->CreateEntity("Child1");
			const FFlecsEntityHandle ChildEntity2 = Fixture.FlecsWorld->CreateEntity("Child2");
			const FFlecsEntityHandle ChildEntity3 = Fixture.FlecsWorld->CreateEntity("Child3");

			ChildEntity1.SetParent(ParentEntity);
			ChildEntity2.SetParent(ChildEntity1);
			ChildEntity3.SetParent(ChildEntity2);

			TestTrue("Parent entity should be valid", ParentEntity.IsValid());
			TestTrue("Child entity 1 should be valid", ChildEntity1.IsValid());
			TestTrue("Child entity 2 should be valid", ChildEntity2.IsValid());
			TestTrue("Child entity 3 should be valid", ChildEntity3.IsValid());
			
			TestEqual("Child entity 1 should have a parent", ChildEntity1.GetParent(), ParentEntity);
			TestEqual("Child entity 2 should have a parent", ChildEntity2.GetParent(), ChildEntity1);
			TestEqual("Child entity 3 should have a parent", ChildEntity3.GetParent(), ChildEntity2);

			TestEqual("Child entity 1 should have a depth of 1", ChildEntity1.GetDepth(flecs::ChildOf), 1);
			TestEqual("Child entity 2 should have a depth of 2", ChildEntity2.GetDepth(flecs::ChildOf), 2);
			TestEqual("Child entity 3 should have a depth of 3", ChildEntity3.GetDepth(flecs::ChildOf), 3);
		});
	});

	Describe("Entity Naming", [this]()
	{
		It("Should create an entity with a name", [this]()
		{
			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			
			TestTrue("Entity should be valid", EntityHandle.IsValid());
			TestEqual("Entity name should be TestEntity", EntityHandle.GetName(), TEXT("TestEntity"));
		});

		It("Should create an entity with a name and change it", [this]()
		{
			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			TestTrue("Entity should be valid", EntityHandle.IsValid());
			TestEqual("Entity name should be TestEntity", EntityHandle.GetName(), TEXT("TestEntity"));
			EntityHandle.SetName(TEXT("NewName"));
			TestEqual("Entity name should be NewName", EntityHandle.GetName(), TEXT("NewName"));
		});

		It("Should create a path of entities using the '::' Separator", [this]()
		{
			const FFlecsEntityHandle ParentEntity = Fixture.FlecsWorld->CreateEntity("Parent");
			const FFlecsEntityHandle ChildEntity1 = Fixture.FlecsWorld->CreateEntity("Child1");
			ChildEntity1.SetParent(ParentEntity);
			const FFlecsEntityHandle ChildEntity2 = Fixture.FlecsWorld->CreateEntity("Child2");
			ChildEntity2.SetParent(ChildEntity1);
			const FFlecsEntityHandle ChildEntity3 = Fixture.FlecsWorld->CreateEntity("Child3");
			ChildEntity3.SetParent(ChildEntity2);

			TestTrue("Parent entity should be valid", Fixture.FlecsWorld->LookupEntity("Parent").IsValid());
			TestTrue("Child entity 1 under parent scope should be valid", Fixture.FlecsWorld->LookupEntity("Parent::Child1").IsValid());
			TestFalse("Child entity 1 should not be valid", Fixture.FlecsWorld->LookupEntity("Child1").IsValid());
			TestTrue("Child entity 2 under child 1 scope should be valid", Fixture.FlecsWorld->LookupEntity("Parent::Child1::Child2").IsValid());
			TestFalse("Child entity 2 should not be valid", Fixture.FlecsWorld->LookupEntity("Child2").IsValid());
		});
	});

	Describe("Entity Serialization", [this]()
	{
		It("Should serialize and deserialize an entity with one basic component", [this]()
		{
			struct FTestComponent
			{
				int32 Value;
			};

			Fixture.FlecsWorld->RegisterComponentType<FTestComponent>()
				.AddMember<int32>("Value");

			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			EntityHandle.Set<FTestComponent>({ 42 });
			const FString SerializedEntity = EntityHandle.ToJson();
			EntityHandle.Destroy();
			
			TestTrue("Serialized entity should not be empty", !SerializedEntity.IsEmpty());

			const FFlecsEntityHandle DeserializedEntity = Fixture.FlecsWorld->CreateEntity();
			
			DeserializedEntity.FromJson(SerializedEntity);
			
			TestTrue("Deserialized entity should be valid", DeserializedEntity.IsValid());
			TestEqual("Deserialized entity value should be 42",
				DeserializedEntity.Get<FTestComponent>().Value, 42);
		});
	});

	Describe("Entity Pairs", [this]()
	{
		It("Should create an entity with a pair", [this]()
		{
			const FFlecsEntityHandle FirstEntity = Fixture.FlecsWorld->CreateEntity(TEXT("FirstEntity"));
			const FFlecsEntityHandle SecondEntity = Fixture.FlecsWorld->CreateEntity(TEXT("SecondEntity"));

			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			
			EntityHandle.AddPair(FirstEntity, SecondEntity);
			TestTrue("Entity should have a pair", EntityHandle.HasPair(FirstEntity, SecondEntity));
		});

		It("Should create an entity with a pair and then remove it", [this]()
		{
			const FFlecsEntityHandle FirstEntity = Fixture.FlecsWorld->CreateEntity(TEXT("FirstEntity"));
			const FFlecsEntityHandle SecondEntity = Fixture.FlecsWorld->CreateEntity(TEXT("SecondEntity"));

			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			
			EntityHandle.AddPair(FirstEntity, SecondEntity);
			TestTrue("Entity should have a pair", EntityHandle.HasPair(FirstEntity, SecondEntity));
			
			EntityHandle.RemovePair(FirstEntity, SecondEntity);
			TestFalse("Entity should not have a pair", EntityHandle.HasPair(FirstEntity, SecondEntity));
		});

		It("Should create an entity with multiple pairs", [this]()
		{
			const FFlecsEntityHandle FirstEntity = Fixture.FlecsWorld->CreateEntity(TEXT("FirstEntity"));
			const FFlecsEntityHandle SecondEntity = Fixture.FlecsWorld->CreateEntity(TEXT("SecondEntity"));
			const FFlecsEntityHandle ThirdEntity = Fixture.FlecsWorld->CreateEntity(TEXT("ThirdEntity"));

			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			
			EntityHandle.AddPair(FirstEntity, SecondEntity);
			EntityHandle.AddPair(FirstEntity, ThirdEntity);
			
			TestTrue("Entity should have a pair with First and Second", EntityHandle.HasPair(FirstEntity, SecondEntity));
			TestTrue("Entity should have a pair with First and Third", EntityHandle.HasPair(FirstEntity, ThirdEntity));
		});

		It("Should create an entity with a pair that has a component in the first of the pair (with data)",
			[this]()
		{
			struct FTestPairComponent
			{
				int32 Value;
			};

			Fixture.FlecsWorld->RegisterComponentType<FTestPairComponent>()
				.AddMember<int32>("Value");

			const FFlecsEntityHandle SecondEntity = Fixture.FlecsWorld->CreateEntity(TEXT("SecondEntity"));

			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			
			EntityHandle.SetPairFirst<FTestPairComponent>(SecondEntity, { .Value = 100 });

			TestTrueExpr(EntityHandle.HasPair<FTestPairComponent>(SecondEntity));
			TestFalseExpr(EntityHandle.HasPairSecond<FTestPairComponent>(SecondEntity));
			TestEqual("Entity pair component value should be 100", 
				EntityHandle.GetPairFirst<FTestPairComponent>(SecondEntity).Value, 100);
		});

		It("Should create an entity with a pair that has a component in the second of the pair (with data)",
			[this]()
		{
			struct FTestPairComponent
			{
				int32 Value;
			};

			Fixture.FlecsWorld->RegisterComponentType<FTestPairComponent>()
				.AddMember<int32>("Value");

			const FFlecsEntityHandle FirstEntity = Fixture.FlecsWorld->CreateEntity(TEXT("FirstEntity"));

			const FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity(TEXT("TestEntity"));
			
			EntityHandle.SetPairSecond<FTestPairComponent>(FirstEntity, { .Value = 200 });

			TestTrueExpr(EntityHandle.HasPairSecond<FTestPairComponent>(FirstEntity));
			TestFalseExpr(EntityHandle.HasPair<FTestPairComponent>(FirstEntity));
			TestEqual("Entity pair component value should be 200", 
				EntityHandle.GetPairSecond<FTestPairComponent>(FirstEntity).Value, 200);
		});
		
	});
}

#endif // WITH_AUTOMATION_TESTS
