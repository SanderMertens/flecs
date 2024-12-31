
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "RegisteredPropertyTest.h"
#include "Components/FlecsActorTag.h"

BEGIN_DEFINE_SPEC(FRegisteredPropertiesTestsSpec,
                  "Flecs.Components.RegisteredProperties",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FRegisteredPropertiesTestsSpec);

void FRegisteredPropertiesTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Registered Tag Properties", [this]()
	{
		It("Should contain Registered Properties from Struct in the Flecs Module",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FFlecsActorTag::StaticStruct());
			
			TestTrue("Component properties should be registered",
				TestType.Has(flecs::PairIsTag));
		});
		
		It("Should contain Registered Properties using type directly",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentType<FTestRegisteredPropertyStruct_RegisterPropertyTest>();
			
			TestTrue("Component properties should be registered",
				TestType.Has(flecs::PairIsTag));
		});
		
		It("Should contain Registered Properties using StaticStruct",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FTestRegisteredPropertyStruct_RegisterPropertyTest::StaticStruct());
			
			TestTrue("Component properties should be registered",
				TestType.Has(flecs::PairIsTag));
		});

		It("Should contain Registered Properties using Auto Component Registration",
			[this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FTestRegisteredPropertyStruct_RegisterPropertyTest>();
			TestTrue("Component should exist on entity",
				TestEntity.Has<FTestRegisteredPropertyStruct_RegisterPropertyTest>());
			TestTrue("Component Entity should exist in world",
				Fixture.FlecsWorld->HasScriptStruct<FTestRegisteredPropertyStruct_RegisterPropertyTest>());
			TestTrue("Component properties should be registered",
				Fixture.FlecsWorld->GetScriptStructEntity<FTestRegisteredPropertyStruct_RegisterPropertyTest>().Has(flecs::PairIsTag));
		});

		It("Should contain Meta-Data with Registered Properties", [this]()
		{
			UScriptStruct* TestStruct = FTestRegisteredPropertyStruct_RegisterPropertyTest::StaticStruct();
			MAYBE_UNUSED const FFlecsEntityHandle TestType = Fixture.FlecsWorld->ObtainComponentTypeStruct(TestStruct);
			TestTrue("ScriptStruct should be registered",
				TestStruct->HasMetaData("FlecsTags"));
			TestTrue("Script Struct should have properties as meta-data",
				TestStruct->GetMetaData("FlecsTags").Contains("flecs::PairIsTag"));
		});
	});

	Describe("Registered Trait Property Structs", [this]()
	{
		It("Should contain Registered Trait Properties using type directly",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentType<FTestRegisteredPropertyStruct2_RegisterPropertyTest>();
			
			TestTrue("Component properties should be registered",
				TestType.Has<FTestRegisteredTraitProperty_RegisterPropertyTest>());
			TestTrue("Component properties should be registered",
				TestType.Has(FTestRegisteredTraitProperty_RegisterPropertyTest::StaticStruct()));
			TestTrue("Component property trait should be of value 1",
				TestType.Get<FTestRegisteredTraitProperty_RegisterPropertyTest>().MyInt32 == 1);
		});

		It("Should contain Registered Trait Properties using StaticStruct",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FTestRegisteredPropertyStruct2_RegisterPropertyTest::StaticStruct());
			
			TestTrue("Component properties should be registered",
				TestType.Has<FTestRegisteredTraitProperty_RegisterPropertyTest>());
			TestTrue("Component properties should be registered",
				TestType.Has(FTestRegisteredTraitProperty_RegisterPropertyTest::StaticStruct()));
			TestTrue("Component property trait should be of value 1",
				TestType.Get<FTestRegisteredTraitProperty_RegisterPropertyTest>().MyInt32 == 1);
		});

		It("Should contain Registered Trait Properties when Auto Registered",
			[this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FTestRegisteredPropertyStruct2_RegisterPropertyTest>();
			TestTrue("Component should exist on entity",
				TestEntity.Has<FTestRegisteredPropertyStruct2_RegisterPropertyTest>());
			TestTrue("Component Entity should exist in world",
				Fixture.FlecsWorld->HasScriptStruct<FTestRegisteredPropertyStruct2_RegisterPropertyTest>());
			TestTrue("Component properties should be registered",
				Fixture.FlecsWorld->GetScriptStructEntity<FTestRegisteredPropertyStruct2_RegisterPropertyTest>()
				.Has<FTestRegisteredTraitProperty_RegisterPropertyTest>());
			TestTrue("Component properties should be registered",
				Fixture.FlecsWorld->GetScriptStructEntity<FTestRegisteredPropertyStruct2_RegisterPropertyTest>()
				.Has(FTestRegisteredTraitProperty_RegisterPropertyTest::StaticStruct()));
			TestTrue("Component property trait should be of value 1",
				Fixture.FlecsWorld->GetScriptStructEntity<FTestRegisteredPropertyStruct2_RegisterPropertyTest>()
				.Get<FTestRegisteredTraitProperty_RegisterPropertyTest>().MyInt32 == 1);
		});
	});

	Describe("Multiple Registered Trait and Tag Properties", [this]()
	{
		It("Should contain Multiple Registered Trait and Tag Properties",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FTestRegisteredPropertyStruct3_RegisterPropertyTest::StaticStruct());
			
			TestTrue("Component properties should be registered",
				TestType.Has<FTestRegisteredTraitProperty_RegisterPropertyTest>());
			TestTrue("Component properties should be registered",
				TestType.Has(FTestRegisteredTraitProperty_RegisterPropertyTest::StaticStruct()));
			TestTrue("Component property trait should be of value 1",
				TestType.Get<FTestRegisteredTraitProperty_RegisterPropertyTest>().MyInt32 == 2);
			TestTrue("Component properties should be registered",
				TestType.Has(flecs::PairIsTag));
		});
	});
}


#endif // WITH_AUTOMATION_TESTS
