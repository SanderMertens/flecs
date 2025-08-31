#if WITH_AUTOMATION_TESTS

#include "Entities/FlecsEntityHandle.h"
#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Worlds/FlecsWorld.h"


BEGIN_DEFINE_SPEC(FStringComponentTestsSpec,
                  "Flecs.Components.Strings",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FStringComponentTestsSpec);

void FStringComponentTestsSpec::Define()
{
	FLECS_FIXTURE_LIFECYCLE(Fixture);

	Describe("String Component", [this]
	{
		It("Should be able to add a string component to an entity", [this]
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set<FString>(FString(TEXT("Hello, World!")));

			FString TestSerialization = TestEntity.ToJson();
		});

		It("Should be able to add a FName component to an entity", [this]
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set<FName>(FName(TEXT("World")));

			FString TestSerialization = TestEntity.ToJson();
		});

		It("Should be able to add a FText component to an entity", [this]
		{
			const FFlecsEntityHandle TestEntity = Fixture.FlecsWorld->CreateEntity();
			TestEntity.Set<FText>(FText::FromString(TEXT("Hello, World!")));

			FString TestSerialization = TestEntity.ToJson();
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
