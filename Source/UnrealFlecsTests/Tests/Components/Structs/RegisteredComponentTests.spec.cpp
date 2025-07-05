// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Tests/FlecsTestTypes.h"

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
		
		Fixture.FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
		Fixture.FlecsWorld->RegisterComponentType<FFlecsTestComponent_Inherited>();
	});
	
	AfterEach([this]()
	{
		Fixture.TearDown();
	});

	Describe("Component Registration", [this]()
	{
		It("Should register a size 1 USTRUCT and be a tag using CPP API", [this]()
		{
			Fixture.FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FFlecsTestStruct_Tag::StaticStruct());

			FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FFlecsTestStruct_Tag::StaticStruct());
			
			TestTrue("Entity should have the FUStructTestComponent_TagUSTRUCT component using the CPP API",
				TestEntity.Has<FFlecsTestStruct_Tag>());
			TestTrue("Entity should have the FUStructTestComponent_TagUSTRUCT component using StaticStruct API",
				TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct()));
			TestEqual("Component Size should be 0",
				RegisteredComponent.Get<flecs::Component>().size, 0);
		});

		It("Should register a size 1 USTRUCT and be a tag using StaticStruct", [this]()
		{
			Fixture.FlecsWorld->RegisterComponentType(FFlecsTestStruct_Tag::StaticStruct());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FFlecsTestStruct_Tag::StaticStruct());

			const FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FFlecsTestStruct_Tag::StaticStruct());
			
			TestTrue("Entity should have the FUStructTestComponent_TagUSTRUCT component using the StaticStruct API",
				TestEntity.Has<FFlecsTestStruct_Tag>());
			TestTrue("Entity should have the FUStructTestComponent_TagUSTRUCT component using StaticStruct API",
				TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct()));
			
			TestEqual("Component Size should be 0",
				RegisteredComponent.Get<flecs::Component>().size, 0);
		});

		It("Should register a size 1 USTRUCT and be a non tag due to having a UPROPERTY using CPP",
			[this]()
		{
			Fixture.FlecsWorld->RegisterComponentType<FUStructTestComponent_NonTagUSTRUCT>();
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FUStructTestComponent_NonTagUSTRUCT::StaticStruct());

			const FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FUStructTestComponent_NonTagUSTRUCT::StaticStruct());
				
			TestTrue("Entity should have the FUStructTestComponent_NonTagUSTRUCT component using the CPP API",
				TestEntity.Has<FUStructTestComponent_NonTagUSTRUCT>());
			TestTrue("Entity should have the FUStructTestComponent_NonTagUSTRUCT component using StaticStruct API",
				TestEntity.Has(FUStructTestComponent_NonTagUSTRUCT::StaticStruct()));
				
			TestEqual("Component Size should be 1",
				RegisteredComponent.Get<flecs::Component>().size, 1);
		});

		It("Should register a size 1 USTRUCT and be a non tag due to having a UPROPERTY using StaticStruct", [this]()
		{
			Fixture.FlecsWorld->RegisterComponentType(FUStructTestComponent_NonTagUSTRUCT::StaticStruct());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FUStructTestComponent_NonTagUSTRUCT::StaticStruct());

			FFlecsEntityHandle RegisteredComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(FUStructTestComponent_NonTagUSTRUCT::StaticStruct());
			TestTrue("Entity should have the FUStructTestComponent_NonTagUSTRUCT component using the StaticStruct API",
				TestEntity.Has<FUStructTestComponent_NonTagUSTRUCT>());
			TestTrue("Entity should have the FUStructTestComponent_NonTagUSTRUCT component using StaticStruct API",
				TestEntity.Has(FUStructTestComponent_NonTagUSTRUCT::StaticStruct()));
			TestEqual("Component Size should be 1",
				RegisteredComponent.Get<flecs::Component>().size, 1);
		});
	});

	Describe("Add Components", [this]()
	{
		It("Should add USTRUCT Component using StaticStruct", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(FFlecsTestStruct_Value::StaticStruct());

			const FFlecsEntityHandle StaticStructComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(FFlecsTestStruct_Value::StaticStruct());
			const FFlecsEntityHandle CPPTypeComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct<FFlecsTestStruct_Value>();

			TestTrue("Entity should have the FFlecsTestStruct_Value component using StaticStruct",
				TestEntity.Has<FFlecsTestStruct_Value>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));
			TestEqual("Component properties should be registered",
				StaticStructComponent, CPPTypeComponent);
		});

		It("Should add USTRUCT Component using cpp type", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FFlecsTestStruct_Value>();
			
			FFlecsEntityHandle StaticStructComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(FFlecsTestStruct_Value::StaticStruct());
			FFlecsEntityHandle CPPTypeComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct<FFlecsTestStruct_Value>();

			TestTrue("Component properties should be registered",
				TestEntity.Has<FFlecsTestStruct_Value>());
			TestTrue("Component properties should be registered",
				TestEntity.Has(FFlecsTestStruct_Value::StaticStruct()));
			TestEqual("Component properties should be registered",
				StaticStructComponent, CPPTypeComponent);
		});

		It("Should set USTRUCT Component using FInstancedStruct", [this]()
		{
			FInstancedStruct TestComponent = FInstancedStruct(FFlecsTestStruct_Value::StaticStruct());
			TestComponent.InitializeAs<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ 1 });
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set(TestComponent);

			TestTrue("Component properties should be registered",
				TestEntity.Has<FFlecsTestStruct_Value>());
				
			if (TestTrue("Component properties should be registered",
				TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())))
			{
				TestEqual("Component properties should be registered",
					TestEntity.Get<FFlecsTestStruct_Value>().Value, 1);
			}
		});

		It("Should set USTRUCT Component using Set Ptr", [this]()
		{
			constexpr FFlecsTestStruct_Value TestComponent{ 1 };
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set(FFlecsTestStruct_Value::StaticStruct(), &TestComponent);

			TestTrue("Component properties should be registered",
				TestEntity.Has<FFlecsTestStruct_Value>());
				
			if (TestTrue("Component properties should be registered",
				TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())))
			{
				TestEqual("Component properties should be registered",
					TestEntity.Get<FFlecsTestStruct_Value>().Value, 1);
			}
		});
	});

	Describe("Unreal Struct Type Registration", [this]()
	{
		It("Should Register Non Static Struct, Script Struct(FVector) using CPP API", [this]()
		{
			FFlecsEntityHandle FVectorComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct<FVector>();
			TestTrue("Component properties should be registered",
				FVectorComponent.Has<FFlecsScriptStructComponent>());
		});

		It("Should Register Non Static Struct, Script Struct(FVector) using BaseStructure", [this]()
		{
			FFlecsEntityHandle FVectorComponent = Fixture.FlecsWorld->ObtainComponentTypeStruct(TBaseStructure<FVector>::Get());
			TestTrue("Component properties should be registered",
				FVectorComponent.Has<FFlecsScriptStructComponent>());
		});
	});

	xDescribe("Custom Alignment Registration", [this]()
	{
		It("Should Register(CPP API) and Add Component with Custom Alignment (16 bytes)", [this]()
		{
			FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes>();
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes>();

			TestTrue("Component should be registered",
				TestEntity.Has<FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes>());
			
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes::StaticStruct()));
			
			TestEqual("Component Size should be 16",
				RegisteredComponent.Get<flecs::Component>().alignment,
				alignof(FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes));

			const FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes* ComponentPtr 
				= TestEntity.GetPtr<FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes>();

			TestTrue("Component Memory should be aligned to 16 bytes",
				IsAligned(ComponentPtr, alignof(FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes)));
		});

		It("Should Register(StaticStruct) and Add Component with Custom Alignment (16 bytes)", [this]()
		{
			FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes::StaticStruct());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes>();

			TestTrue("Component should be registered",
				TestEntity.Has<FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes>());
			
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes::StaticStruct()));
			
			TestEqual("Component Size should be 16",
				RegisteredComponent.Get<flecs::Component>().alignment,
				alignof(FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes));

			const FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes* ComponentPtr
				= TestEntity.GetPtr<FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes>();

			TestTrue("Component Memory should be aligned to 16 bytes",
				IsAligned(ComponentPtr, alignof(FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes)));
		});

		It("Should Register(CPP API) and Add Component with Custom Alignment (32 bytes)", [this]()
		{
			FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes>();
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes>();

			TestTrue("Component should be registered",
				TestEntity.Has<FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes>());
			
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes::StaticStruct()));
			
			TestEqual("Component Size should be 32",
				RegisteredComponent.Get<flecs::Component>().alignment,
				alignof(FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes));

			const FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes* ComponentPtr
				= TestEntity.GetPtr<FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes>();
			
			TestTrue("Component Memory should be aligned to 32 bytes",
				IsAligned(ComponentPtr, alignof(FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes)));
		});

		It("Should Register(StaticStruct) and Add Component with Custom Alignment (32 bytes)", [this]()
		{
			FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes::StaticStruct());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes>();

			TestTrue("Component should be registered",
				TestEntity.Has<FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes>());
			
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes::StaticStruct()));
			
			TestEqual("Component Size should be 32",
				RegisteredComponent.Get<flecs::Component>().alignment,
				alignof(FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes));

			const FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes* ComponentPtr
				= TestEntity.GetPtr<FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes>();

			TestTrue("Component Memory should be aligned to 32 bytes",
				IsAligned(ComponentPtr, alignof(FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes)));
		});

		It("Should Register(CPP API) and Add Component with Custom Alignment (64 bytes)", [this]()
		{
			FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes>();
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes>();

			TestTrue("Component should be registered",
				TestEntity.Has<FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes>());
			
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes::StaticStruct()));
			
			TestEqual("Component Size should be 64",
				RegisteredComponent.Get<flecs::Component>().alignment,
				alignof(FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes));
		});

		It("Should Register(StaticStruct) and Add Component with Custom Alignment (64 bytes)", [this]()
		{
			FFlecsEntityHandle RegisteredComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes::StaticStruct());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes>();

			TestTrue("Component should be registered",
				TestEntity.Has<FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes>());
			
			TestTrue("Component properties should be registered",
				TestEntity.Has(FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes::StaticStruct()));
			
			TestEqual("Component Size should be 64",
				RegisteredComponent.Get<flecs::Component>().alignment,
				alignof(FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes));

		});
		
	});

	Describe("Pair Testing", [this]()
	{
		It("Should add entity tag pair", [this]()
		{
			const FFlecsEntityHandle Tag = Fixture.FlecsWorld->CreateEntity("TestTag1");
			const FFlecsEntityHandle Tag2 = Fixture.FlecsWorld->CreateEntity("TestTag2");
			

			const FFlecsEntityHandle Entity = Fixture.FlecsWorld->CreateEntity()
				.AddPair(Tag, Tag2);
			TestTrue("Entity should have pair tag",
				Entity.HasPair(Tag, Tag2));
		});

		It("Should add component pair using Flecs API", [this]()
		{
			const FFlecsComponentHandle PairComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent>();
			const FFlecsComponentHandle PairSecondComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent_Second>();

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity()
				.AddPair(PairComponent, PairSecondComponent);
		});

		It("Should add component pair using CPP Type API", [this]()
		{
			const FFlecsComponentHandle PairComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent>();
			const FFlecsComponentHandle PairSecondComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent_Second>();

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity()
				.AddPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>();
			
			TestTrue("Entity should have pair component",
				TestEntity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>());
		});

		It("Should add component pair using Static Struct API", [this]()
		{
			const FFlecsComponentHandle PairComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUSTRUCTPairTestComponent::StaticStruct());
			const FFlecsComponentHandle PairSecondComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUSTRUCTPairTestComponent_Second::StaticStruct());

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity()
				.AddPair(FUSTRUCTPairTestComponent::StaticStruct(),
					FUSTRUCTPairTestComponent_Second::StaticStruct());
			
			TestTrue("Entity should have pair component",
				TestEntity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(),
					FUSTRUCTPairTestComponent_Second::StaticStruct()));
		});

		It("Should add component pair using Static Struct API with CPP Type", [this]()
		{
			const FFlecsComponentHandle PairComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUSTRUCTPairTestComponent::StaticStruct());
			const FFlecsComponentHandle PairSecondComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUSTRUCTPairTestComponent_Second::StaticStruct());

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity()
				.AddPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>();
			
			TestTrue("Entity should have pair component",
				TestEntity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>());
		});

		It("Should add component pair with first being data using CPP API", [this]()
		{
			const FFlecsComponentHandle PairComponent
			= Fixture.FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent>();
			const FFlecsComponentHandle PairSecondComponent
				= Fixture.FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent_Data>();

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity()
				.SetPairFirst<FUSTRUCTPairTestComponent_Data, FUSTRUCTPairTestComponent>(
					FUSTRUCTPairTestComponent_Data
					{
						.Value = 42
					});

			TestTrue("Entity should have pair component",
				TestEntity.HasPair<FUSTRUCTPairTestComponent_Data, FUSTRUCTPairTestComponent>());

			TestTrue("Entity should have pair component using StaticStruct API",
				TestEntity.HasPair(FUSTRUCTPairTestComponent_Data::StaticStruct(), FUSTRUCTPairTestComponent::StaticStruct()));

			const FUSTRUCTPairTestComponent_Data& PairData
				= TestEntity.GetPairFirst<FUSTRUCTPairTestComponent_Data, FUSTRUCTPairTestComponent>();
			TestEqual("Pair Data Value should be 42", PairData.Value, 42);
		});

		It("Should add component pair with first being data using Static Struct API", [this]()
		{
			const FFlecsComponentHandle PairComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUSTRUCTPairTestComponent::StaticStruct());
			const FFlecsComponentHandle PairSecondComponent
				= Fixture.FlecsWorld->RegisterComponentType(FUSTRUCTPairTestComponent_Data::StaticStruct());

			FUSTRUCTPairTestComponent_Data PairData
			{
				.Value = 42
			};
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity()
				.SetPairFirst(FUSTRUCTPairTestComponent_Data::StaticStruct(), &PairData, FUSTRUCTPairTestComponent::StaticStruct());
			
			TestTrue("Entity should have pair component",
				TestEntity.HasPair<FUSTRUCTPairTestComponent_Data, FUSTRUCTPairTestComponent>());

			TestTrue("Entity should have pair component using StaticStruct API",
				TestEntity.HasPair(FUSTRUCTPairTestComponent_Data::StaticStruct(),
					FUSTRUCTPairTestComponent::StaticStruct()));

			const void* PairDataPtr
				= TestEntity.GetPairPtrFirst(FUSTRUCTPairTestComponent_Data::StaticStruct(),
					FUSTRUCTPairTestComponent::StaticStruct());
			TestEqual("Pair Data Value should be 42",
				static_cast<const FUSTRUCTPairTestComponent_Data*>(PairDataPtr)->Value, 42);
		});
		
	});
}

#endif // WITH_AUTOMATION_TESTS
