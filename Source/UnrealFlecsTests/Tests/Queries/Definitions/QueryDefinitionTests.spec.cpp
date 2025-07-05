// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Queries/FlecsQueryDefinition.h"
#include "Queries/FlecsQuery.h"
#include "Tests/FlecsTestTypes.h"

BEGIN_DEFINE_SPEC(FQueryDefinitionTestsSpec,
                  "Flecs.Query.Definition",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FQueryDefinitionTestsSpec);

void FQueryDefinitionTestsSpec::Define()
{
	BeforeEach([this]()
	{
		Fixture.SetUp();

		Fixture.FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
	});
	
	AfterEach([this]()
	{
		Fixture.TearDown();
	});

	Describe("Create Query Definition", [this]
	{
		It("Should create a query definition", [this]
		{
			FFlecsQueryDefinition Definition;
			
			FFlecsQueryTermExpression TermExpression1;
			TermExpression1.InputType.Type = EFlecsQueryInputType::ScriptStruct;
			TermExpression1.InputType.ScriptStruct = FFlecsTestStruct_Value::StaticStruct();
			
			Definition.AddQueryTerm(TermExpression1);

			TestTrue("Query definition has 1 term", Definition.Terms.Num() == 1);
			TestTrue("Query definition has 1 term with correct input type",
				Definition.Terms[0].InputType.Type == EFlecsQueryInputType::ScriptStruct);
		});

		It("Should create a query definition and build a Query", [this]
		{
			FFlecsQueryDefinition Definition;

			FFlecsQueryTermExpression TermExpression1;
			TermExpression1.InputType.Type = EFlecsQueryInputType::ScriptStruct;
			TermExpression1.InputType.ScriptStruct = FFlecsTestStruct_Value::StaticStruct();

			Definition.AddQueryTerm(TermExpression1);

			flecs::query_builder<> Builder = flecs::query_builder(Fixture.FlecsWorld->World, "TestQuery");
			
			Definition.Apply(Fixture.FlecsWorld.Get(), Builder);
			flecs::query<> Query = Builder.build();

			TestTrue("Query has 1 term count", Query.term_count() == 1);
		});
	});

	Describe("Create and Query for Entities using Definition", [this]
	{
		It("Should create a query definition and query for entities", [this]
		{
			FFlecsEntityHandle Entity1 = Fixture.FlecsWorld->CreateEntity();
			Entity1.Add<FFlecsTestStruct_Value>();
			
			FFlecsQueryDefinition Definition;

			FFlecsQueryTermExpression TermExpression1;
			TermExpression1.InputType.Type = EFlecsQueryInputType::ScriptStruct;
			TermExpression1.InputType.ScriptStruct = FFlecsTestStruct_Value::StaticStruct();

			Definition.AddQueryTerm(TermExpression1);

			flecs::query_builder<> Builder = flecs::query_builder(Fixture.FlecsWorld->World, "TestQuery");

			Definition.Apply(Fixture.FlecsWorld.Get(), Builder);
			flecs::query<> Query = Builder.build();

			TestTrue("Query has 1 term count", Query.term_count() == 1);
			TestTrue("Query has at least 1 entity", Query.count() >= 1);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
