
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Kismet/KismetSystemLibrary.h"

BEGIN_DEFINE_SPEC(FUObjectTestsSpec,
                  "Flecs.Components.UObjectComponent",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FUObjectTestsSpec);

void FUObjectTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);
}

#endif // WITH_AUTOMATION_TESTS
