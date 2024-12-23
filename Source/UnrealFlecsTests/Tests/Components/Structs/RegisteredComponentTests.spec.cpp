
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "ComponentTestStructs.h"

BEGIN_DEFINE_SPEC(FRegisteredComponentTestsSpec,
                  "UnrealFlecs.Components.Registration",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FRegisteredComponentTestsSpec);

void FRegisteredComponentTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Registered Component Types", [this]()
	{
		It("Should register USTRUCT Component using StaticStruct",
			[this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FUStructTestComponent::StaticStruct());

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent::StaticStruct()));
		});

		It("Should register USTRUCT Component using cpp type",
			[this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent>();

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent::StaticStruct()));
		});

		
	});
}

#endif // WITH_AUTOMATION_TESTS
