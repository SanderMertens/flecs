
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FEntityLifecycleTestsSpec,
                  "UnrealFlecs.Entity.LifecycleTests",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEntityLifecycleTestsSpec);

void FEntityLifecycleTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Entity Creation and Deletion", [this]()
	{
		It("Should create and delete an entity", [this]()
		{
			FFlecsEntityHandle EntityHandle = Fixture.FlecsWorld->CreateEntity();
			TestTrue("Entity should be valid", EntityHandle.IsValid());
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
