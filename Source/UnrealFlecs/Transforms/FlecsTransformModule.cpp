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

	InWorld->CreateObserver<const FFlecsRelativeTrait&>(TEXT("RelativeTransformObserver"))
		.event(flecs::OnAdd)
		.event(flecs::OnSet)
		.each([](flecs::entity InEntity, const FFlecsRelativeTrait& InTrait)
		{
			const FFlecsEntityHandle ParentEntity = InEntity.parent();
			solid_checkf(ParentEntity.IsValid(), TEXT("Parent entity is invalid"));

			const int32 RelativeTraitIndex = InTrait.RelativeToIndex;

			static constexpr std::string_view PathSeparator = "::";

			const FString Path = ParentEntity.GetPath();
			solid_checkf(Path.Len() > 0, TEXT("Entity path cannot be empty"));
						
			const int32 SeparatorIndex = Path.Find(PathSeparator.data(),
				ESearchCase::IgnoreCase, ESearchDir::FromEnd, Path.Len() - RelativeTraitIndex);

			const FFlecsEntityHandle RelativeEntity
				= ParentEntity.GetFlecsWorld()->LookupEntity(Path.Left(SeparatorIndex));
			solid_checkf(RelativeEntity.IsValid(), TEXT("Relative entity is invalid"));

			ParentEntity.SetTrait<FFlecsTransformComponent, FFlecsRelativeCacheTrait>(FFlecsRelativeCacheTrait{ RelativeEntity });
		});
	
	InWorld->CreateSystemWithBuilder(
		TEXT("FlecsTransformPropagateSystem"))
		.kind(flecs::PreUpdate)
		.with<FFlecsTransformComponent>().read_write()
		.with<FFlecsTransformComponent>().parent().cascade()
		.and_()
		.with_trait<FFlecsTransformComponent, FFlecsDirtyTransformTrait>().inout_none()
		.and_()
		.immediate()
		.run([](flecs::iter& Iter)
		{
			while (Iter.next())
			{
				if (!Iter.other_table())
				{
					Iter.skip();
					continue;
				}

				flecs::field<FFlecsTransformComponent> TransformField = Iter.field<FFlecsTransformComponent>(0);
				flecs::field<FFlecsTransformComponent> ParentTransformField = Iter.field<FFlecsTransformComponent>(2);

				for (const flecs::entity_t Index : Iter)
				{
					FFlecsEntityHandle Entity = Iter.entity(Index);
					FFlecsTransformComponent& Transform = TransformField[Index];
					FFlecsTransformComponent& ParentTransform = ParentTransformField[Index];

					if (Entity.HasTrait<FFlecsTransformComponent, FFlecsRelativeTrait>())
					{
						if (Entity.HasTrait<FFlecsTransformComponent, FFlecsRelativeCacheTrait>())
						{
							continue;
						}

						const FFlecsTransformComponent RelativeTransform = Entity
							.GetTrait<FFlecsTransformComponent, FFlecsRelativeCacheTrait>()
							.RelativeEntity.Get<FFlecsTransformComponent>();
						
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
