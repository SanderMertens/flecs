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
	BeforeEach([this]()
	{
		Fixture.SetUp();

		Fixture.FlecsWorld->RegisterComponentType<FTestComponent_RegisterComponentTest>();
		Fixture.FlecsWorld->RegisterComponentType<FUStructTestComponent_RegisterComponentTest>();
		Fixture.FlecsWorld->RegisterComponentType<FUStructTestComponent_Inherited>();
	});
	
	AfterEach([this]()
	{
		Fixture.TearDown();
	});

	Describe("Component Registration", [this]()
	{
		It("Should register a size 1 USTRUCT and be a tag using CPP API", [this]()
		{
			Fixture.FlecsWorld->RegisterComponentType<FUStructTestComponent_TagUSTRUCT>();
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FUStructTestComponent_TagUSTRUCT::StaticStruct());

			FFlecsEntityHandle RegisteredComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(FUStructTestComponent_TagUSTRUCT::StaticStruct());
			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_TagUSTRUCT>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_TagUSTRUCT::StaticStruct()));
			TestEqual("Component Size should be 0",
				RegisteredComponent.Get<flecs::Component>().size, 0);
		});

		It("Should register a size 1 USTRUCT and be a tag using StaticStruct", [this]()
		{
			Fixture.FlecsWorld->RegisterComponentType(FUStructTestComponent_TagUSTRUCT::StaticStruct());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FUStructTestComponent_TagUSTRUCT::StaticStruct());

			FFlecsEntityHandle RegisteredComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(FUStructTestComponent_TagUSTRUCT::StaticStruct());
			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_TagUSTRUCT>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_TagUSTRUCT::StaticStruct()));
			TestEqual("Component Size should be 0",
				RegisteredComponent.Get<flecs::Component>().size, 0);
		});
	});

	Describe("Add Components", [this]()
	{
		It("Should add USTRUCT Component using StaticStruct", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FUStructTestComponent_RegisterComponentTest::StaticStruct());

			FFlecsEntityHandle StaticStructComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(FUStructTestComponent_RegisterComponentTest::StaticStruct());
			FFlecsEntityHandle CPPTypeComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct<FUStructTestComponent_RegisterComponentTest>();

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_RegisterComponentTest>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_RegisterComponentTest::StaticStruct()));
			TestEqual("Component properties should be registered",
				StaticStructComponent, CPPTypeComponent);
		});

		It("Should add USTRUCT Component using cpp type", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_RegisterComponentTest>();
			
			FFlecsEntityHandle StaticStructComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(FUStructTestComponent_RegisterComponentTest::StaticStruct());
			FFlecsEntityHandle CPPTypeComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct<FUStructTestComponent_RegisterComponentTest>();

			TestTrue("Component properties should be registered",
				TestEntity.Has<FUStructTestComponent_RegisterComponentTest>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_RegisterComponentTest::StaticStruct()));
			TestEqual("Component properties should be registered",
				StaticStructComponent, CPPTypeComponent);
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

	Describe("Register Inherited Components", [this]()
	{
		// It("Should add USTRUCT Component using StaticStruct", [this]()
		// {
		// 	FFlecsEntityHandle RegisteredInheritedComponent
		// 		= Fixture.FlecsWorld->ObtainComponentTypeStruct(FUStructTestComponent_Inherited::StaticStruct());
		// 	TestTrue("Inherited Component should have its parent",
		// 		RegisteredInheritedComponent.HasPair(flecs::IsA, FUStructTestComponent_RegisterComponentTest::StaticStruct()));
		// });
	});

	It("Should Register Non Static Struct Script Structs(FVector)", [this]()
	{
		FFlecsEntityHandle FVectorComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct<FVector>();
		TestTrue("Component properties should be registered",
			FVectorComponent.Has<FFlecsScriptStructComponent>());
	});
}

#endif // WITH_AUTOMATION_TESTS
