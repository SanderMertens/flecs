
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FFlecsWorldLifecycleTestsSpec, "UnrealFlecs.World.LifecycleTests",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

	FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FFlecsWorldLifecycleTestsSpec)

void FFlecsWorldLifecycleTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);
	
	Describe("World Creation", [this]()
	{
		It("Should create a valid Flecs world", [this]()
		{
			TestTrue("Flecs world is valid", Fixture.FlecsWorld.IsValid());
		});
	});
}

#endif // WITH_AUTOMATION_TESTS