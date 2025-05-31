
#if WITH_AUTOMATION_TESTS

#include "flecs.h"

#include "Misc/AutomationTest.h"
#include "HAL/PlatformProcess.h"
#include "Worlds/FlecsOSAPI.h"

BEGIN_DEFINE_SPEC(FFlecsOSApiTestsSpec,
                  "Flecs.API.OS-API.Tests",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

END_DEFINE_SPEC(FFlecsOSApiTestsSpec);

void FFlecsOSApiTestsSpec::Define()
{
    Describe("Memory", [this]
    {
	    It("Should Allocate Memory", [this]
	    {
		    void* Memory = ecs_os_malloc(10);
	    	TestNotNull("Memory should not be null", Memory);
	    	ecs_os_free(Memory);
	    	Memory = nullptr;
	    });

    	It("Should Reallocate Memory", [this]
		{
			void* Memory = ecs_os_malloc(10);
    		TestNotNull("Memory should not be null", Memory);
    		void* NewMemory = ecs_os_realloc(Memory, 20);
    		TestNotNull("Memory should not be null", NewMemory);
    		ecs_os_free(NewMemory);
    		NewMemory = nullptr;
		});
    });

	Describe("Time", [this]
	{
		It("Should Get Time Now", [this]
		{
			const uint32_t Time = ecs_os_now();
			TestTrue("Time should be greater than 0", Time > 0);
		});

		It("Should Sleep 10000 nanosecs", [this]
		{
			ecs_time_t Time = {};
			ecs_time_measure(&Time);
			ecs_os_sleep(0, 10000);
			const double Elapsed = ecs_time_measure(&Time);
			TestTrue("Elapsed time should be greater than 0", Elapsed > 0);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
