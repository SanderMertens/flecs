
#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Transforms/FlecsTransformComponents.h"

BEGIN_DEFINE_SPEC(FComponentCollectionTestSpec,
                  "Flecs.Components.Collections",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FComponentCollectionTestSpec);

void FComponentCollectionTestSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("Component Collection Addition", [this]()
	{
		It("Should be able to add a Component Collection and then check if the entity has the Collection's Components",
			[this]()
		{
			FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			UFlecsTransform3dCollection* Collection = NewObject<UFlecsTransform3dCollection>();
			Collection->Location = FFlecsLocationComponent(FVector(1.f, 2.f, 3.f));
			TestEntity.AddCollection(Collection);
			TestTrue("Entity has 3d Transform Collection",
				TestEntity.HasCollection(UFlecsTransform3dCollection::StaticClass()));
			TestTrue("Entity has 3d Transform Collection",
				TestEntity.HasCollection<UFlecsTransform3dCollection>());
			TestTrue("Entity has Location Component", TestEntity.Has<FFlecsLocationComponent>());
			TestTrue("Entity has Rotation Component", TestEntity.Has<FFlecsRotationComponent>());
			TestTrue("Entity has Scale Component", TestEntity.Has<FFlecsScaleComponent>());

			TestEqual("Entity Location Component is correct",
				TestEntity.Get<FFlecsLocationComponent>().Location,
				FVector(1.f, 2.f, 3.f));
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
