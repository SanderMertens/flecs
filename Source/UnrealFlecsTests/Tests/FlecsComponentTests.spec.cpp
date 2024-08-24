#include "Misc/AutomationTest.h"
#include "UnrealFlecsTests/Helpers/FlecsWorldFixture.h"
#include "UnrealFlecsTests/Helpers/TestUSTRUCTComponent.h"

BEGIN_DEFINE_SPEC(FlecsComponentTests,
                  "UnrealFlecs.Flecs.Component",
                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

	TUniquePtr<FFlecsWorldFixture> WorldFixture;

END_DEFINE_SPEC(FlecsComponentTests)

void FlecsComponentTests::Define()
{
	Describe("Component", [&]()
	{
		BeforeEach([&]()
		{
			WorldFixture = MakeUnique<FFlecsWorldFixture>();
		});

		AfterEach([&]()
		{
			WorldFixture.Reset();
		});

		It("Should register component using UScriptStruct", [&]()
		{
			FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity();
			TestEntity.Add(FTestUSTRUCTComponent::StaticStruct());
			TestTrue("Component is registered", TestEntity.Has<FTestUSTRUCTComponent>());
			FFlecsEntityHandle ComponentEntity
				= WorldFixture->GetFlecsWorld()->ObtainComponentTypeStruct(FTestUSTRUCTComponent::StaticStruct());
			TestTrue("Has the flecs::Inherit trait", ComponentEntity.Has(flecs::Inherit));
		});

		It("Should remove component using UScriptStruct", [&]()
		{
			FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity();
			TestEntity.Add(FTestUSTRUCTComponent::StaticStruct());
			TestTrue("Component is registered", TestEntity.Has<FTestUSTRUCTComponent>());
			TestEntity.Remove(FTestUSTRUCTComponent::StaticStruct());
			TestFalse("Component is removed", TestEntity.Has<FTestUSTRUCTComponent>());
		});
	});
}