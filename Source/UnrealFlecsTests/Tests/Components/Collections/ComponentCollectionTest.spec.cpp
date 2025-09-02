
#include "Transforms/FlecsTransformModule.h"
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Transforms/FlecsTransformComponents.h"
#include "Worlds/FlecsWorld.h"

BEGIN_DEFINE_SPEC(FComponentCollectionTestSpec,
                  "Flecs.Components.Collections",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FComponentCollectionTestSpec);

void FComponentCollectionTestSpec::Define()
{
	BeforeEach([this]()
	{
		const TSolidNotNull<UFlecsTransformModule*> Module = NewObject<UFlecsTransformModule>();
		Fixture.SetUp( nullptr, { Module } );
	});
	
	AfterEach([this]()
	{
		Fixture.TearDown();
	});
}

#endif // WITH_AUTOMATION_TESTS
