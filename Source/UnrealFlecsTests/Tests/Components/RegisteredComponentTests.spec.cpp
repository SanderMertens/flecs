
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

// Define the Spec using template variables for consistency
BEGIN_DEFINE_SPEC(FRegisteredComponentTestsSpec,
                  "UnrealFlecs.Components.Registration",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FRegisteredComponentTestsSpec);

void FRegisteredComponentTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);
	
}

#endif // WITH_AUTOMATION_TESTS
