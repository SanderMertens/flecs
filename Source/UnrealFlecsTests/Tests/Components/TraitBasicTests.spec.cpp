#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FTraitBasicTestsSpec,
                  "UnrealFlecs.Components.Traits.Basic",
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

	Describe("Trait Queries", [this]()
	{
		It("Should be able to query for a trait", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<>()
				.with<FTestComponent>()
				.begin_scope_traits()
					.with<FTestTrait>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Trait should be found", bFound);
		});

		It("Should be able to query for multiple traits", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();
			Entity.AddTrait<FTestComponent, FTestTrait2>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<>()
				.with<FTestComponent>()
				.begin_scope_traits()
					.with<FTestTrait>()
					.with<FTestTrait2>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query for a trait and a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent>()
				.begin_scope_traits<FTestComponent>()
					.with<FTestTrait>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Trait should be found", bFound);
		});

		It("Should be able to query using previous term begin_scope_traits from template type",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();
			Entity.AddTrait<FTestComponent, FTestTrait2>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent>()
				.begin_scope_traits()
					.with<FTestTrait2>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query using previous term begin_scope_traits from with",
			[this]()
		{
				FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
				Entity.Add<FTestComponent>();
				Entity.AddTrait<FTestComponent, FTestTrait>();

				Fixture.FlecsWorld->Progress();

				const flecs::query<> query = Fixture.FlecsWorld->World.query_builder()
					.with<FTestComponent>()
					.begin_scope_traits()
						.with<FTestTrait>()
					.end_scope_traits()
					.build();

				const bool bFound = query.is_true();
			
				TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query using begin_scope_traits_index",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity("TestEntity");
			Entity.Add<FTestComponent>();
			Entity.AddTrait<FTestComponent, FTestTrait>();
			Entity.AddTrait<FTestComponent, FTestTrait2>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent>()
				.begin_scope_traits_index(0)
					.with<FTestTrait2>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query using begin_scope_traits_index using Index 1", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity("TestEntity");
			Entity.Add<FTestComponent>();
			Entity.Add<FTestComponent2>();
			Entity.AddTrait<FTestComponent, FTestTrait>();
			Entity.AddTrait<FTestComponent2, FTestTrait2>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent>()
				.with<FTestComponent2>()
				.begin_scope_traits_index(1)
					.with<FTestTrait2>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();

			TestTrue("Traits should be found", bFound);
		});
		
	});
}

#endif // WITH_AUTOMATION_TESTS
