
#if WITH_AUTOMATION_TESTS

#include "GameplayTagContainer.h"
#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

struct FNativeGameplayTags : public FGameplayTagNativeAdder
{
	FGameplayTag TestTag1;
	FGameplayTag TestTag2;
	FGameplayTag TestTag3;

	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		TestTag1 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag1"));
		TestTag2 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag2"));
		TestTag3 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag3"));
	}

	FORCEINLINE static const FNativeGameplayTags& Get()
	{
		return StaticInstance;
	}
	static FNativeGameplayTags StaticInstance;
};
FNativeGameplayTags FNativeGameplayTags::StaticInstance;

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
			Entity.Add(FNativeGameplayTags::Get().TestTag1);

			TestTrue("Entity has Gameplay Tag", Entity.Has(FNativeGameplayTags::Get().TestTag1));
			TestFalse("Entity does not have other Gameplay Tags", Entity.Has(FNativeGameplayTags::Get().TestTag2));
		});

		It("Should add multiple Gameplay Tags to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FNativeGameplayTags::Get().TestTag1);
			Entity.Add(FNativeGameplayTags::Get().TestTag2);

			TestTrue("Entity has Gameplay Tag 1", Entity.Has(FNativeGameplayTags::Get().TestTag1));
			TestTrue("Entity has Gameplay Tag 2", Entity.Has(FNativeGameplayTags::Get().TestTag2));
			TestFalse("Entity does not have other Gameplay Tags", Entity.Has(FNativeGameplayTags::Get().TestTag3));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
