#include "Misc/AutomationTest.h"
#include "UnrealFlecsTests/Helpers/FlecsWorldFixture.h"

BEGIN_DEFINE_SPEC(FlecsEntityTests, "UnrealFlecs.Flecs.Entity",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

	TUniquePtr<FFlecsWorldFixture> WorldFixture;

END_DEFINE_SPEC(FlecsEntityTests)

namespace FlecsEntityTesting
{
	struct FTestComponent
	{
		int32 Value;
	}; // struct FTestComponent

	struct FTestTrait
	{
		int32 Value;
	}; // struct FTestTrait
	
} // namespace FlecsEntityTesting
 
void FlecsEntityTests::Define()
{
	Describe("Entity", [&]()
	{
		BeforeEach([&]()
		{
			WorldFixture = MakeUnique<FFlecsWorldFixture>();
		});

		AfterEach([&]()
		{
			WorldFixture.Reset();
		});

		It("Should create and destroy an entity", [&]()
		{
			const FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity();
			TestTrue("Entity is valid", TestEntity.IsValid());

			TestEntity.Destroy();
			TestTrue("Entity is not valid", !TestEntity.IsValid());
		});

		It("Should create an Entity with specific Id", [&]()
		{
			constexpr flecs::entity_t EntityId = 42;
			const FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntityWithId(EntityId);
			TestEqual("Entity Id is correct", TestEntity.GetId(), static_cast<uint32>(EntityId));
		});

		It("Should create an Entity with Name", [&]()
		{
			const FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity(TEXT("TestEntity"));
			TestTrue("Entity has name", TestEntity.HasName());
			TestEqual("Entity name is correct", TestEntity.GetName(), TEXT("TestEntity"));
		});

		It("Should create an Entity with a Component", [&]()
		{
			const FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity();
			TestEntity.Set<FlecsEntityTesting::FTestComponent>({ 42 });
			
			TestTrue("Entity has component", TestEntity.Has<FlecsEntityTesting::FTestComponent>());
			TestEqual("Component value is correct", TestEntity.Get<FlecsEntityTesting::FTestComponent>().Value, 42);
		});

		It("Should create an Entity with a Component and Trait", [&]()
		{
			const FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity();
			TestEntity.Set<FlecsEntityTesting::FTestComponent>({ 42 });
			TestEntity.SetTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>(
				{ 24 });
			
			TestTrue("Entity has component", TestEntity.Has<FlecsEntityTesting::FTestComponent>());
			TestEqual("Component value is correct", TestEntity.Get<FlecsEntityTesting::FTestComponent>().Value, 42);
			TestTrue("Entity has trait", TestEntity.HasTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>());
			TestEqual("Trait value is correct",
				TestEntity.GetTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>().Value, 24);
		});

		It("Should create an Entity with a Component and Trait then remove Trait", [&]()
		{
			const FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity();
			TestEntity.Set<FlecsEntityTesting::FTestComponent>({ 42 });
			TestEntity.SetTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>(
				{ 24 });
			TestEntity.RemoveTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>();
			
			TestTrue("Entity has component", TestEntity.Has<FlecsEntityTesting::FTestComponent>());
			TestEqual("Component value is correct", TestEntity.Get<FlecsEntityTesting::FTestComponent>().Value, 42);
			TestFalse("Entity has no trait",
				TestEntity.HasTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>());
		});

		It("Should create an Entity with a Component and Trait then remove Component", [&]()
		{
			const FFlecsEntityHandle TestEntity = WorldFixture->GetFlecsWorld()->CreateEntity();
			TestEntity.Set<FlecsEntityTesting::FTestComponent>({ 42 });
			TestEntity.SetTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>(
				{ 24 });
			TestEntity.Remove<FlecsEntityTesting::FTestComponent>();
			
			TestFalse("Entity has no component", TestEntity.Has<FlecsEntityTesting::FTestComponent>());
			TestFalse("Entity has no trait",
				TestEntity.HasTrait<FlecsEntityTesting::FTestComponent, FlecsEntityTesting::FTestTrait>());
		});

		It("Should verify components are copied on cloning", [&]()
		{
			const FFlecsEntityHandle Entity = WorldFixture->GetFlecsWorld()->CreateEntity();
			Entity.Set<FlecsEntityTesting::FTestComponent>({ 42 });

			const FFlecsEntityHandle ClonedEntity = Entity.Clone();

			TestTrue("Cloned entity is valid", ClonedEntity.IsValid());
			TestTrue("Cloned entity has component", ClonedEntity.Has<FlecsEntityTesting::FTestComponent>());
			TestEqual("Cloned component value is correct",
				ClonedEntity.Get<FlecsEntityTesting::FTestComponent>().Value, 42);
		});
		
	});
// Additional test
		
	
}


