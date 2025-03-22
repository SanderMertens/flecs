
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
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum_UENUM>());
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
		});

		It("Should Register an Enum Component in CPP", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterComponentType<ETestEnum_UENUM>();
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
		});

		It("Should Register an Enum Component in Static then CPP", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum_UENUM>());
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
			
			FFlecsEntityHandle TestEntity2 = Fixture.FlecsWorld->RegisterComponentType<ETestEnum_UENUM>();
			TestTrue("Entity is Component", TestEntity2.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity2.IsEnum());

			TestEqual("Both Entities are the same", TestEntity.GetFlecsId(), TestEntity2.GetFlecsId());
		});

		It("Should Register an Enum Component in CPP then Static", [this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterComponentType<ETestEnum_UENUM>();
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity.IsEnum());
			
			FFlecsEntityHandle TestEntity2 = Fixture.FlecsWorld->ObtainComponentTypeEnum(StaticEnum<ETestEnum_UENUM>());
			TestTrue("Entity is Component", TestEntity2.IsComponent());
			TestTrue("Entity has Enum Component", TestEntity2.IsEnum());

			TestEqual("Both Entities are the same", TestEntity.GetFlecsId(), TestEntity2.GetFlecsId());
		});
	});

	Describe("Enum Component Addition/Removal", [this]()
	{
		It("Should Add an Enum Component using CPP API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<ETestEnum_UENUM>();

			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<ETestEnum_UENUM>(ETestEnum_UENUM::One);
			
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum_UENUM>(flecs::Wildcard));
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum_UENUM>()));
		});

		It("Should Add an Enum Component using Static API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum_UENUM>());
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<ETestEnum_UENUM>(), static_cast<int64>(ETestEnum_UENUM::One));
			
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum_UENUM>()));
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum_UENUM>(flecs::Wildcard));
		});

		It("Should Remove an Enum Component using CPP API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<ETestEnum_UENUM>();

			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<ETestEnum_UENUM>(ETestEnum_UENUM::One);
			
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum_UENUM>(flecs::Wildcard));
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum_UENUM>()));
			
			TestEntity.RemovePair<ETestEnum_UENUM>(flecs::Wildcard);

			TestFalse("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum_UENUM>()));
			TestFalse("Entity has Enum Component", TestEntity.HasPair<ETestEnum_UENUM>(flecs::Wildcard));
		});

		It("Should Remove an Enum Component using Static API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<ETestEnum_UENUM>());
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<ETestEnum_UENUM>(), static_cast<int64>(ETestEnum_UENUM::One));
			
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum_UENUM>()));
			TestTrue("Entity has Enum Component", TestEntity.HasPair<ETestEnum_UENUM>(flecs::Wildcard));
			
			TestEntity.Remove(StaticEnum<ETestEnum_UENUM>());
			
			TestFalse("Entity has Enum Component", TestEntity.Has(StaticEnum<ETestEnum_UENUM>()));
			TestFalse("Entity has Enum Component", TestEntity.HasPair<ETestEnum_UENUM>(flecs::Wildcard));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
