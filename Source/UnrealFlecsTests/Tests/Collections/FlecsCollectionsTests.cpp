
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "FlecsCollectonTestTypes.h"

#include "Collections/FlecsCollectionDefinition.h"
#include "Collections/FlecsCollectionWorldSubsystem.h"

#include "Tests/FlecsTestTypes.h"

/*
 * Layout of the tests:
 * A. Subsystem Initialization Tests
 * B. Empty Collection Registration Tests
 * C. Collection Registration Type Tests
 * D. Collection Emtpy Instantiation Tests
 * E. Collection Instantiation With Tag Tests
 * F. Collection Parameterized Instantiation Tests
 * G. Collection Instantiation With Sub-Entities Tests
 */
TEST_CLASS_WITH_FLAGS(B4_CollectionBasicTests, "UnrealFlecs.B4_CollectionsBasic",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
			| EAutomationTestFlags::CriticalPriority)
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;
	inline static TObjectPtr<UFlecsCollectionWorldSubsystem> Collections = nullptr;

	BEFORE_EACH()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();
		Collections = FlecsWorld->GetWorld()->GetSubsystem<UFlecsCollectionWorldSubsystem>();
	}
	
	AFTER_EACH()
	{
		Collections = nullptr;
		FlecsWorld = nullptr;
		Fixture.Reset();
	}
	
	TEST_METHOD(A1_SubsystemBootstrapsAndRegistersScope)
	{
		ASSERT_THAT(IsTrue(IsValid(FlecsWorld)));
		ASSERT_THAT(IsTrue(IsValid(Collections)));
		
		const FFlecsEntityHandle Scope = Collections->GetCollectionScope();
		ASSERT_THAT(IsTrue(Scope.IsValid()));
		ASSERT_THAT(IsTrue(Scope.Has(flecs::Module)));
	}

	TEST_METHOD(B1_RegisterEmptyCollection_CreatesPrefab_DefinitionBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollection_Def");
		}
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionDefinition(TEXT("TestCollection_Def"), Def);
		
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("TestCollection_Def")));

		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsFalse(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(B2_RegisterEmptyCollection_CreatesPrefab_CPPBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollection_CPP");
		});

		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("TestCollection_CPP")));

		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsFalse(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(B3_RegisterEmptyCollection_UClassBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();

		// Uses the class name as the collection name
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionClass(UFlecsCollectionTestClassNoInterface::StaticClass(),
			[](FFlecsCollectionBuilder& Builder)
		{

		});

		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("UFlecsCollectionTestClassNoInterface")));

		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsFalse(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(B4_RegisterEmptyCollection_CreatesPrefab_ClassBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();

		// Uses the class name as the collection name
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionClass(UFlecsCollectionTestClassNoInterface::StaticClass(),
			[](FFlecsCollectionBuilder& Builder)
		{

		});
		
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("UFlecsCollectionTestClassNoInterface")));

		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsFalse(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}
	
	TEST_METHOD(C1_RegisterCollectionFromDefinition_CreatesPrefabWithTag_DefinitionBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollection_Def")
				.Add<FFlecsTestStruct_Tag_Inherited>();
		}
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionDefinition(TEXT("TestCollection_Def"), Def);
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(C2_RegisterCollectionFromDefinition_CreatesPrefabWithTag_CPPBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollection_CPP")
				.Add<FFlecsTestStruct_Tag_Inherited>();
		});

		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(C3_RegisterCollectionFromClass_CreatesPrefabWithTag_ClassBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();

		// Uses the class name as the collection name
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionClass(UFlecsCollectionTestClassNoInterface::StaticClass(),
			[](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Add<FFlecsTestStruct_Tag_Inherited>();
		});
		
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	// Uses the class name as the collection name
	TEST_METHOD(C4_RegisterCollectionFromClass_CreatesPrefabWithTag_UClassInterfaceAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();

		// Uses the class name as the collection name
		const FFlecsEntityHandle Prefab
			= Collections->RegisterCollectionInterfaceClass(UFlecsCollectionTestClassWithInterface_Inherited::StaticClass());
		
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("UFlecsCollectionTestClassWithInterface_Inherited")));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	// Uses the class name as the collection name
	TEST_METHOD(C5_RegisterCollectionFromClass_CreatesPrefabWithTag_TypedCPPInterfaceAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();

		// Uses the class name as the collection name
		const FFlecsEntityHandle Prefab
			= Collections->RegisterTypedInterfaceCollection<UFlecsCollectionTestClassWithInterface_Inherited>();
		
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("UFlecsCollectionTestClassWithInterface_Inherited")));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(C6_RegisterCollectionFromDefinition_CreatesPrefabWithSubEntity_DefinitionAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollectionWithSubEntities_Def")
				.Add<FFlecsTestStruct_Tag_Inherited>();

			Builder.BeginSubEntity()
				.Add<FFlecsTestStruct_Tag_Inherited>()
			.EndSubEntity();
		}

		const FFlecsEntityHandle Prefab
			= Collections->RegisterCollectionDefinition(TEXT("TestCollectionWithSubEntities_Def"), Def);

		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("TestCollectionWithSubEntities_Def")));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));

		bool bFoundSubEntity = false;
		Prefab.IterateChildren([&](const FFlecsEntityHandle& ChildEntity)
		{
			if (ChildEntity.Has<FFlecsTestStruct_Tag_Inherited>())
			{
				bFoundSubEntity = true;
			}
		});

		ASSERT_THAT(IsTrue(bFoundSubEntity));
	}

	TEST_METHOD(C7_RegisterCollectionFromDefinition_CreatesPrefabWithSubEntity_WithName_DefinitionAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollectionWithSubEntities_Def")
				.Add<FFlecsTestStruct_Tag_Inherited>();

			Builder.BeginSubEntity("SubEntity1")
				.Add<FFlecsTestStruct_Tag_Inherited>()
			.EndSubEntity();
		}

		const FFlecsEntityHandle Prefab
			= Collections->RegisterCollectionDefinition(TEXT("TestCollectionWithSubEntities_Def"), Def);

		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("TestCollectionWithSubEntities_Def")));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsTestStruct_Tag_Inherited>()));

		const FFlecsEntityView SubEntity1View = Prefab.Lookup<FFlecsEntityView>("SubEntity1");
		ASSERT_THAT(IsTrue(SubEntity1View.IsValid()));
		ASSERT_THAT(IsTrue(SubEntity1View.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(D1_InstantiateEmptyCollection_CreatesEntityFromPrefab_BuilderAPI)
	{
		const FFlecsEntityHandle CollectionPrefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollectionToInstantiate");
		});

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
	}

	TEST_METHOD(D2_InstantiateEmptyCollection_CreatesEntityFromPrefab_DefinitionAPI)
	{
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollectionToInstantiate_Def");
		}
		
		const FFlecsEntityHandle CollectionPrefab
			= Collections->RegisterCollectionDefinition(TEXT("TestCollectionToInstantiate_Def"), Def);
		
		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
	}

	TEST_METHOD(D3_InstantiateEmptyCollection_CreatesEntityFromPrefab_ClassAPI)
	{
		const FFlecsEntityHandle CollectionPrefab = Collections->RegisterCollectionClass(UFlecsCollectionTestClassNoInterface::StaticClass(),
			[](FFlecsCollectionBuilder& Builder)
		{
			// No-op
		});

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(UFlecsCollectionTestClassNoInterface::StaticClass())));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection<UFlecsCollectionTestClassNoInterface>()));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(UFlecsCollectionTestClassNoInterface::StaticClass())));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection<UFlecsCollectionTestClassNoInterface>()));
	}

	TEST_METHOD(E1_InstantiateCollection_Inherited_CreatesEntityFromPrefab_InterfaceClassAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		const FFlecsEntityHandle CollectionPrefab
			= Collections->RegisterCollectionInterfaceClass(UFlecsCollectionTestClassWithInterface_Inherited::StaticClass());

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(UFlecsCollectionTestClassWithInterface_Inherited::StaticClass())));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection<UFlecsCollectionTestClassWithInterface_Inherited>()));
		
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(UFlecsCollectionTestClassWithInterface_Inherited::StaticClass())));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection<UFlecsCollectionTestClassWithInterface_Inherited>()));
		
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(E2_InstantiateCollection_Inherited_CreatesEntityFromPrefab_TypedCPPInterfaceAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		const FFlecsEntityHandle CollectionPrefab
			= Collections->RegisterTypedInterfaceCollection<UFlecsCollectionTestClassWithInterface_Inherited>();

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(UFlecsCollectionTestClassWithInterface_Inherited::StaticClass())));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection<UFlecsCollectionTestClassWithInterface_Inherited>()));
		
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(UFlecsCollectionTestClassWithInterface_Inherited::StaticClass())));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection<UFlecsCollectionTestClassWithInterface_Inherited>()));

		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(E3_InstantiateCollection_Inherited_CreatesEntityFromPrefab_DefinitionAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollectionToInstantiate_Def")
				.Add<FFlecsTestStruct_Tag_Inherited>();
		}
		
		const FFlecsEntityHandle CollectionPrefab
			= Collections->RegisterCollectionDefinition(TEXT("TestCollectionToInstantiate_Def"), Def);
		
		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(E4_InstantiateCollection_Inherited_CreatesEntityFromPrefab_BuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		const FFlecsEntityHandle CollectionPrefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollectionToInstantiate")
				.Add<FFlecsTestStruct_Tag_Inherited>();
		});

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(E5_InstantiateCollection_Inherited_CreatesEntityFromPrefab_ClassBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag_Inherited>();
		
		const FFlecsEntityHandle CollectionPrefab = Collections->RegisterCollectionClass(UFlecsCollectionTestClassNoInterface::StaticClass(),
			[](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Add<FFlecsTestStruct_Tag_Inherited>();
		});

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(UFlecsCollectionTestClassNoInterface::StaticClass())));
		ASSERT_THAT(IsTrue(TestEntity.HasCollection<UFlecsCollectionTestClassNoInterface>()));
		
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));

		TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection(UFlecsCollectionTestClassNoInterface::StaticClass())));
		ASSERT_THAT(IsFalse(TestEntity.HasCollection<UFlecsCollectionTestClassNoInterface>()));
		
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag_Inherited>()));
	}

	TEST_METHOD(F1_InstantiateParameterizedCollection_CPPBuilderAPI_WithDefaultParameters_DefaultValue_InstancedStructAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();

		const FFlecsEntityHandle CollectionPrefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollection_Parameterized")
				.Add<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 33 })
				.Add<FFlecsTestStruct_Tag>()
				.Parameters(FInstancedStruct::Make<FFlecsTestStruct_Value>({ 33 }),
					[](const FFlecsEntityHandle& InCollectionEntity, const FInstancedStruct& InParams)
				{
					InCollectionEntity.Assign<FFlecsTestStruct_Value>(InParams.Get<FFlecsTestStruct_Value>());
				});
		});

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity_Parameterized_Default");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));

		// Verify the value was set from the default params
		const FFlecsTestStruct_Value& Applied = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Applied.Value == 33));

		// Cleanup
		/*TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));*/
	}

	TEST_METHOD(F2_InstantiateParameterizedCollection_CPPBuilderAPI_WithExplicitParameters_AppliesValue_InstancedStructAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();

		const FFlecsEntityHandle CollectionPrefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollection_Parameterized")
				.Add<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 33 })
				.Add<FFlecsTestStruct_Tag>()
				.Parameters<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 33 },
					[](const FFlecsEntityHandle& InCollectionEntity, const FFlecsTestStruct_Value& InParams)
				{
					InCollectionEntity.Assign<FFlecsTestStruct_Value>(InParams);
				});
		});

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity_Parameterized_Explicit");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab, FInstancedStruct::Make<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{99}));

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));

		// Verify the value was set from the explicit params
		const FFlecsTestStruct_Value& Applied = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Applied.Value == 99));

		// Cleanup
		/*TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));*/
	}

	TEST_METHOD(F3_InstantiateParameterizedCollection_ClassBuilderAPI_WithExplicitParameters_DefaultValue_InstancedStructAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();

		const FFlecsEntityHandle CollectionPrefab
			= Collections->RegisterCollectionInterfaceClass(UFlecsCollectionTestClassWithInterface_Parameterized::StaticClass());

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity_Parameterized_Explicit");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));

		// Verify the value was set from the explicit params
		const FFlecsTestStruct_Value& Applied = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Applied.Value == 33));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));

		// Cleanup
		/*TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));*/
	}

	TEST_METHOD(F4_InstantiateParameterizedCollection_ClassBuilderAPI_WithExplicitParameters_AppliesValue_InstancedStructAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();

		const FFlecsEntityHandle CollectionPrefab
			= Collections->RegisterCollectionInterfaceClass(UFlecsCollectionTestClassWithInterface_Parameterized::StaticClass());

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity_Parameterized_Explicit");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab, FInstancedStruct::Make<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{99}));

		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));

		// Verify the value was set from the explicit params
		const FFlecsTestStruct_Value& Applied = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Applied.Value == 99));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));

		// Cleanup
		/*TestEntity.RemoveCollection(CollectionPrefab);
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));*/
	}

	TEST_METHOD(G1_InstantiateCollectionWithSubEntities_CreatesEntityWithSubEnitities_BuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();

		const FFlecsEntityHandle CollectionPrefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollection_WithSubEntities")
				.BeginSubEntity("SubEntity1")
					.Add<FFlecsTestStruct_Tag>()
				.EndSubEntity();
		});

		ASSERT_THAT(IsTrue(CollectionPrefab.IsValid()));
		ASSERT_THAT(IsTrue(CollectionPrefab.Has(flecs::Prefab)));

		const FFlecsEntityHandle TestEntity = FlecsWorld->CreateEntity("TestEntity_WithSubEntities");
		ASSERT_THAT(IsTrue(TestEntity.IsValid()));

		TestEntity.AddCollection(CollectionPrefab);
		ASSERT_THAT(IsTrue(TestEntity.HasCollection(CollectionPrefab)));
		ASSERT_THAT(IsTrue(TestEntity.Lookup<FFlecsEntityView>("SubEntity1").IsValid()));
		ASSERT_THAT(IsTrue(TestEntity.Lookup<FFlecsEntityView>("SubEntity1").Has<FFlecsTestStruct_Tag>()));
	}

	
}; // End of B4_CollectionBasicTests

#endif // WITH_AUTOMATION_TESTS
