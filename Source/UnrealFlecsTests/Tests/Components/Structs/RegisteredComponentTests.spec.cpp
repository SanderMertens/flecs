// Elie Wiese-Namir © 2025. All Rights Reserved.


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
			TestEntity.Add(FUStructTestComponent_RegisterComponentTest::StaticStruct());

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_RegisterComponentTest>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_RegisterComponentTest::StaticStruct()));
		});

		It("Should add USTRUCT Component using cpp type", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_RegisterComponentTest>();

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_RegisterComponentTest>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_RegisterComponentTest::StaticStruct()));
		});

		It("Should set USTRUCT Component using FInstancedStruct", [this]()
		{
			FInstancedStruct TestComponent = FInstancedStruct(FUStructTestComponent_RegisterComponentTest::StaticStruct());
			TestComponent.InitializeAs<FUStructTestComponent_RegisterComponentTest>(FUStructTestComponent_RegisterComponentTest{ 1 });
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set(TestComponent);

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_RegisterComponentTest>());
				
			if (TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_RegisterComponentTest::StaticStruct())))
			{
				TestEqual("Component properties should be registered",
					TestEntity.Get<FUStructTestComponent_RegisterComponentTest>().Value, 1);
			}
		});

		It("Should set USTRUCT Component using Set Ptr", [this]()
		{
			constexpr FUStructTestComponent_RegisterComponentTest TestComponent{ 1 };
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set(FUStructTestComponent_RegisterComponentTest::StaticStruct(), &TestComponent);

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_RegisterComponentTest>());
				
			if (TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_RegisterComponentTest::StaticStruct())))
			{
				TestEqual("Component properties should be registered",
					TestEntity.Get<FUStructTestComponent_RegisterComponentTest>().Value, 1);
			}
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
