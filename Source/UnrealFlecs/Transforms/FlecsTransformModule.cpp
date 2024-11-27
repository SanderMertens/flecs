// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTransformModule.h"
#include "FlecsTransformComponent.h"
#include "FlecsTransformDefaultEntities.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformModule)

UFlecsTransformModule::UFlecsTransformModule()
{
}

void UFlecsTransformModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	InWorld->CreateObserver<const FFlecsRelativeTrait&>(TEXT("RelativeTransformObserver"))
		.event(flecs::OnAdd)
		.event(flecs::OnSet)
		.yield_existing()
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

			ParentEntity
				.SetTrait<FFlecsTransformComponent, FFlecsRelativeCacheTrait>(FFlecsRelativeCacheTrait{ RelativeEntity });
		});
	
	InWorld->CreateSystemWithBuilder<
		FFlecsTransformComponent>(TEXT("FlecsGlobalTransformPropagateSystem"))
		.kind(flecs::PreUpdate)
		.term_at(0).read_write()
		.begin_scope_traits<FFlecsTransformComponent>()
			.with(FlecsGlobalTrait)
		.end_scope_traits()
		.each([](flecs::iter& Iter, size_t Index, FFlecsTransformComponent& InTransform)
		{
			InTransform.GlobalTransform = InTransform.Transform;
		});

	InWorld->CreateSystemWithBuilder<
		FFlecsTransformComponent>(TEXT("FlecsLocalTransformPropagateSystem"))
		.kind(flecs::PreUpdate)
		.term_at(0).read_write()
		.with<FFlecsTransformComponent>().parent().cascade()
		.begin_scope_traits<FFlecsTransformComponent>()
			.with(FlecsLocalTrait)
		.end_scope_traits()
		.each([](flecs::iter& Iter, size_t Index,
			FFlecsTransformComponent& InTransform)
		{
			InTransform.GlobalTransform
				= Iter.field_at<FFlecsTransformComponent>(1, Index).GetTransform() * InTransform.Transform;
		});

	
}

void UFlecsTransformModule::DeinitializeModule(UFlecsWorld* InWorld)
{
}
