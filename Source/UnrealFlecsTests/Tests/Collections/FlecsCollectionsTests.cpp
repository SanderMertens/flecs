
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Collections/FlecsCollectionDefinition.h"
#include "Collections/FlecsCollectionWorldSubsystem.h"
#include "Tests/FlecsTestTypes.h"

TEST_CLASS_WITH_FLAGS(B1_CollectionBasicTests, "UnrealFlecs.B1.CollectionsBasic",
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
	
	TEST_METHOD(B1_RegisterCollectionFromDefinition_CreatesPrefabWithTag)
	{
		ASSERT_THAT(IsTrue(IsValid(Collections)));

		FlecsWorld->RegisterComponentType<FFlecsCollectionPrefabTag>();
		
		FFlecsCollectionDefinition Def;
		{
			FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Def);
			Builder.Name("TestCollection_Def");
			Builder.Add<FFlecsTestStruct_Tag>(); // just any simple component
		}
		
		const FFlecsEntityHandle Prefab = Collections->RegisterCollectionDefinition(TEXT("TestCollection_Def"), Def);
		ASSERT_THAT(IsTrue(Prefab.IsValid()));
		ASSERT_THAT(IsTrue(Prefab.Has(flecs::Prefab)));
		ASSERT_THAT(IsTrue(Prefab.Has<FFlecsCollectionPrefabTag>()));
	}
};

#endif // WITH_AUTOMATION_TESTS
