
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Tests/FlecsTestTypes.h"
#include "Worlds/FlecsWorld.h"

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
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity is an Enum Component", TestEntity.IsEnum());
		});

		It("Should Register an Enum Component in CPP", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
			
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity is an Enum Component", TestEntity.IsEnum());
		});

		It("Should Register an Enum Component in Static then CPP", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity is an Enum Component", TestEntity.IsEnum());
			
			const FFlecsEntityHandle TestEntity2 = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
			TestTrue("Entity is Component", TestEntity2.IsComponent());
			TestTrue("Entity is an Enum Component", TestEntity2.IsEnum());

			TestEqual("Both Entities are the same (as they are registered just under different APIs)",
				TestEntity.GetFlecsId(), TestEntity2.GetFlecsId());
		});

		It("Should Register an Enum Component in CPP then Static", [this]()
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
			TestTrue("Entity is Component", TestEntity.IsComponent());
			TestTrue("Entity is an Enum Component", TestEntity.IsEnum());
			
			const FFlecsEntityHandle TestEntity2 = Fixture.FlecsWorld->ObtainComponentTypeEnum(StaticEnum<EFlecsTestEnum_UENUM>());
			TestTrue("Entity is Component", TestEntity2.IsComponent());
			TestTrue("Entity is an Enum Component", TestEntity2.IsEnum());

			TestEqual("Both Entities are the same (as they are registered just under different APIs)",
				TestEntity.GetFlecsId(),
				TestEntity2.GetFlecsId());
		});
	});

	Describe("Enum Component Addition/Removal", [this]()
	{
		It("Should Add an Enum Component using CPP API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();

			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One);
			
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
			
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
		});

		It("Should Add an Enum Component using Static API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One));
			
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
		});

		It("Should Register an Enum Component with CPP API and use Static API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One));
			
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
		});
		
		It("Should Register an Enum Component with Static API and use CPP API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One);
			
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
		});

		It("Should Remove an Enum Component using CPP API", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();

			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<EFlecsTestEnum_UENUM>(EFlecsTestEnum_UENUM::One);
			
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
			
			TestEntity.RemovePair<EFlecsTestEnum_UENUM>(flecs::Wildcard);

			TestFalse("Entity has Enum Component using CPP API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
			TestFalse("Entity has Enum Component using Static API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
		});

		It("Should Remove an Enum Component using Static API", [this]()
		{
			const FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
			
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<EFlecsTestEnum_UENUM>(), static_cast<int64>(EFlecsTestEnum_UENUM::One));
			
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
			
			TestEntity.Remove(StaticEnum<EFlecsTestEnum_UENUM>());
			
			TestFalse("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_UENUM>()));
			TestFalse("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_UENUM>(flecs::Wildcard));
		});
	});

	Describe("Sparse Enum Components", [this]()
	{
		It("Should Register an Enum Component with Static API with Sparse UENUM", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_SparseUENUM>());
			
			TestTrue("Entity is Component", TestComponent.IsComponent());
			TestTrue("Entity has Enum Component", TestComponent.IsEnum());
		});

		It("Should Register an Enum Component with CPP API with Sparse UENUM", [this]()
		{
			const FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_SparseUENUM>();
			
			TestTrue("Entity is Component", TestComponent.IsComponent());
			TestTrue("Entity is an Enum Component", TestComponent.IsEnum());
		});

		It("Should Add an Enum Component using Static API with Sparse UENUM", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_SparseUENUM>());
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<EFlecsTestEnum_SparseUENUM>(), static_cast<int64>(EFlecsTestEnum_SparseUENUM::Five));
			
			TestTrue("Entity has Enum Component", TestEntity.Has(StaticEnum<EFlecsTestEnum_SparseUENUM>()));
			TestTrue("Entity has Enum Component", TestEntity.HasPair<EFlecsTestEnum_SparseUENUM>(flecs::Wildcard));
		});

		It("Should Add an Enum Component using CPP API with Sparse UENUM", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_SparseUENUM>();
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<EFlecsTestEnum_SparseUENUM>(EFlecsTestEnum_SparseUENUM::Five);
			
			TestTrue("Entity has Enum Component",
				TestEntity.HasPair<EFlecsTestEnum_SparseUENUM>(flecs::Wildcard));
			TestTrue("Entity has Enum Component",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_SparseUENUM>()));
		});

		It("Should Remove an Enum Component using Static API with Sparse UENUM", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_SparseUENUM>());
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add(StaticEnum<EFlecsTestEnum_SparseUENUM>(), static_cast<int64>(EFlecsTestEnum_SparseUENUM::Five));
			
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_SparseUENUM>()));
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_SparseUENUM>(flecs::Wildcard));
			
			TestEntity.Remove(StaticEnum<EFlecsTestEnum_SparseUENUM>());
			
			TestFalse("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_SparseUENUM>()));
			TestFalse("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_SparseUENUM>(flecs::Wildcard));
		});

		It("Should Remove an Enum Component using CPP API with Sparse UENUM", [this]()
		{
			FFlecsEntityHandle TestComponent = Fixture.FlecsWorld->RegisterComponentType<EFlecsTestEnum_SparseUENUM>();
			
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Add<EFlecsTestEnum_SparseUENUM>(EFlecsTestEnum_SparseUENUM::Five);
			
			TestTrue("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_SparseUENUM>(flecs::Wildcard));
			TestTrue("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_SparseUENUM>()));
			
			TestEntity.RemovePair<EFlecsTestEnum_SparseUENUM>(flecs::Wildcard);

			TestFalse("Entity has Enum Component using Static API",
				TestEntity.Has(StaticEnum<EFlecsTestEnum_SparseUENUM>()));
			TestFalse("Entity has Enum Component using CPP API",
				TestEntity.HasPair<EFlecsTestEnum_SparseUENUM>(flecs::Wildcard));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
