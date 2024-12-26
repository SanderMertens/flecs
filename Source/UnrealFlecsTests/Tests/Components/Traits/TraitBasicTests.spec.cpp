#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FTraitBasicTestsSpec,
                  "Flecs.Traits.Basic",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FTraitBasicTestsSpec);

struct FTestComponent
{
	int32 Value;
}; // struct FTestComponent

struct FTestComponent2
{
	float Value;
}; // struct FTestComponent2

struct FTestTrait
{
	int32 Value;
}; // struct FTestTrait

struct FTestTrait2
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
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();

			FFlecsEntityHandle TraitHolder = Entity.ObtainTraitHolderEntity<FTestComponent>();
			TestTrue("Trait Holder should be valid", TraitHolder.IsValid());
		});
		
		It("Should be able to add a trait to a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();
			
			TestTrue("Trait should be added", Entity.Has<FTestComponent>());
			TestTrue("Trait should be added", Entity.HasTrait<FTestComponent, FTestTrait>());
		});

		It("Should be able to remove a trait from a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();

			TestTrue("Trait should be removed", Entity.HasTrait<FTestComponent, FTestTrait>());

			Entity.RemoveTrait<FTestComponent, FTestTrait>();
			TestFalse("Trait should be removed", Entity.HasTrait<FTestComponent, FTestTrait>());
		});

		It("Should be able to add multiple traits to a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();
			Entity.AddTrait<FTestComponent, FTestTrait2>();

			TestTrue("Trait should be added", Entity.HasTrait<FTestComponent, FTestTrait>());
			TestTrue("Trait should be added", Entity.HasTrait<FTestComponent, FTestTrait2>());
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
