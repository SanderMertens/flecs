// Elie Wiese-Namir © 2025. All Rights Reserved.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Fixtures/Commands/FlecsWorldTestCommands.h"

BEGIN_DEFINE_SPEC(FFlecsWorldLifecycleTestsSpec,
                  "Flecs.World.LifecycleTests",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

	FFlecsTestFixture Fixture;

END_DEFINE_SPEC(FFlecsWorldLifecycleTestsSpec)

void FFlecsWorldLifecycleTestsSpec::Define()
{
	Describe("World Creation", [this]()
	{
		FLECS_FIXTURE_LIFECYCLE(Fixture);
		
		It("Should create a valid Flecs world", [this]()
		{
			TestTrue("Flecs world is valid", Fixture.FlecsWorld.IsValid());
		});
	});

	Describe("World Ptr Tests", [this]()
	{
		FLECS_FIXTURE_LIFECYCLE(Fixture);
		
		It("Should be able to convert flecs::world to UFlecsWorld using ToFlecsWorld Function",
			[this]()
		{
			TestTrue("Flecs world is valid", Fixture.FlecsWorld.IsValid());
				
			TestNotNull("Flecs world singleton is valid",
				Unreal::Flecs::ToFlecsWorld(Fixture.FlecsWorld->World));
		});
	});

	Describe("Latent World Testing", [this]()
	{
		FLECS_FIXTURE_LIFECYCLE_LATENT(Fixture);
		
		LatentIt("Should be able to progress world", [this](const FDoneDelegate& Done)
		{
			Fixture.FlecsWorld->CreateSystemWithBuilder("TestSystem")
				.run([Done](flecs::iter& Iter)
				{
					Done.ExecuteIfBound();
				});

			Fixture.TickWorld();
			Fixture.TickWorld();
			Fixture.TickWorld();
		});

		LatentIt("Should be able to progress world with multiple ticks", [this](const FDoneDelegate& Done)
		{
			int32 TickCount = 0;

			Fixture.FlecsWorld->CreateSystemWithBuilder("TestSystem")
				.run([&TickCount, Done](flecs::iter& Iter)
				{
					TickCount++;
					if (TickCount >= 3)
					{
						Done.ExecuteIfBound();
					}
				});

			for (int32 i = 0; i < 5; ++i)
			{
				Fixture.TickWorld();
			}
		});
	});
}

#endif // WITH_AUTOMATION_TESTS