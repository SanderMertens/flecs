
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FEntityBenchmarksSpec,
                  "Flecs.Benchmarks.Entity",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEntityBenchmarksSpec);

void FEntityBenchmarksSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);
}

#endif // WITH_AUTOMATION_TESTS
