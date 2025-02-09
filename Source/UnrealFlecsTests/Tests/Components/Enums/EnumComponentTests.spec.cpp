
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "ComponentTestEnums.h"

// Define the Spec using template variables for consistency
BEGIN_DEFINE_SPEC(FEnumComponentTestsSpec,
                  "Flecs.Components.Enum",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEnumComponentTestsSpec);

void FEnumComponentTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);
	
	Describe("Enum Component Basic", [this]()
	{
		It("Should be able to Register an Static Enum Component", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum>());
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.Has<flecs::Enum>());
		});

		It("Should be able to Register an Enum Component in CPP", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.Has<flecs::Enum>());
		});

		It("Should be able to Register an Enum Component in CPP and Static", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum>());
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.Has<flecs::Enum>());
			
			FFlecsEntityHandle TestEntity2 = Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();
			TestTrue("Entity is Component", TestEntity2.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity2.Has<flecs::Enum>());

			TestEqual("Both Entities are the same", TestEntity.GetFlecsId(), TestEntity2.GetFlecsId());
		});

		It("Should be able to add an Enum Component", [this]()
		{
			Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.GetEntity().set<ETestEnum>(ETestEnum::One);
			
			TestTrue("Entity has ETestEnum Component", TestEntity.Has<ETestEnum>());
			TestEqual("TestEnum is correct", TestEntity.Get<ETestEnum>(), ETestEnum::One);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
