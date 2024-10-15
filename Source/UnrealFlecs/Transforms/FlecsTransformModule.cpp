// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTransformModule.h"
#include "FlecsTransformComponent.h"
#include "FlecsTransformEngineSubsystem.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformModule)

UFlecsTransformModule::UFlecsTransformModule()
{
}

void UFlecsTransformModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	InWorld->CreateObserver(TEXT("DirtyGlobalTransformObserver"))
		.with<FFlecsTransformComponent>().event(flecs::OnSet).yield_existing()
		.with_trait<FFlecsTransformComponent, FFlecsDirtyTransformTrait>().filter()
		.without_trait<FFlecsTransformComponent>(FlecsGlobalTrait).filter()
		.each([](flecs::entity InEntity)
		{
			const FFlecsEntityHandle Entity = InEntity;
			Entity.AddTrait<FFlecsTransformComponent, FFlecsDirtyTransformTrait>();
		});

	/*.run([](flecs::iter& Iter)
		{
			while (Iter.next())
			{
				if (!Iter.changed())
				{
					Iter.skip();
					continue;
				}

				flecs::field<FFlecsTransformComponent> TransformField = Iter.field<FFlecsTransformComponent>(0);
				flecs::field<FFlecsTransformComponent> ParentTransformField = Iter.field<FFlecsTransformComponent>(1);

				for (const flecs::entity_t Index : Iter)
				{
					if UNLIKELY_IF(!Iter.is_set(1))
					{
						UN_LOGF(LogFlecsSystem, Log,
							"FlecsTransformPropagateSystem: Entity %s has no parent",
							Iter.entity(Index).name().c_str());
						continue;
					}

					FFlecsEntityHandle Entity = Iter.entity(Index);
					FFlecsTransformComponent& Transform = TransformField[Index];
					FFlecsTransformComponent& ParentTransform = ParentTransformField[Index];

					if (Entity.HasTrait<FFlecsTransformComponent, FFlecsRelativeTrait>())
					{
						const int32 RelativeTraitIndex
							= Entity.GetTrait<FFlecsTransformComponent, FFlecsRelativeTrait>().RelativeToIndex;

						static constexpr std::string_view PathSeparator = "::";

						FString Path = Entity.GetPath();
						solid_checkf(Path.Len() > 0, TEXT("Entity path cannot be empty"));
						
						const int32 SeparatorIndex = Path.Find(PathSeparator.data(),
								ESearchCase::IgnoreCase, ESearchDir::FromEnd, Path.Len() - RelativeTraitIndex);

						FFlecsEntityHandle RelativeEntity = Entity.GetFlecsWorld()->LookupEntity(Path.Left(SeparatorIndex));
						solid_checkf(RelativeEntity.IsValid(), TEXT("Relative entity is invalid"));

						const FFlecsTransformComponent RelativeTransform = RelativeEntity.Get<FFlecsTransformComponent>();
						Transform.GlobalTransform = RelativeTransform.GlobalTransform * Transform.Transform;
					}
					else if (Entity.HasTrait<FFlecsTransformComponent>(FlecsLocalTrait))
					{
						Transform.GlobalTransform = ParentTransform.GlobalTransform * Transform.Transform;
					}
					// implied global transform
					else
					{
						Transform.GlobalTransform = Transform.Transform;
					}
				}
			}
		});*/
	
	InWorld->CreateSystemWithBuilder(
		TEXT("FlecsTransformPropagateSystem"))
		.kind(flecs::PreUpdate)
		.with<FFlecsTransformComponent>()
		.with<FFlecsTransformComponent>().parent().cascade()
		.with_trait<FFlecsTransformComponent, FFlecsDirtyTransformTrait>().inout_none()
		.immediate()
		.run([](flecs::iter& Iter)
		{
			while (Iter.next())
			{
				if (!Iter.changed())
				{
					Iter.skip();
					continue;
				}

				flecs::field<FFlecsTransformComponent> TransformField = Iter.field<FFlecsTransformComponent>(0);
				flecs::field<FFlecsTransformComponent> ParentTransformField = Iter.field<FFlecsTransformComponent>(1);

				for (const flecs::entity_t Index : Iter)
				{
					if UNLIKELY_IF(!Iter.is_set(1))
					{
						UN_LOGF(LogFlecsSystem, Log,
							"FlecsTransformPropagateSystem: Entity %s has no parent",
							Iter.entity(Index).name().c_str());
						continue;
					}

					FFlecsEntityHandle Entity = Iter.entity(Index);
					FFlecsTransformComponent& Transform = TransformField[Index];
					FFlecsTransformComponent& ParentTransform = ParentTransformField[Index];

					if (Entity.HasTrait<FFlecsTransformComponent, FFlecsRelativeTrait>())
					{
						const int32 RelativeTraitIndex
							= Entity.GetTrait<FFlecsTransformComponent, FFlecsRelativeTrait>().RelativeToIndex;

						static constexpr std::string_view PathSeparator = "::";

						FString Path = Entity.GetPath();
						solid_checkf(Path.Len() > 0, TEXT("Entity path cannot be empty"));
						
						const int32 SeparatorIndex = Path.Find(PathSeparator.data(),
								ESearchCase::IgnoreCase, ESearchDir::FromEnd, Path.Len() - RelativeTraitIndex);

						FFlecsEntityHandle RelativeEntity = Entity.GetFlecsWorld()->LookupEntity(Path.Left(SeparatorIndex));
						solid_checkf(RelativeEntity.IsValid(), TEXT("Relative entity is invalid"));

						const FFlecsTransformComponent RelativeTransform = RelativeEntity.Get<FFlecsTransformComponent>();
						Transform.GlobalTransform = RelativeTransform.GlobalTransform * Transform.Transform;
					}
					else if (Entity.HasTrait<FFlecsTransformComponent>(FlecsLocalTrait))
					{
						Transform.GlobalTransform = ParentTransform.GlobalTransform * Transform.Transform;
					}
					// implied global transform
					else
					{
						Transform.GlobalTransform = Transform.Transform;
					}
				}
			}
		});
}

void UFlecsTransformModule::DeinitializeModule(UFlecsWorld* InWorld)
{
}
