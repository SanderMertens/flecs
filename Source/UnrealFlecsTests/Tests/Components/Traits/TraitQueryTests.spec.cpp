#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FTraitQueryTestsSpec,
                  "Flecs.Traits.Query",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FTraitQueryTestsSpec);

struct FTestComponent_TraitQuery
{
	int32 Value;
}; // struct FTestComponent

struct FTestComponent2_TraitQuery
{
	float Value;
}; // struct FTestComponent2

struct FTestTrait_TraitQuery
{
	int32 Value;
}; // struct FTestTrait_TraitQuery

struct FTestTrait2_TraitQuery
{
	float Value;
}; // struct FTestTrait2__TraitQuery

void FTraitQueryTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Trait Inclusion Queries", [this]()
	{
		It("Should be able to query for a trait", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<>()
				.with<FTestComponent_TraitQuery>()
				.begin_scope_traits()
					.with<FTestTrait_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Trait should be found", bFound);
		});

		It("Should be able to query for multiple traits", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait2_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<>()
				.with<FTestComponent_TraitQuery>()
				.begin_scope_traits()
					.with<FTestTrait_TraitQuery>()
					.with<FTestTrait2_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query for a trait and a component", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent_TraitQuery>()
				.begin_scope_traits<FTestComponent_TraitQuery>()
					.with<FTestTrait_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Trait should be found", bFound);
		});

		It("Should be able to query using previous term begin_scope_traits from template type",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait2_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent_TraitQuery>()
				.begin_scope_traits()
					.with<FTestTrait2_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query using previous term begin_scope_traits from with",
			[this]()
		{
				FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
				Entity.Add<FTestComponent_TraitQuery>();
				Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();

				Fixture.FlecsWorld->Progress();

				const flecs::query<> query = Fixture.FlecsWorld->World.query_builder()
					.with<FTestComponent_TraitQuery>()
					.begin_scope_traits()
						.with<FTestTrait_TraitQuery>()
					.end_scope_traits()
					.build();

				const bool bFound = query.is_true();
			
				TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query using begin_scope_traits_index",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity("TestEntity");
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait2_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent_TraitQuery>()
				.begin_scope_traits_index(0)
					.with<FTestTrait2_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Traits should be found", bFound);
		});

		It("Should be able to query using begin_scope_traits_index using Index 1", [this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity("TestEntity");
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.Add<FTestComponent2_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();
			Entity.AddTrait<FTestComponent2_TraitQuery, FTestTrait2_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent_TraitQuery>()
				.with<FTestComponent2_TraitQuery>()
				.begin_scope_traits_index(1)
					.with<FTestTrait2_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();

			TestTrue("Traits should be found", bFound);
		});
	});

	Describe("Trait Exclusion Queries", [this]()
	{
		It("Should be able to query for a trait exclusion without anything",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<>()
				.begin_scope_traits<FTestComponent_TraitQuery>().optional()
					.without<FTestTrait_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Trait should be found", bFound);
		});

		It("Should be able to query for a trait exclusion with a component",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent_TraitQuery>()
				.begin_scope_traits<FTestComponent_TraitQuery>().optional()
					.without<FTestTrait_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestTrue("Trait should be found", bFound);
		});

		It("Should be able to query for a trait exclusion with a component and a trait",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent_TraitQuery>()
				.begin_scope_traits<FTestComponent_TraitQuery>().optional()
					.with<FTestTrait_TraitQuery>()
					.without<FTestTrait_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.is_true();
			
			TestFalse("Trait should not be found", bFound);
		});

		It("Should be able to query for a trait exclusion with a component and 2 traits",
			[this]()
		{
			FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity();
			Entity.Add<FTestComponent_TraitQuery>();
			Entity.AddTrait<FTestComponent_TraitQuery, FTestTrait_TraitQuery>();

			Fixture.FlecsWorld->Progress();

			const flecs::query<> query = Fixture.FlecsWorld->World.query_builder<FTestComponent_TraitQuery>()
				.begin_scope_traits<FTestComponent_TraitQuery>().optional()
					.with<FTestTrait_TraitQuery>()
					.without<FTestTrait2_TraitQuery>()
				.end_scope_traits()
				.build();

			const bool bFound = query.contains(Entity);
			
			TestTrue("Trait should not be found", bFound);
		});
		
	});
}

#endif // WITH_AUTOMATION_TESTS
