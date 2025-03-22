
#if WITH_AUTOMATION_TESTS

#include "GameplayTagContainer.h"
#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

struct FFlecsTestNativeGameplayTags : public FGameplayTagNativeAdder
{
	FGameplayTag TestTag1;
	FGameplayTag TestTag2;
	FGameplayTag TestTag3;

	FGameplayTag TestSameSubTag1;
	FGameplayTag TestSameSubTag2;

	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		TestTag1 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag1"));
		TestTag2 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag2"));
		TestTag3 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag3"));

		TestSameSubTag1 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Sub1.Tag1"));
		TestSameSubTag2 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Sub2.Tag1"));
	}

	FORCEINLINE static const FFlecsTestNativeGameplayTags& Get()
	{
		return StaticInstance;
	}
	
	static FFlecsTestNativeGameplayTags StaticInstance;
};
FFlecsTestNativeGameplayTags FFlecsTestNativeGameplayTags::StaticInstance;

BEGIN_DEFINE_SPEC(FGameplayTagsTestsSpec,
                  "Flecs.Components.GameplayTags",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FGameplayTagsTestsSpec);

void FGameplayTagsTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Adding and Removing Gameplay Tags from Entities", [this]()
	{
		It("Should add a Gameplay Tag to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);

			TestTrue("Entity has Gameplay Tag", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			TestFalse("Entity does not have other Gameplay Tags", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));
		});

		It("Should add multiple Gameplay Tags to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag2);

			TestTrue("Entity has Gameplay Tag 1", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			TestTrue("Entity has Gameplay Tag 2", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));
			TestFalse("Entity does not have other Gameplay Tags", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag3));
		});

		It("Should remove a Gameplay Tag from an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);

			TestTrue("Entity has Gameplay Tag", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestTag1);

			TestFalse("Entity does not have Gameplay Tag", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
		});

		It("Should remove multiple Gameplay Tags from an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag2);

			TestTrue("Entity has Gameplay Tag 1", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			TestTrue("Entity has Gameplay Tag 2", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestTag1);
			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestTag2);

			TestFalse("Entity does not have Gameplay Tag 1", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			TestFalse("Entity does not have Gameplay Tag 2", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));
		});
	});

	Describe("Same Name but different Path Sub Tag Support", [this]()
	{
		It("Should add a Same Sub Gameplay Tag to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);

			TestTrue("Entity has Gameplay Tag", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestFalse("Entity does not have other Gameplay Tags", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));
		});

		It("Should add multiple Same Sub Gameplay Tags to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2);

			TestTrue("Entity has Gameplay Tag 1", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestTrue("Entity has Gameplay Tag 2", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));
		});

		It("Should remove a Same Sub Gameplay Tag from an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);

			TestTrue("Entity has Gameplay Tag", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);

			TestFalse("Entity does not have Gameplay Tag", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
		});

		It("Should remove multiple Same Sub Gameplay Tags from an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2);

			TestTrue("Entity has Gameplay Tag 1", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestTrue("Entity has Gameplay Tag 2", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);
			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2);

			TestFalse("Entity does not have Gameplay Tag 1", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestFalse("Entity does not have Gameplay Tag 2", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
