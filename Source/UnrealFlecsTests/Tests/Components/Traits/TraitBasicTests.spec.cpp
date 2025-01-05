// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FTraitBasicTestsSpec,
                  "Flecs.Traits.Basic",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FTraitBasicTestsSpec);

struct FTestComponent_TraitBasic
{
	int32 Value;
}; // struct FTestComponent

struct FTestComponent2_TraitBasic
{
	float Value;
}; // struct FTestComponent2

struct FTestTrait_TraitBasic
{
	int32 Value;
}; // struct FTestTrait

struct FTestTrait2_TraitBasic
{
	float Value;
}; // struct FTestTrait2

void FTraitBasicTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Trait Creation and Destruction", [this]()
	{
		It("Should be able to Obtain the Trait Holder Entity of a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitBasic>();
			Entity.AddTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>();

			FFlecsEntityHandle TraitHolder = Entity.ObtainTraitHolderEntity<FTestComponent_TraitBasic>();
			TestTrue("Trait Holder should be valid", TraitHolder.IsValid());
		});
		
		It("Should be able to add a trait to a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitBasic>();
			Entity.AddTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>();
			
			TestTrue("Trait should be added", Entity.Has<FTestComponent_TraitBasic>());
			TestTrue("Trait should be added", Entity.HasTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>());
		});

		It("Should be able to remove a trait from a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitBasic>();
			Entity.AddTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>();

			TestTrue("Trait should be removed", Entity.HasTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>());

			Entity.RemoveTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>();
			TestFalse("Trait should be removed", Entity.HasTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>());
		});

		It("Should be able to add multiple traits to a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitBasic>();
			Entity.AddTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>();
			Entity.AddTrait<FTestComponent_TraitBasic, FTestTrait2_TraitBasic>();

			TestTrue("Trait should be added", Entity.HasTrait<FTestComponent_TraitBasic, FTestTrait_TraitBasic>());
			TestTrue("Trait should be added", Entity.HasTrait<FTestComponent_TraitBasic, FTestTrait2_TraitBasic>());
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
