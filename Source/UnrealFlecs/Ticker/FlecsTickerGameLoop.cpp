// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTickerGameLoop.h"

#include "Worlds/FlecsWorld.h"
#include "FlecsFixedTickSystemTag.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTickerGameLoop)

DECLARE_STATS_GROUP(TEXT("FlecsTickerGameLoop"), STATGROUP_FlecsTickerGameLoop, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("UFlecsTickerGameLoop::Progress"),
	STAT_FlecsFlecsTickerGameLoop_Progress, STATGROUP_FlecsTickerGameLoop);
DECLARE_CYCLE_STAT(TEXT("UFlecsTickerGameLoop::Progress::RunPipeline"),
	STAT_FlecsTickerGameLoop_Progress_RunPipeline, STATGROUP_FlecsTickerGameLoop);

DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("UFlecsTickerGameLoop::Progress::RunPipeline::Ticks"),
	STAT_FlecsTickerGameLoop_Progress_RunPipeline_Ticks, STATGROUP_FlecsTickerGameLoop);

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

void UFlecsTickerGameLoop::InitializeGameLoop(const TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InGameLoopEntity)
{
	InWorld->RegisterComponentType<FFlecsTickerSingletonComponent>();
	InWorld->RegisterComponentType<FFlecsFixedTickSystemTag>();
	
	InWorld->AddSingleton<FFlecsTickerSingletonComponent>();

	TickerComponentPtr = InWorld->TryGetMutSingleton<FFlecsTickerSingletonComponent>();
	solid_cassume(TickerComponentPtr);
	
	TickerPipeline = InWorld->CreatePipeline()
		.with(flecs::System)
		.with(flecs::Phase).cascade(flecs::DependsOn)
		.with<FFlecsFixedTickSystemTag>()
		.without(flecs::Disabled).up(flecs::DependsOn)
		.without(flecs::Disabled).up(flecs::ChildOf)
		#ifdef FLECS_ENABLE_SYSTEM_PRIORITY
		.with<flecs::SystemPriority>()
		.order_by<flecs::SystemPriority>(flecs_priority_compare)
		#else // FLECS_ENABLE_SYSTEM_PRIORITY
		.order_by(flecs_entity_compare)
		#endif // FLECS_ENABLE_SYSTEM_PRIORITY
		.build()
		.set_name("TickerPipeline");

	TickerInterval = 1.0 / static_cast<double>(TickerRate);
}

bool UFlecsTickerGameLoop::Progress(double DeltaTime, TSolidNotNull<UFlecsWorld*> InWorld)
{
	SCOPE_CYCLE_COUNTER(STAT_FlecsFlecsTickerGameLoop_Progress);

	TickerAccumulator += DeltaTime;
	
	while (TickerAccumulator >= TickerInterval)
	{
		SCOPE_CYCLE_COUNTER(STAT_FlecsTickerGameLoop_Progress_RunPipeline);

		solid_cassume(TickerComponentPtr);
		
		TickerAccumulator -= TickerInterval;
		
		++TickerComponentPtr->TickId;
		INC_DWORD_STAT(STAT_FlecsTickerGameLoop_Progress_RunPipeline_Ticks);

		solid_checkf(TickerPipeline.IsValid(), TEXT("TickerPipeline is not valid!"));
		InWorld->RunPipeline(TickerPipeline, TickerInterval);
	}
	
	return true;
}

bool UFlecsTickerGameLoop::IsMainLoop() const
{
	return false;
}
