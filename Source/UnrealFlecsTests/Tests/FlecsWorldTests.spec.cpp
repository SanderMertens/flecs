#include "Misc/AutomationTest.h"
#include "UnrealFlecsTests/Helpers/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FlecsWorldTests,
                  "UnrealFlecs.Flecs.World",
                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

	TUniquePtr<FFlecsWorldFixture> WorldFixture;

END_DEFINE_SPEC(FlecsWorldTests)

void FlecsWorldTests::Define()
{
	Describe("World", [&]()
	{
		BeforeEach([&]()
		{
			WorldFixture = MakeUnique<FFlecsWorldFixture>();
		});

		AfterEach([&]()
		{
			WorldFixture.Reset();
		});

		It("Should create a Flecs world", [&]()
		{
			TestTrue("Flecs world is valid", IsValid(WorldFixture->GetFlecsWorld()));
		});
	});
}


