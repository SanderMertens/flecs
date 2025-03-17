
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "ComponentTestEnums.h"

BEGIN_DEFINE_SPEC(FEnumComponentTestsSpec,
                  "Flecs.Components.Enum",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FEnumComponentTestsSpec);

void FEnumComponentTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);
	
	Describe("Enum Component Registration", [this]()
	{
		It("Should Register an Static Enum Component", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum>());
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
		});

		It("Should Register an Enum Component in CPP", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
		});

		It("Should Register an Enum Component in Static then CPP", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum>());
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
			
			FFlecsEntityHandle TestEntity2 = Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();
			TestTrue("Entity is Component", TestEntity2.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity2.IsEnum());

			TestEqual("Both Entities are the same", TestEntity.GetFlecsId(), TestEntity2.GetFlecsId());
		});

		It("Should Register an Enum Component in CPP then Static", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
			
			FFlecsEntityHandle TestEntity2 = Fixture.FlecsWorld->ObtainComponentTypeEnum(StaticEnum<ETestEnum>());
			TestTrue("Entity is Component", TestEntity2.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity2.IsEnum());

			TestEqual("Both Entities are the same", TestEntity.GetFlecsId(), TestEntity2.GetFlecsId());
		});
	});

	Describe("Enum Component Addition/Removal", [this]()
	{
		It("Should Add an Enum Component using CPP API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();

			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<ETestEnum>(ETestEnum::One);
			
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum>(flecs::Wildcard));
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum>()));
		});

		It("Should Add an Enum Component using Static API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum>());
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<ETestEnum>(), static_cast<int64>(ETestEnum::One));
			
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum>()));
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum>(flecs::Wildcard));
		});

		It("Should Remove an Enum Component using CPP API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<ETestEnum>();

			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<ETestEnum>(ETestEnum::One);
			
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum>(flecs::Wildcard));
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum>()));
			
			TestEntity.RemovePair<ETestEnum>(flecs::Wildcard);

			TestFalse("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum>()));
			TestFalse("Entity has Enum Component", TestEntity.HasPair<ETestEnum>(flecs::Wildcard));
		});

		It("Should Remove an Enum Component using Static API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum>());
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<ETestEnum>(), static_cast<int64>(ETestEnum::One));
			
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum>()));
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum>(flecs::Wildcard));
			
			TestEntity.Remove(StaticEnum<ETestEnum>());
			
			TestFalse("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum>()));
			TestFalse("Entity has Enum Component", TestEntity.HasPair<ETestEnum>(flecs::Wildcard));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
