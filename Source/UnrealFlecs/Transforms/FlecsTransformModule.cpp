// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTransformModule.h"
#include "FlecsGlobalTransformComponent.h"
#include "FlecsTransformComponent.h"
#include "FlecsTransformDefaultEntities.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformModule)

UFlecsTransformModule::UFlecsTransformModule()
{
}

void UFlecsTransformModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	/*InWorld->CreateObserver<const FFlecsRelativeTrait&>(TEXT("RelativeTransformObserver"))
		.event(flecs::OnAdd)
		.event(flecs::OnSet)
		.yield_existing()
		.each([](flecs::entity InEntity, const FFlecsRelativeTrait& InTrait)
		{
			const FFlecsEntityHandle TraitParentEntity = InEntity.parent();
			solid_checkf(TraitParentEntity.IsValid(), TEXT("Parent entity is invalid"));
			solid_check(TraitParentEntity.HasPair<FFlecsTransformComponent>(InEntity));

			const int32 RelativeTraitIndex = InTrait.RelativeToIndex;

			static constexpr std::string_view PathSeparator = "::";

			const FString Path = TraitParentEntity.GetPath();
			solid_checkf(Path.Len() > 0, TEXT("Entity path cannot be empty"));
						
			const int32 SeparatorIndex = Path.Find(PathSeparator.data(),
				ESearchCase::IgnoreCase, ESearchDir::FromEnd, Path.Len() - RelativeTraitIndex);

			const FFlecsEntityHandle RelativeEntity
				= TraitParentEntity.GetFlecsWorld()->LookupEntity(Path.Left(SeparatorIndex));
			solid_checkf(RelativeEntity.IsValid(), TEXT("Relative entity is invalid"));
		});*/

	const flecs::entity_t FlecsTransformSystemKind = flecs::PreFrame;

	InWorld->CreateSystemWithBuilder<FFlecsTransformComponent>(
		TEXT("FlecsGlobalTransformPropagateSystem"))
		.kind(FlecsTransformSystemKind)
		.term_at(0).read()
		.begin_scope_traits<FFlecsTransformComponent>()
			.without(FlecsLocalTrait)
		.end_scope_traits()
		.write<FFlecsGlobalTransformComponent>().optional()
		.each([](flecs::iter& Iter, size_t Index, FFlecsTransformComponent& InTransform)
		{
			Iter.entity(Index).set<FFlecsGlobalTransformComponent>(
				InTransform.GetTransform());
		});
	
	InWorld->CreateSystemWithBuilder<
		FFlecsTransformComponent, const FFlecsGlobalTransformComponent*>(
			TEXT("FlecsLocalTransformPropagateSystem"))
		.kind(FlecsTransformSystemKind)
		.term_at(0).read()
		.term_at(1).parent().cascade().read()
		.begin_scope_traits<FFlecsTransformComponent>()
			.with(FlecsLocalTrait)
		.end_scope_traits()
		.write<FFlecsGlobalTransformComponent>().optional()
		.each([](flecs::iter& Iter, size_t Index, FFlecsTransformComponent& InTransform,
				const FFlecsGlobalTransformComponent* InParentTransform)
		{
			Iter.entity(Index).set<FFlecsGlobalTransformComponent>(
				InTransform.GetTransform() * InParentTransform->GetTransform());
		});
}

void UFlecsTransformModule::DeinitializeModule(UFlecsWorld* InWorld)
{
}
