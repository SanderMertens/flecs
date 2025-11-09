// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsDefaultGameLoop.h"

#include "FlecsOutsideMainLoopTag.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsDefaultGameLoop)

static NO_DISCARD FORCEINLINE int flecs_entity_compare(
	const ecs_entity_t e1,
	MAYBE_UNUSED const void* ptr1,
	const ecs_entity_t e2,
	MAYBE_UNUSED const void* ptr2)
{
	return (e1 > e2) - (e1 < e2);
}

#ifdef FLECS_ENABLE_SYSTEM_PRIORITY

static NO_DISCARD FORCEINLINE int flecs_priority_compare(
	const flecs::entity_t InEntityA,
	const flecs::SystemPriority* InPtrA,
	const flecs::entity_t InEntityB,
	const flecs::SystemPriority* InPtrB) 
{
	solid_check(InPtrA);
	solid_check(InPtrB);
	
	if (InPtrA->value == InPtrB->value)
	{
		return flecs_entity_compare(InEntityA, InPtrA, InEntityB, InPtrB);
	}
	else // lower value has higher priority
	{
		return InPtrA->value >= InPtrB->value ? 1 : -1;
	}
}

#endif // FLECS_ENABLE_SYSTEM_PRIORITY

void UFlecsDefaultGameLoop::InitializeGameLoop(TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InGameLoopEntity)
{
	MainPipeline = InWorld->CreatePipeline()
		.with(flecs::System)
		.with(flecs::Phase).cascade(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::ChildOf)
		#ifdef FLECS_ENABLE_SYSTEM_PRIORITY
		.with<flecs::SystemPriority>()
		#endif // FLECS_ENABLE_SYSTEM_PRIORITY
		.without<FFlecsOutsideMainLoopTag>()
		#ifdef FLECS_ENABLE_SYSTEM_PRIORITY
		.order_by<flecs::SystemPriority>(flecs_priority_compare)
		#else // FLECS_ENABLE_SYSTEM_PRIORITY
		.order_by(flecs_entity_compare)
		#endif // FLECS_ENABLE_SYSTEM_PRIORITY
		.build()
		.set_name("MainPipeline");

	InWorld->SetPipeline(MainPipeline);
}

bool UFlecsDefaultGameLoop::Progress(const double DeltaTime, const TSolidNotNull<UFlecsWorld*> InWorld)
{
	return InWorld->Progress(DeltaTime);
}

bool UFlecsDefaultGameLoop::IsMainLoop() const
{
	return true;
}
