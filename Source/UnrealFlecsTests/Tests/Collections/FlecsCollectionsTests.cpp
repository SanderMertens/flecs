
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Collections/FlecsCollectionDefinition.h"
#include "Collections/FlecsCollectionWorldSubsystem.h"
#include "Tests/FlecsTestTypes.h"

/*
 * Layout of the tests:
 * A. Subsystem Initialization Tests
 * B. Empty Collection Registration Tests
 * C. Collection Registration Type Tests
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
		ASSERT_THAT(IsTrue(IsValid(Collections)));

		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollection_Def");
		}
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionDefinition(TEXT("TestCollection_Def"), Def);
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("TestCollection_Def")));
	}

	TEST_METHOD(B2_RegisterEmptyCollection_CreatesPrefab_CPPBuilderAPI)
	{
		ASSERT_THAT(IsTrue(IsValid(Collections)));
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionBuilder([](FFlecsCollectionBuilder& Builder)
		{
			Builder
				.Name("TestCollection_CPP");
		});

		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.GetName() == TEXT("TestCollection_CPP")));
	}
	
	TEST_METHOD(C1_RegisterCollectionFromDefinition_CreatesPrefabWithTag_DefinitionBuilderAPI)
	{
		FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def)
				.Name("TestCollection_Def")
				.Add<FFlecsTestStruct_Tag>();
		}
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionDefinition(TEXT("TestCollection_Def"), Def);
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
	}
};

#endif // WITH_AUTOMATION_TESTS
