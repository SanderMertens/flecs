
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "ComponentTestStructs.h"

BEGIN_DEFINE_SPEC(FRegisteredComponentTestsSpec,
                  "Flecs.Components.Registration",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FRegisteredComponentTestsSpec);

void FRegisteredComponentTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Add Components", [this]()
	{
		It("Should add USTRUCT Component using StaticStruct",
			[this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FUStructTestComponent::StaticStruct());

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent::StaticStruct()));
		});

		It("Should add USTRUCT Component using cpp type", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent>();

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent::StaticStruct()));
		});

		It("Should set USTRUCT Component using FInstancedStruct", [this]()
		{
			FInstancedStruct TestComponent = FInstancedStruct(FUStructTestComponent::StaticStruct());
			TestComponent.InitializeAs<FUStructTestComponent>(FUStructTestComponent{ 1 });
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set(TestComponent);

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent>());
				
			if (TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent::StaticStruct())))
			{
				TestEqual("Component properties should be registered",
					TestEntity.Get<FUStructTestComponent>().Value, 1);
			}
		});

		It("Should set USTRUCT Component using Set Ptr", [this]()
		{
			constexpr FUStructTestComponent TestComponent{ 1 };
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set(FUStructTestComponent::StaticStruct(), &TestComponent);

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent>());
				
			if (TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent::StaticStruct())))
			{
				TestEqual("Component properties should be registered",
					TestEntity.Get<FUStructTestComponent>().Value, 1);
			}
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
