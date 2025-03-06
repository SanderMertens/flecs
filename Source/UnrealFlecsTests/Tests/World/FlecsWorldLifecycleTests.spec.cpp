// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FFlecsWorldLifecycleTestsSpec, "Flecs.World.LifecycleTests",
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

	Describe("World Ptr Tests", [this]()
	{
		It("Should be able to convert flecs::world to UFlecsWorld using ToFlecsWorld Function", [this]()
		{
			TestTrue("Flecs world is valid", Fixture.FlecsWorld.IsValid());
			TestNotNull("Flecs world singleton is valid", ToFlecsWorld(Fixture.FlecsWorld->World));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS