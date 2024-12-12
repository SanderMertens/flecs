#include "UnrealFlecsTests/Fixtures/FlecsWorldFixture.h"
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

BEGIN_DEFINE_SPEC(FFlecsWorldLifecycleTestsSpec, "UnrealFlecs.World.LifecycleTests",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

	FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FFlecsWorldLifecycleTestsSpec)

void FFlecsWorldLifecycleTestsSpec::Define()
{
	BeforeEach([this]()
	{
		Fixture.SetUp();
	});

	AfterEach([this]()
	{
		Fixture.TearDown();
	});
	
	Describe("World Creation", [this]()
	{
		It("should create a valid Flecs world", [this]()
		{
			TestTrue("Flecs world is valid", Fixture.FlecsWorld.IsValid());
		});
	});
}

#endif // WITH_AUTOMATION_TESTS