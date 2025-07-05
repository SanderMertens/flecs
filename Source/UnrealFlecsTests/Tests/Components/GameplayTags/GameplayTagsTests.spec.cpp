
#if WITH_AUTOMATION_TESTS

#include "GameplayTagContainer.h"
#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Tests/FlecsTestTypes.h"

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
			const FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);

			TestTrue("Entity has Gameplay Tag",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			
			TestFalse("Entity does not have other Gameplay Tags",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));
		});

		It("Should add multiple Gameplay Tags to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag2);

			TestTrue("Entity has Gameplay Tag 1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			TestTrue("Entity has Gameplay Tag 2",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));
			
			TestFalse("Entity does not have other Gameplay Tags",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag3));
		});

		It("Should remove a Gameplay Tag from an entity", [this]()
		{
			const FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);

			TestTrue("Entity has TestTag1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestTag1);

			TestFalse("Entity does not have TestTag1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
		});

		It("Should remove multiple Gameplay Tags from an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestTag2);

			TestTrue("Entity has Gameplay Tag 1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			TestTrue("Entity has Gameplay Tag 2",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestTag1);
			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestTag2);

			TestFalse("Entity does not have Gameplay Tag 1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag1));
			TestFalse("Entity does not have Gameplay Tag 2",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestTag2));
		});
	});

	Describe("Same Name but different Path Sub Tag Support", [this]()
	{
		It("Should add a Same Sub Gameplay Tag to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);

			TestTrue("Entity has Gameplay Tag",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestFalse("Entity does not have other Gameplay Tags",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));
		});

		It("Should add multiple Same Sub Gameplay Tags to an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2);

			TestTrue("Entity has Gameplay Tag 1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestTrue("Entity has Gameplay Tag 2",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));
		});

		It("Should remove a Same Sub Gameplay Tag from an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);

			TestTrue("Entity has Gameplay Tag", Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);

			TestFalse("Entity does not have Gameplay Tag",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
		});

		It("Should remove multiple Same Sub Gameplay Tags from an entity", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);
			Entity.Add(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2);

			TestTrue("Entity has Gameplay Tag 1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestTrue("Entity has Gameplay Tag 2",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));

			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1);
			Entity.Remove(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2);

			TestFalse("Entity does not have Gameplay Tag 1",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag1));
			TestFalse("Entity does not have Gameplay Tag 2",
				Entity.Has(FFlecsTestNativeGameplayTags::Get().TestSameSubTag2));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
