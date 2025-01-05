// Elie Wiese-Namir © 2025. All Rights Reserved.


#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FQueryDefinitionTestsSpec,
                  "Flecs.Query.Definition",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FQueryDefinitionTestsSpec);

void FQueryDefinitionTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);
}

#endif // WITH_AUTOMATION_TESTS
