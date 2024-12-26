
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "RegisteredPropertyTest.h"
#include "Components/FlecsActorTag.h"

BEGIN_DEFINE_SPEC(FRegisteredPropertiesTestsSpec,
                  "UnrealFlecs.Components.RegisteredProperties",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FRegisteredPropertiesTestsSpec);

void FRegisteredPropertiesTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Registered Types", [this]()
	{
		It("Should contain Registered Properties from Struct in the Flecs Module",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FFlecsActorTag::StaticStruct());
			
			TestTrue("Component properties should be registered",
				TestType.Has(flecs::PairIsTag));
		});
		
		It("Should contain Registered Properties(Tags) using type directly",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentType<FTestRegisteredPropertyStruct>();
			
			TestTrue("Component properties should be registered",
				TestType.Has(flecs::PairIsTag));
		});
		
		It("Should contain Registered Properties(Tags) using StaticStruct",
			[this]()
		{
			const FFlecsEntityHandle TestType
				= Fixture.FlecsWorld->ObtainComponentTypeStruct(FTestRegisteredPropertyStruct::StaticStruct());
			
			TestTrue("Component properties should be registered",
				TestType.Has(flecs::PairIsTag));
		});

		It("Should contain Registered Properties(Tags) using Auto Component Registration",
			[this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<FTestRegisteredPropertyStruct>();
			TestTrue("Component should exist on entity",
				TestEntity.Has<FTestRegisteredPropertyStruct>());
			TestTrue("Component Entity should exist in world",
				Fixture.FlecsWorld->HasScriptStruct<FTestRegisteredPropertyStruct>());
			TestTrue("Component properties should be registered",
				Fixture.FlecsWorld->GetScriptStructEntity<FTestRegisteredPropertyStruct>().Has(flecs::PairIsTag));
		});

		It("Should contain Meta-Data with Registered Properties(Tags)", [this]()
		{
			UScriptStruct* TestStruct = FTestRegisteredPropertyStruct::StaticStruct();
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
				= Fixture.FlecsWorld->ObtainComponentType<FTestRegisteredPropertyStruct2>();
			
			TestTrue("Component properties should be registered",
				TestType.Has<FTestRegisteredTraitProperty>());
			TestTrue("Component properties should be registered",
				TestType.Has(FTestRegisteredTraitProperty::StaticStruct()));
		});
	});
}


#endif // WITH_AUTOMATION_TESTS
