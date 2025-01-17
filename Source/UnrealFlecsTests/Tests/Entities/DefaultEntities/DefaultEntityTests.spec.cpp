// Elie Wiese-Namir © 2025. All Rights Reserved.


#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Transforms/FlecsTransformDefaultEntities.h"
#include "DefaultEntityDefinitions.h"

BEGIN_DEFINE_SPEC(FDefaultEntityTestsSpec,
                  "Flecs.Entity.Default_Entities",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FDefaultEntityTestsSpec);

void FDefaultEntityTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Default Entity Validity", [this]()
	{
		It("Should be valid from the same module", [this]()
		{
			TestTrue("Default entity should be valid",
				Fixture.FlecsWorld->GetEntity(TestEntityOption).IsValid());
		});
		
		It("Should be valid from another module", [this]()
		{
			TestTrue("Default entity should be valid",
				Fixture.FlecsWorld->GetEntity(FlecsLocalTrait).IsValid());
		});
	});

	Describe("Default Entity Ids", [this]()
	{
		It("Should have the correct id", [this]()
		{
			TestEqual("Default entity should have the correct id",
				Fixture.FlecsWorld->GetEntity(TestEntityOption).GetId(),
				6000);
			TestEqual("Default entity should have the correct id",
				Fixture.FlecsWorld->GetEntity(FlecsLocalTrait).GetId(),
				5002);
			TestEqual("Default entity should have the correct id",
				Fixture.FlecsWorld->GetEntity(FlecsGlobalTrait).GetId(),
				5003);
			TestEqual("Default entity should have the correct id",
				Fixture.FlecsWorld->GetEntity(FlecsRelativeTrait).GetId(),
				5004);
			
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
